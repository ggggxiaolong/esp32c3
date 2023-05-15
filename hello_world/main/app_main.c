#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "gdb/test.h"

void app_main(void)
{
    printf("app_main \n");
    gdb_test();
}
