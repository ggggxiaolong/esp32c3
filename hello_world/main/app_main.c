#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "stream/buffer_size.h"

void app_main(void)
{
    printf("app_main \n");
    test_send_case_2();
}
