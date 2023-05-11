#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "task/task_notify_value.h"

void app_main(void)
{
    printf("app_main \n");
    test_notify_value_case_1();
}
