#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "event/group_wait.h"

void app_main(void)
{
    printf("app_main \n");
    task_group_wait_case();
}
