#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_st7735.h"

#define LCD_HOST SPI2_HOST
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ SPI_MASTER_FREQ_10M
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

static esp_lcd_panel_handle_t panel_handle = NULL;
static const char *TAG = "st7735.test";

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    return false;
}

void init_lcd()
{
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
    esp_lcd_panel_set_gap(panel_handle, 1, 26);
}

void test_lcd()
{
    if (panel_handle == NULL)
        return;
    uint16_t *buff;
    ESP_LOGI(TAG, "ST7735_WHITE");
    buff = (uint16_t *)malloc(EXAMPLE_LCD_BUF_SIZE);
    for (size_t i = 0; i < EXAMPLE_LCD_BUF_SIZE / 2; i++)
    {
        buff[i] = ST7735_WHITE;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, buff);
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "ST7735_RED");
    for (size_t i = 0; i < EXAMPLE_LCD_BUF_SIZE / 2; i++)
    {
        buff[i] = ST7735_RED;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, buff);
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "ST7735_GREEN");
    for (size_t i = 0; i < EXAMPLE_LCD_BUF_SIZE / 2; i++)
    {
        buff[i] = ST7735_GREEN;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, buff);
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "ST7735_BLUE");
    for (size_t i = 0; i < EXAMPLE_LCD_BUF_SIZE / 2; i++)
    {
        buff[i] = ST7735_BLUE;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, buff);
}