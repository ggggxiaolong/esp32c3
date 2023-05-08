#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_LEFT GPIO_NUM_12
#define LED_RIGHT GPIO_NUM_13

void blink()
{
    gpio_reset_pin(LED_LEFT);
    gpio_set_direction(LED_LEFT, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_RIGHT);
    gpio_set_direction(LED_RIGHT, GPIO_MODE_OUTPUT);

    uint32_t led_state = 0;
    while (1)
    {
        led_state = !led_state;
        gpio_set_level(LED_LEFT, led_state);
        gpio_set_level(LED_RIGHT, !led_state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}