#include "esp_lcd_panel_st7789.h"

static const char *TAG = "lcd_panel.st7789";

static esp_err_t panel_st7789_del(esp_lcd_panel_t *panel);
static esp_err_t panel_st7789_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_st7789_init(esp_lcd_panel_t *panel);
static esp_err_t panel_st7789_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
static esp_err_t panel_st7789_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t panel_st7789_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t panel_st7789_swap_xy(esp_lcd_panel_t *panel, bool swap_axes);
static esp_err_t panel_st7789_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_st7789_disp_on_off(esp_lcd_panel_t *panel, bool off);

typedef struct
{
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    // bool reset_level;
    int x_gap;
    int y_gap;
    uint8_t fb_bits_per_pixel;
    uint8_t madctl_val; // save current value of LCD_CMD_MADCTL register
    // uint8_t colmod_cal; // save surrent value of LCD_CMD_COLMOD register
} st7789_panel_t;

esp_err_t new_panel_st7789(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    st7789_panel_t *st7789 = NULL;
    ESP_GOTO_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    st7789 = calloc(1, sizeof(st7789_panel_t));
    ESP_GOTO_ON_FALSE(st7789, ESP_ERR_NO_MEM, err, TAG, "no mem for st7789 panel");

    if (panel_dev_config->reset_gpio_num >= 0)
    {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << panel_dev_config->reset_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configure GPIO for RST line failed");
    }
    st7789->madctl_val = 0x00;
    uint8_t fb_bits_per_pixel = 0;
    switch (panel_dev_config->bits_per_pixel)
    {
    case 16: // RGB565
        fb_bits_per_pixel = 16;
        break;
    case 18: // RGB666
        // each color component (R/G/B) should occupy the 6 high bits of a byte, which means 3 full bytes are required for a pixel
        fb_bits_per_pixel = 24;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "unsupported pixel width");
        break;
    }

    st7789->io = io;
    st7789->fb_bits_per_pixel = fb_bits_per_pixel;
    st7789->reset_gpio_num = panel_dev_config->reset_gpio_num;
    st7789->base.reset = panel_st7789_reset;
    st7789->base.init = panel_st7789_init;
    st7789->base.draw_bitmap = panel_st7789_draw_bitmap;
    st7789->base.invert_color = panel_st7789_invert_color;
    st7789->base.set_gap = panel_st7789_set_gap;
    st7789->base.mirror = panel_st7789_mirror;
    st7789->base.swap_xy = panel_st7789_swap_xy;
    st7789->base.disp_on_off = panel_st7789_disp_on_off;
    st7789->base.del = panel_st7789_del;
    *ret_panel = &(st7789->base);
    ESP_LOGD(TAG, "new st7789 panel @%p", st7789);

    return ESP_OK;

err:
    if (st7789)
    {
        if (panel_dev_config->reset_gpio_num >= 0)
        {
            gpio_reset_pin(panel_dev_config->reset_gpio_num);
        }
        free(st7789);
    }
    return ret;
}

static esp_err_t panel_st7789_del(esp_lcd_panel_t *panel)
{
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);

    if (st7789->reset_gpio_num >= 0)
    {
        gpio_reset_pin(st7789->reset_gpio_num);
    }
    ESP_LOGD(TAG, "del st7789 panel @%p", st7789);
    free(st7789);
    return ESP_OK;
}

static esp_err_t panel_st7789_reset(esp_lcd_panel_t *panel)
{
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7789->io;

    // perform hardware reset
    if (st7789->reset_gpio_num >= 0)
    {
        gpio_set_level(st7789->reset_gpio_num, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(st7789->reset_gpio_num, 1);
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    else
    { // perform software reset
        esp_lcd_panel_io_tx_param(io, LCD_CMD_SWRESET, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(20)); // spec, wait at least 5m before sending new command
    }

    return ESP_OK;
}

static esp_err_t panel_st7789_init(esp_lcd_panel_t *panel)
{
    ESP_LOGD(TAG, "panel_st7789_init");
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7789->io;
    //--------------------------------------------------官方--------------------------------------------------------------------------------
    // vTaskDelay(pdMS_TO_TICKS(120));
    // esp_lcd_panel_io_tx_param(io, ST7789_MADCTL, (uint8_t[]){0x00}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_COLMOD, (uint8_t[]){0x05}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_PORCTRL, (uint8_t[]){0x0C, 0x0C, 0x00, 0x33, 0x33}, 5);
    // esp_lcd_panel_io_tx_param(io, ST7789_GCTRL, (uint8_t[]){0x35}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_VCOMS, (uint8_t[]){0x19}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_LCMCTRL, (uint8_t[]){0x2C}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_VDVVRHEN, (uint8_t[]){0x01}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_VRHS, (uint8_t[]){0x12}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_VDVSET, (uint8_t[]){0x20}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_FRCTR2, (uint8_t[]){0x0F}, 1);
    // esp_lcd_panel_io_tx_param(io, ST7789_PWCTRL1, (uint8_t[]){0xa4, 0xa1}, 2);
    // esp_lcd_panel_io_tx_param(io, ST7789_PVGAMCTRL, (uint8_t[]){0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23}, 14);
    // esp_lcd_panel_io_tx_param(io, ST7789_NVGAMCTRL, (uint8_t[]){0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23}, 14);
    // esp_lcd_panel_io_tx_param(io, ST7789_INVON, NULL, 0);
    // esp_lcd_panel_io_tx_param(io, ST7789_SLPOUT, NULL, 0);
    // esp_lcd_panel_io_tx_param(io, ST7789_DISPON, NULL, 0);
    // vTaskDelay(pdMS_TO_TICKS(120));

    //--------------------------------------------------tft-espi--------------------------------------------------------------------------------
    // /**
    esp_lcd_panel_io_tx_param(io, ST7789_SLPOUT, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));
    esp_lcd_panel_io_tx_param(io, ST7789_NORON, NULL, 0);
    esp_lcd_panel_io_tx_param(io, ST7789_MADCTL, (uint8_t[]){0x00}, 1);
    esp_lcd_panel_io_tx_param(io, 0xb6, (uint8_t[]){0x0a, 0xb2}, 2);
    esp_lcd_panel_io_tx_param(io, ST7789_RAMCTRL, (uint8_t[]){0x0, 0xE0}, 2);
    esp_lcd_panel_io_tx_param(io, ST7789_COLMOD, (uint8_t[]){0x55}, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    esp_lcd_panel_io_tx_param(io, ST7789_PORCTRL, (uint8_t[]){0x0C, 0x0C, 0x00, 0x33, 0x33}, 5);
    esp_lcd_panel_io_tx_param(io, ST7789_GCTRL, (uint8_t[]){0x35}, 1);
    esp_lcd_panel_io_tx_param(io, ST7789_VCOMS, (uint8_t[]){0x28}, 1);
    esp_lcd_panel_io_tx_param(io, ST7789_LCMCTRL, (uint8_t[]){0x0C}, 1);
    esp_lcd_panel_io_tx_param(io, ST7789_VDVVRHEN, (uint8_t[]){0x01, 0xFF}, 2);
    esp_lcd_panel_io_tx_param(io, ST7789_VRHS, (uint8_t[]){0x10}, 1);
    esp_lcd_panel_io_tx_param(io, ST7789_VDVSET, (uint8_t[]){0x20}, 1);
    esp_lcd_panel_io_tx_param(io, ST7789_FRCTR2, (uint8_t[]){0x0F}, 1);
    esp_lcd_panel_io_tx_param(io, ST7789_PWCTRL1, (uint8_t[]){0xa4, 0xa1}, 2);
    esp_lcd_panel_io_tx_param(io, ST7789_PVGAMCTRL, (uint8_t[]){0xd0, 0x00, 0x02, 0x07, 0x0a, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0e, 0x12, 0x14, 0x17}, 14);
    esp_lcd_panel_io_tx_param(io, ST7789_NVGAMCTRL, (uint8_t[]){0xd0, 0x00, 0x02, 0x07, 0x0a, 0x28, 0x31, 0x54, 0x47, 0x0e, 0x1c, 0x17, 0x1b, 0x1e}, 14);
    esp_lcd_panel_io_tx_param(io, ST7789_INVON, NULL, 0);
    esp_lcd_panel_io_tx_param(io, ST7789_CASET, (uint8_t[]){0x00, 0x00, 0x00, 0xEF}, 4);
    esp_lcd_panel_io_tx_param(io, ST7789_RASET, (uint8_t[]){0x00, 0x00, 0x01, 0x3F}, 4);
    esp_lcd_panel_io_tx_param(io, ST7789_DISPON, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));
    // */

    return ESP_OK;
}

static esp_err_t panel_st7789_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");
    esp_lcd_panel_io_handle_t io = st7789->io;

    x_start += st7789->x_gap;
    x_end += st7789->x_gap;
    y_start += st7789->y_gap;
    y_end += st7789->y_gap;

    // define an area of frame memory where MCU can access
    // Column Address Set 设置行地址：开始地址-结束地址（有效地址 0～宽度-1）
    esp_lcd_panel_io_tx_param(io, ST7789_CASET, (uint8_t[]){
                                                    (x_start >> 8) & 0xFF,
                                                    x_start & 0xFF,
                                                    ((x_end - 1) >> 8) & 0xFF,
                                                    (x_end - 1) & 0xFF,
                                                },
                              4);
    // Row Address Set 设置列地址：开始地址-结束地址（有效地址 0～高度-1）
    esp_lcd_panel_io_tx_param(io, ST7789_RASET, (uint8_t[]){
                                                    (y_start >> 8) & 0xFF,
                                                    y_start & 0xFF,
                                                    ((y_end - 1) >> 8) & 0xFF,
                                                    (y_end - 1) & 0xFF,
                                                },
                              4);
    // transfer frame buffer
    size_t len = (x_end - x_start) * (y_end - y_start) * st7789->fb_bits_per_pixel / 8;
    // Memory Write 写内存，有效范围 (0~宽*高*颜色位宽)
    esp_lcd_panel_io_tx_color(io, ST7789_RAMWR, color_data, len);

    return ESP_OK;
}

static esp_err_t panel_st7789_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7789->io;
    int command = 0;
    if (invert_color_data)
    {
        command = ST7789_INVON;
    }
    else
    {
        command = ST7789_INVOFF;
    }
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}

static esp_err_t panel_st7789_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7789->io;
    if (mirror_x)
    {
        st7789->madctl_val |= LCD_CMD_MX_BIT;
    }
    else
    {
        st7789->madctl_val &= ~LCD_CMD_MX_BIT;
    }
    if (mirror_y)
    {
        st7789->madctl_val |= LCD_CMD_MY_BIT;
    }
    else
    {
        st7789->madctl_val &= ~LCD_CMD_MY_BIT;
    }
    esp_lcd_panel_io_tx_param(io, ST7789_MADCTL, (uint8_t[]){st7789->madctl_val}, 1);
    return ESP_OK;
}

static esp_err_t panel_st7789_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7789->io;
    if (swap_axes)
    {
        st7789->madctl_val |= LCD_CMD_MV_BIT;
    }
    else
    {
        st7789->madctl_val &= ~LCD_CMD_MV_BIT;
    }
    esp_lcd_panel_io_tx_param(io, ST7789_MADCTL, (uint8_t[]){st7789->madctl_val}, 1);
    return ESP_OK;
}

static esp_err_t panel_st7789_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);
    st7789->x_gap = x_gap;
    st7789->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_st7789_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    st7789_panel_t *st7789 = __containerof(panel, st7789_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7789->io;
    int command = 0;
    if (on_off)
    {
        command = ST7789_DISPON;
    }
    else
    {
        command = ST7789_DISPOFF;
    }
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}
