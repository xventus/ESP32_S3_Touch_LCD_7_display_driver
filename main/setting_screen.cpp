// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   setting_screen.cpp
/// @author Petr Vanek
///

#include "setting_screen.h"
#include "esp_log.h"
#include "screen_manager.h"

SettingScreen::SettingScreen()
{
    // some stupid texts...
    _texts = {"Hello, World!", "ESP32 Rocks!", "LVGL is Great!", "Dynamic Text!"};
}

ScreenType SettingScreen::getType() const
{
    return ScreenType::Setting;
}

bool SettingScreen::init()
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

        lv_obj_set_style_bg_color(_screen, lv_color_hex(0x99FFAA), 0);
        lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);

        // switch to other screen
        lv_obj_t *btn = lv_btn_create(_screen);
        if (!btn)
            break;
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_size(btn, 180, 50);
        lv_obj_t *label = lv_label_create(btn);
        if (!label)
            break;
        lv_label_set_text(label, "Go to Home");
        lv_obj_center(label);
        // Callback
        lv_obj_add_event_cb(btn,  [](lv_event_t *e) {  
             ScreenManager::getInstance()->showScreenByType(ScreenType::Main);
        }, LV_EVENT_CLICKED, this);

        // change text
        btn = lv_btn_create(_screen);
        if (!btn)
            break;
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 100);
        lv_obj_set_size(btn, 180, 50);
        label = lv_label_create(btn);
        if (!label)
            break;
        lv_label_set_text(label, "Change text");
        lv_obj_center(label);
        // Callback
        lv_obj_add_event_cb(btn,  [](lv_event_t *e) {  auto *self = static_cast<SettingScreen *>(lv_event_get_user_data(e));
        if (self) {
            self->_currentTextIndex = (self->_currentTextIndex + 1) % self->_texts.size();
            const char *nextText = self->_texts[self->_currentTextIndex].c_str();
            self->updateText(nextText);
      
        }}, LV_EVENT_CLICKED, this);


        _label = lv_label_create(_screen);
        if (!label)
            break;

        lv_label_set_text(_label, "Hello, World!");
        lv_obj_align(_label, LV_ALIGN_BOTTOM_MID, 0, -50);

        rc = true;
    } while (false);
    return rc;
}

 void SettingScreen::down()
{
    if (_screen && lv_obj_is_valid(_screen))
    {
        lv_obj_del(_screen);
        _screen = nullptr; 
    }

    _label = nullptr;

    ESP_LOGI(TAG, "SettingScreen resources released.");
}



void SettingScreen::show()
{
    if (_screen)
    {
        lv_scr_load(_screen);
    }
}

void SettingScreen::updateText(std::string_view data)
{

    if (_label && !data.empty())
    {
        DDLockGuard lock;
        lv_label_set_text(_label, data.data());
    }
    else
    {
        ESP_LOGE(TAG, "Label is not initialized");
    }
}

