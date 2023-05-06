#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task_param.h"

/// @brief 传递整数
/// @param pvParam 参数指针
void task_param(void *pvParam)
{
    int *num;
    num = (int *)pvParam;

    printf("task_param param is %d\n", *num);

    vDelete(NULL);
}

/// @brief 传递整数数组
/// @param pvParam 参数指针
void task_param_arr(void *pvParam)
{
    int *num;
    num = (int *)pvParam;

    printf("task_param_arr param array 0 is %d\n", *num);
    printf("task_param_arr param array 1 is %d\n", *(num + 1));
    printf("task_param_arr param array 2 is %d\n", *(num + 2));

    vDelete(NULL);
}


/// @brief 传递结构体
/// @param pvParam 参数指针
void task_param_struct(void *pvParam)
{
    struct TransParam *p_param;
    p_param = (TransParam *)pvParam;

    printf("task_param_struct param1 is %d\n", p_param->param1);
    printf("task_param_struct param2 is %d\n", p_param->param2);

    vDelete(NULL);
}


/// @brief 传递结构体
/// @param pvParam 参数指针
void task_param_string(void *pvParam)
{
    char *p_msg;
    p_msg = (char *)pvParam;

    printf("task_param_string param is %s\n", p_msg);

    vDelete(NULL);
}