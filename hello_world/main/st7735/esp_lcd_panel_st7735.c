/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// #include <stdlib.h>
// #include <sys/cdefs.h>
// #include "sdkconfig.h"
// #if CONFIG_LCD_ENABLE_DEBUG_LOG
// // The local log level must be defined before including esp_log.h
// // Set the maximum log level for this source file
// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
// #endif
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_lcd_panel_interface.h"
// #include "esp_lcd_panel_io.h"
// #include "esp_lcd_panel_vendor.h"
// #include "esp_lcd_panel_ops.h"
// #include "esp_lcd_panel_commands.h"
// #include "driver/gpio.h"
// #include "esp_log.h"
// #include "esp_check.h"
#include "esp_lcd_panel_st7735.h"

// static const char *TAG = "lcd_panel.st7735";

static esp_err_t panel_st7735_del(esp_lcd_panel_t *panel);
static esp_err_t panel_st7735_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_st7735_init(esp_lcd_panel_t *panel);
static esp_err_t panel_st7735_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
static esp_err_t panel_st7735_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t panel_st7735_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t panel_st7735_swap_xy(esp_lcd_panel_t *panel, bool swap_axes);
static esp_err_t panel_st7735_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_st7735_disp_on_off(esp_lcd_panel_t *panel, bool off);

// typedef struct
// {
//     esp_lcd_panel_t base;
//     esp_lcd_panel_io_handle_t io;
//     int reset_gpio_num;
//     // bool reset_level;
//     int x_gap;
//     int y_gap;
//     uint8_t fb_bits_per_pixel;
//     uint8_t madctl_val; // save current value of LCD_CMD_MADCTL register
//     // uint8_t colmod_cal; // save surrent value of LCD_CMD_COLMOD register
// } st7735_panel_t;

esp_err_t esp_lcd_new_panel_st7735(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    st7735_panel_t *st7735 = NULL;
    ESP_GOTO_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    st7735 = calloc(1, sizeof(st7735_panel_t));
    ESP_GOTO_ON_FALSE(st7735, ESP_ERR_NO_MEM, err, TAG, "no mem for st7735 panel");

    if (panel_dev_config->reset_gpio_num >= 0)
    {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << panel_dev_config->reset_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configure GPIO for RST line failed");
    }
    st7735->madctl_val = 0xC8;
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

    st7735->io = io;
    st7735->fb_bits_per_pixel = fb_bits_per_pixel;
    st7735->reset_gpio_num = panel_dev_config->reset_gpio_num;
    // st7735->reset_level = panel_dev_config->flags.reset_active_high;
    st7735->base.reset = panel_st7735_reset;
    st7735->base.init = panel_st7735_init;
    st7735->base.draw_bitmap = panel_st7735_draw_bitmap;
    st7735->base.invert_color = panel_st7735_invert_color;
    st7735->base.set_gap = panel_st7735_set_gap;
    st7735->base.mirror = panel_st7735_mirror;
    st7735->base.swap_xy = panel_st7735_swap_xy;
    st7735->base.disp_on_off = panel_st7735_disp_on_off;
    *ret_panel = &(st7735->base);
    ESP_LOGD(TAG, "new st7735 panel @%p", st7735);

    return ESP_OK;

err:
    if (st7735)
    {
        if (panel_dev_config->reset_gpio_num >= 0)
        {
            gpio_reset_pin(panel_dev_config->reset_gpio_num);
        }
        free(st7735);
    }
    return ret;
}

static esp_err_t panel_st7735_del(esp_lcd_panel_t *panel)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);

    if (st7735->reset_gpio_num >= 0)
    {
        gpio_reset_pin(st7735->reset_gpio_num);
    }
    ESP_LOGD(TAG, "del st7735 panel @%p", st7735);
    free(st7735);
    return ESP_OK;
}

static esp_err_t panel_st7735_reset(esp_lcd_panel_t *panel)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7735->io;

    // perform hardware reset
    if (st7735->reset_gpio_num >= 0)
    {
        gpio_set_level(st7735->reset_gpio_num, 0);
        vTaskDelay(pdMS_TO_TICKS(120));
        gpio_set_level(st7735->reset_gpio_num, 1);
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    else
    { // perform software reset
        esp_lcd_panel_io_tx_param(io, LCD_CMD_SWRESET, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(20)); // spec, wait at least 5m before sending new command
    }

    return ESP_OK;
}

static esp_err_t panel_st7735_init(esp_lcd_panel_t *panel)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7735->io;
    //  1: software reset, no args, w/delay
    esp_lcd_panel_io_tx_param(io, ST7735_SWRESET, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));
    // Frame rate=fosc/((RTNB + 20) x (LINE + FPB + BPB))   fosc = 333kHz
    //  2: out of sleep mode, no args, w/delay 退出睡眠模式
    esp_lcd_panel_io_tx_param(io, ST7735_SLPOUT, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));
    //  3: frame rate control - normal mode, 3 args: 设置普通模式帧率
    esp_lcd_panel_io_tx_param(io, ST7735_FRMCTR1, (uint8_t[]){0x01, 0x2c, 0x2d}, 3);
    //  4: frame rate control - idle mode, 3 args:
    esp_lcd_panel_io_tx_param(io, ST7735_FRMCTR2, (uint8_t[]){0x01, 0x2c, 0x2d}, 3);
    //  5: frame rate control - partial mode, 6 args:
    esp_lcd_panel_io_tx_param(io, ST7735_FRMCTR3, (uint8_t[]){0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d}, 6);
    //  6: display inversion control, 1 arg, no delay: 显示反转控制
    esp_lcd_panel_io_tx_param(io, ST7735_INVCTR, (uint8_t[]){0x07}, 1);
    //  7: power control, 3 args, no delay:功率控制
    esp_lcd_panel_io_tx_param(io, ST7735_PWCTR1, (uint8_t[]){0xa2, 0x02, 0x84}, 3);
    //  8: power control, 1 arg, no delay: Set the VGH and VGL supply power level
    esp_lcd_panel_io_tx_param(io, ST7735_PWCTR2, (uint8_t[]){0xC5}, 1);
    //  9: power control, 2 args, no delay:
    esp_lcd_panel_io_tx_param(io, ST7735_PWCTR3, (uint8_t[]){0x0a, 0x00}, 2);
    // 10: power control, 2 args, no delay:
    esp_lcd_panel_io_tx_param(io, ST7735_PWCTR4, (uint8_t[]){0x8a, 0x2A}, 2);
    // 11: power control, 2 args, no delay:
    esp_lcd_panel_io_tx_param(io, ST7735_PWCTR5, (uint8_t[]){0x8a, 0xEE}, 2);
    // 12: power control, 1 arg, no delay: Set VCOMH Voltage
    esp_lcd_panel_io_tx_param(io, ST7735_VMCTR1, (uint8_t[]){0x0e}, 1);
    // 13: invert display, no args, no delay 打开返显模式， 颜色反转
    esp_lcd_panel_io_tx_param(io, ST7735_INVON, NULL, 0);
    // 14: memory access control (directions), 1 arg: 地址顺序：右到左,底到顶，行列交换：否，刷新顺序:左到右，顶到底，颜色：BGR
    esp_lcd_panel_io_tx_param(io, ST7735_MADCTL, (uint8_t[]){0xc8}, 1);
    // 15: set color mode, 1 arg, no delay:  颜色位宽：101->16bit
    esp_lcd_panel_io_tx_param(io, ST7735_COLMOD, (uint8_t[]){0x05}, 1);
    // 16: magical unicorn dust, 16 args, no delay:
    esp_lcd_panel_io_tx_param(io, ST7735_GAMCTRP1, (uint8_t[]){0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10}, 16);
    // 17: sparkles and rainbows, 16 args, no delay:
    esp_lcd_panel_io_tx_param(io, ST7735_GAMCTRN1, (uint8_t[]){0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10}, 16);
    // 18: normal display on, no args, no delay 普通显示模式，关闭局部显示模式
    esp_lcd_panel_io_tx_param(io, ST7735_NORON, NULL, 0);
    // 19: set column address, 4 args, no delay:
    esp_lcd_panel_io_tx_param(io, ST7735_CASET, (uint8_t[]){0x00, 0x02, 0x00, 0x7f + 0x02}, 4);
    // 20: set row address, 4 args, no delay:
    esp_lcd_panel_io_tx_param(io, ST7735_RASET, (uint8_t[]){0x00, 0x01, 0x00, 0x9f + 0x01}, 4);
    // 21: set write ram, N args, no delay:
    // esp_lcd_panel_io_tx_param(io, ST7735_RAMWR, NULL, 0);
    // write_buff(g, (uint8_t *)g->priv, GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT * 2);
    // 22: main screen turn on, no args, no delay 打开显示
    esp_lcd_panel_io_tx_param(io, ST7735_DISPON, NULL, 0);

    return ESP_OK;
}

static esp_err_t panel_st7735_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");
    esp_lcd_panel_io_handle_t io = st7735->io;

    x_start += st7735->x_gap;
    x_end += st7735->x_gap;
    y_start += st7735->y_gap;
    y_end += st7735->y_gap;

    // define an area of frame memory where MCU can access
    // Column Address Set 设置行地址：开始地址-结束地址（有效地址 0～宽度-1）
    esp_lcd_panel_io_tx_param(io, ST7735_CASET, (uint8_t[]){
                                                    (x_start >> 8) & 0xFF,
                                                    x_start & 0xFF,
                                                    ((x_end - 1) >> 8) & 0xFF,
                                                    (x_end - 1) & 0xFF,
                                                },
                              4);
    // Row Address Set 设置列地址：开始地址-结束地址（有效地址 0～高度-1）
    esp_lcd_panel_io_tx_param(io, ST7735_RASET, (uint8_t[]){
                                                    (y_start >> 8) & 0xFF,
                                                    y_start & 0xFF,
                                                    ((y_end - 1) >> 8) & 0xFF,
                                                    (y_end - 1) & 0xFF,
                                                },
                              4);
    // transfer frame buffer
    size_t len = (x_end - x_start) * (y_end - y_start) * st7735->fb_bits_per_pixel / 8;
    // Memory Write 写内存，有效范围 (0~宽*高*颜色位宽)
    esp_lcd_panel_io_tx_color(io, ST7735_RAMWR, color_data, len);

    return ESP_OK;
}

static esp_err_t panel_st7735_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7735->io;
    int command = 0;
    if (invert_color_data)
    {
        command = ST7735_INVON;
    }
    else
    {
        command = ST7735_INVOFF;
    }
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}

static esp_err_t panel_st7735_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7735->io;
    if (mirror_x)
    {
        st7735->madctl_val |= LCD_CMD_MX_BIT;
    }
    else
    {
        st7735->madctl_val &= ~LCD_CMD_MX_BIT;
    }
    if (mirror_y)
    {
        st7735->madctl_val |= LCD_CMD_MY_BIT;
    }
    else
    {
        st7735->madctl_val &= ~LCD_CMD_MY_BIT;
    }
    esp_lcd_panel_io_tx_param(io, ST7735_MADCTL, (uint8_t[]){st7735->madctl_val}, 1);
    return ESP_OK;
}

static esp_err_t panel_st7735_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7735->io;
    if (swap_axes)
    {
        st7735->madctl_val |= LCD_CMD_MV_BIT;
    }
    else
    {
        st7735->madctl_val &= ~LCD_CMD_MV_BIT;
    }
    esp_lcd_panel_io_tx_param(io, ST7735_MADCTL, (uint8_t[]){st7735->madctl_val}, 1);
    return ESP_OK;
}

static esp_err_t panel_st7735_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    st7735->x_gap = x_gap;
    st7735->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_st7735_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7735->io;
    int command = 0;
    if (on_off)
    {
        command = ST7735_DISPON;
    }
    else
    {
        command = ST7735_DISPOFF;
    }
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}
