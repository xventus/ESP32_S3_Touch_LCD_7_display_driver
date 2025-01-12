// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   display_driver.h
/// @author Petr Vanek
///



#pragma once

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
#include "hardware.h"

class DisplayDriver
{
public:
    DisplayDriver();
    ~DisplayDriver();

    /// @brief bus & IO init
    void initBus();

    /// @brief LGVL driver start & attac
    void start(const uint32_t usStackDepth = 4094, UBaseType_t uxPriority = 2);

    /// @brief lgvl ui lock
    /// @param timeout_ms  
    /// @return 
    bool lock(int timeout_ms = -1);

    /// @brief lgvl ui unlock
    void unlock();

private:
    // events callback
    static bool lvglVsynEvent(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data);
    static void lvglFlush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
    static void lvglTick(void *arg);
    static void lvglTouch(lv_indev_drv_t * drv, lv_indev_data_t * data);

    // working task
    static void lvglWorkingTask(void *arg);
    
    // helpers
    void initLCD();
    void initI2C();
    void initGPIO();
    void resetTouch();
    void initTouch();


private:
    
    static constexpr const char *TAG = "DD";

    esp_lcd_panel_handle_t _panelHandle{nullptr};
    lv_disp_draw_buf_t _displayBuff;
    lv_disp_drv_t _displayDriver;      
    esp_lcd_touch_handle_t _touchHandle{nullptr};
    esp_lcd_panel_io_handle_t _touchIOHandle{nullptr};
    SemaphoreHandle_t _lvglLock {nullptr};
    lv_indev_drv_t _devDr;

};


