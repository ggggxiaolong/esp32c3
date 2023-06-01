/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/widgets/extra/meter.html#simple-meter

#include "lvgl.h"

void lv_example_get_start_1(void)
{
    lv_obj_t *src = lv_scr_act();
    lv_obj_set_style_bg_color(src, lv_color_hex(0x003a57), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(src);
    lv_label_set_text(label, "Hello, This is MrTan");
    lv_obj_set_style_text_color(src, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void lv_example_get_start_2(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn, 10, 10);
    lv_obj_set_size(btn, 70, 20);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
}