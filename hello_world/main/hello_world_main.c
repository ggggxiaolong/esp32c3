/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task/task_suspend.h"

void app_main(void)
{
    printf("app_main 2 \n");
    test_task_suspend_case_3();
}
