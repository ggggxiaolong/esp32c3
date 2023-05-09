#include "task_suspend.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_list_print()
{
    test_task_suspend_case_3();

    static char pcWriteBuffer[512] = {0};
    while (1)
    {
        vTaskList(pcWriteBuffer);
        printf("--------------------------------------------------------------\n");
        printf("Name            State   Priority    Stack    Num\n");
        printf("%s\n", pcWriteBuffer);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}