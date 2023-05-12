#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "task/task_list.h"

void app_main(void)
{
    printf("app_main \n");
    task_list_print();
}
