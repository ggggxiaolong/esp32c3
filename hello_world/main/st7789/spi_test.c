#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_st7789.h"


#define LCD_HOST SPI2_HOST
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ SPI_MASTER_FREQ_20M
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL 1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_SCLK 2
#define EXAMPLE_PIN_NUM_MOSI 3
#define EXAMPLE_PIN_NUM_MISO -1
#define EXAMPLE_PIN_NUM_LCD_DC 6
#define EXAMPLE_PIN_NUM_LCD_RST 10
#define EXAMPLE_PIN_NUM_LCD_CS 7
#define EXAMPLE_PIN_NUM_BK_LIGHT 11
#define EXAMPLE_LCD_H_RES 240
#define EXAMPLE_LCD_V_RES 240
#define EXAMPLE_LCD_BUF_SIZE 240 * 240 * 2
#define EXAMPLE_LCD_CMD_BITS 8
#define EXAMPLE_LCD_PARAM_BITS 8