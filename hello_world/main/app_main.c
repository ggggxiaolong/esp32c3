#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "st7789/test.h"

void app_main(void)
{

    printf("app_main \n");
    init_lcd();
    test_lcd();    
}
