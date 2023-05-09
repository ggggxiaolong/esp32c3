#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "queue/queue_set.h"

void app_main(void)
{
    printf("app_main \n");
    test_queue_set_case_1();
}
