// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   main_screen.cpp
/// @author Petr Vanek
///

#include "main_screen.h"
#include "esp_log.h"
#include "screen_manager.h"

MainScreen::MainScreen() {}

ScreenType MainScreen::getType() const
{
    return ScreenType::Main;
}

bool MainScreen::init()
{
    bool rc = false;

    do
    {
        if (_screen && lv_obj_is_valid(_screen))
        {
            ESP_LOGE(TAG, "Screen already initialized");
            break;
        }

        _screen = lv_obj_create(nullptr);
        if (!_screen)
            break;

        lv_obj_set_style_bg_color(_screen, lv_color_hex(0x99AAFF), 0);
        lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);

        lv_obj_t *btn = lv_btn_create(_screen);
        if (!btn)
            break;
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_size(btn, 180, 50);

        lv_obj_t *label = lv_label_create(btn);
        if (!label)
            break;
        lv_label_set_text(label, "Go to Settings");
        lv_obj_center(label);
        // Callback
        lv_obj_add_event_cb(btn, [](lv_event_t *e)
                            { ScreenManager::getInstance()->showScreenByType(ScreenType::Setting); }, LV_EVENT_CLICKED, this);

        btn = lv_btn_create(_screen);
        if (!btn)
            break;
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 100);
        lv_obj_set_size(btn, 180, 50);

        label = lv_label_create(btn);
        if (!label)
            break;
        lv_label_set_text(label, "LED");
        lv_obj_center(label);
        // Callback
        lv_obj_add_event_cb(btn, [](lv_event_t *e)
                            { auto *self = static_cast<MainScreen *>(lv_event_get_user_data(e));
        if (self)
        {
            self->_ledStat = !self->_ledStat;
            self->updateLEDState(self->_ledStat);
        } }, LV_EVENT_CLICKED, this);

        _led = lv_led_create(_screen);
        if (!_led)
            break;

        lv_obj_align(_led, LV_ALIGN_BOTTOM_MID, 0, -50);
        updateLEDState(_ledStat);

        rc = true;
    } while (false);
    return rc;
}

void MainScreen::show()
{
    if (_screen)
    {
        lv_scr_load(_screen);
    }
}

void MainScreen::updateLEDState(bool isOn)
{
    if (_led)
    {
        DDLockGuard lock();
        if (isOn)
        {
            lv_led_on(_led);
            ESP_LOGW(TAG, "LED ON");
        }
        else
        {
            lv_led_off(_led);
            ESP_LOGW(TAG, "LED OFF");
        }
    }
}
