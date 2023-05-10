#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
// one time
// repeat
// stop
// name id
// reset
// period

void soft_timer_callback_1(TimerHandle_t timer)
{
    const char *name = pcTimerGetName(timer);
    // 将指针转换成 uint
    uint32_t id;
    id = (uint32_t)pvTimerGetTimerID(timer);
    printf("%s-%ld \n", name, id);
}

void task_soft_timer_case_1()
{
    TimerHandle_t timer_1, timer_2;
    timer_1 = xTimerCreate("timer 1", pdMS_TO_TICKS(1000), pdFALSE, (void *)1, soft_timer_callback_1);
    timer_2 = xTimerCreate("timer 2", pdMS_TO_TICKS(1000), pdTRUE, (void *)2, soft_timer_callback_1);
    if (timer_1 != NULL)
    {
        printf("timer created \n");
        xTimerStart(timer_1, 0);
        xTimerStart(timer_2, 0);
    }
    else
    {
        printf("timer fail \n");
    }
}

void task_soft_timer_case_2()
{
    TimerHandle_t timer_1, timer_2;
    timer_1 = xTimerCreate("timer 1", pdMS_TO_TICKS(1000), pdFALSE, (void *)1, soft_timer_callback_1);
    timer_2 = xTimerCreate("timer 2", pdMS_TO_TICKS(1000), pdTRUE, (void *)2, soft_timer_callback_1);
    if (timer_1 != NULL)
    {
        printf("timer created \n");
        xTimerStart(timer_1, 0);
        xTimerStart(timer_2, 0);
        vTaskDelay(6000 / portTICK_PERIOD_MS);
        printf("stop timer2\n");
        xTimerStop(timer_2, 0);
    }
    else
    {
        printf("timer fail \n");
    }
}

void task_soft_timer_case_3()
{
    TimerHandle_t timer_1, timer_2;
    timer_1 = xTimerCreate("timer 1", pdMS_TO_TICKS(1000), pdFALSE, (void *)1, soft_timer_callback_1);
    timer_2 = xTimerCreate("timer 2", pdMS_TO_TICKS(2000), pdTRUE, (void *)2, soft_timer_callback_1);
    if (timer_1 != NULL)
    {
        printf("timer created \n");
        xTimerStart(timer_1, 0);
        xTimerStart(timer_2, 0);
        vTaskDelay(6000 / portTICK_PERIOD_MS);
        printf("reset timer2\n");
        for (size_t i = 0; i < 3; i++)
        {
            xTimerReset(timer_2, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        printf("stop reset timer2\n");
    }
    else
    {
        printf("timer fail \n");
    }
}

void task_soft_timer_case_4()
{
    TimerHandle_t timer_1, timer_2;
    timer_1 = xTimerCreate("timer 1", pdMS_TO_TICKS(1000), pdTRUE, (void *)1, soft_timer_callback_1);
    timer_2 = xTimerCreate("timer 2", pdMS_TO_TICKS(1000), pdTRUE, (void *)2, soft_timer_callback_1);
    if (timer_1 != NULL)
    {
        printf("timer created \n");
        xTimerStart(timer_1, 0);
        xTimerStart(timer_2, 0);
        vTaskDelay(6000 / portTICK_PERIOD_MS);
        printf("period timer2\n");
        xTimerChangePeriod(timer_2, pdMS_TO_TICKS(2000), 0);
    }
    else
    {
        printf("timer fail \n");
    }
}