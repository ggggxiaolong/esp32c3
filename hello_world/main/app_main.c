#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "st7735/esp_lcd_panel_st7735.h"

#define LCD_HOST SPI2_HOST
// static const char *TAG = "example";
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ SPI_MASTER_FREQ_26M
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL 1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_SCLK 2
#define EXAMPLE_PIN_NUM_MOSI 3
#define EXAMPLE_PIN_NUM_MISO -1
#define EXAMPLE_PIN_NUM_LCD_DC 6
#define EXAMPLE_PIN_NUM_LCD_RST 10
#define EXAMPLE_PIN_NUM_LCD_CS 7
#define EXAMPLE_PIN_NUM_BK_LIGHT 11
#define EXAMPLE_LCD_CMD_BITS 8
#define EXAMPLE_LCD_PARAM_BITS 8

#define EXAMPLE_LCD_H_RES 160
#define EXAMPLE_LCD_V_RES 80
#define EXAMPLE_LCD_BUF_SIZE 160 * 80 * 2

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    // lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    // lv_disp_flush_ready(disp_driver);
    return false;
}

// static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
// {
//     esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
//     int offsetx1 = area->x1;
//     int offsetx2 = area->x2;
//     int offsety1 = area->y1;
//     int offsety2 = area->y2;
//     // copy a buffer's content to a specific area of the display
//     esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
// }

void app_main(void)
{

    // static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    // static lv_disp_drv_t disp_drv;      // contains callback functions

    printf("app_main \n");
    // ESP_LOGI(TAG, "Turn off LCD backlight");
    // gpio_config_t bk_gpio_config = {
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pin_bit_mask = 1ULL << CONFIG_LCD_BL_PIN
    // };
    // ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,
        .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,
        .miso_io_num = EXAMPLE_PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = example_notify_lvgl_flush_ready,
        // .user_ctx = &disp_drv,
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = 16,
    };

    ESP_LOGI(TAG, "Install st7735 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7735(io_handle, &panel_config, &panel_handle));
    ESP_LOGI(TAG, "esp_lcd_panel_reset");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_LOGI(TAG, "esp_lcd_panel_init");
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    // ESP_LOGI(TAG, "esp_lcd_panel_mirror");
    // ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    esp_lcd_panel_set_gap(panel_handle, 26, 1);

    // ESP_LOGI(TAG, "buf size: %d", EXAMPLE_LCD_BUF_SIZE);
    uint16_t *buff;
    buff = (uint16_t *)malloc(EXAMPLE_LCD_BUF_SIZE);
    for (size_t i = 0; i < EXAMPLE_LCD_BUF_SIZE / 2; i++)
    {
        buff[i] = ST7735_WHITE;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_V_RES , EXAMPLE_LCD_H_RES, buff);
    vTaskDelay(pdMS_TO_TICKS(1000));
    for (size_t i = 0; i < EXAMPLE_LCD_BUF_SIZE / 2; i++)
    {
        buff[i] = ST7735_BLUE;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_V_RES , EXAMPLE_LCD_H_RES, buff);
    vTaskDelay(pdMS_TO_TICKS(2000));
    for (size_t i = 0; i < EXAMPLE_LCD_BUF_SIZE / 2; i++)
    {
        buff[i] = ST7735_RED;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_V_RES , EXAMPLE_LCD_H_RES, buff);
    vTaskDelay(pdMS_TO_TICKS(2000));
    for (size_t i = 0; i < EXAMPLE_LCD_BUF_SIZE / 2; i++)
    {
        buff[i] = ST7735_GREEN;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_V_RES , EXAMPLE_LCD_H_RES, buff);
    // free(buff);
}
