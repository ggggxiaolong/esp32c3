#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "gdb_test";

uint8_t i = 0, j = 0;
char *a;
uint8_t k[5];
int n = 0;

typedef struct 
{
    char *name;
    uint8_t age;
} People;

People customer;

void square_func(uint8_t i)
{
    n = i * i;
}

void gdb_test(void)
{
    printf("gdb_test \n");
    while (1)
    {
        i++;
        ESP_LOGI(TAG, "i = %d", i);

        j++;
        ESP_LOGI(TAG, "j = %d", j);

        i += 2;
        ESP_LOGI(TAG, "i = %d", i);

        for (uint8_t m = 0; m < 5; m++)
        {
            k[m] = m;
            ESP_LOGI(TAG, "k[%d] = %d", m, k[m]);
        }

        a = "Hello";
        ESP_LOGI(TAG, "a = %s", a);

        a = "I am MrTan";
        ESP_LOGI(TAG, "a = %s", a);

        customer.name = "MrTan";
        customer.age = 20;
        ESP_LOGI(TAG, "customer.name = %s; customer.age = %d", customer.name, customer.age);

        square_func(i);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
