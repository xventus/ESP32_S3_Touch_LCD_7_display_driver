//
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   main.cpp
/// @author Petr Vanek


//#include "display_driver.h"
//

#include "hardware.h"

extern "C"
{
    void app_main(void);
}


#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "driver/i2c.h"
#include "esp_lcd_touch_gt911.h"

#include "display_driver.h"



void create_simple_button(lv_disp_t *disp) {
    // Create a screen
    lv_obj_t *screen = lv_disp_get_scr_act(disp);

    // Create a button
    lv_obj_t *btn = lv_btn_create(screen); // Add a button to the current screen
    lv_obj_set_size(btn, 100, 50);         // Set the size of the button
    lv_obj_center(btn);                   // Align the button to the center of the screen

    // Create a label on the button
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Click Me!"); // Set the label text
    lv_obj_center(label);                 // Align the label to the center of the button
}

DisplayDriver  dd;
void app_main(void)
{

    dd.initBus();
    dd.start();

    dd.lock(-1);
    create_simple_button(nullptr);
    dd.unlock();

}
