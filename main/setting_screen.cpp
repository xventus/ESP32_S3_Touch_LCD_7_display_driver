// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   setting_screen.cpp
/// @author Petr Vanek
///

#include "setting_screen.h"
#include "esp_log.h"
#include "ui/screen_manager.h"

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
        const lv_coord_t shift = 300;

        if (_screen && lv_obj_is_valid(_screen))
        {
            ESP_LOGE(TAG, "Screen already initialized");
            break;
        }

        _screen = lv_obj_create(nullptr);
        if (!_screen)
            break;

        lv_obj_set_style_bg_color(_screen, lv_color_hex(UIStyle::BackgroundScreen), 0);
        lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);

        _wifiSelector = std::make_unique<WifiSsidSelector>(_screen, 350, 200, 430, 10);
        _wifiSelector->withOnWifiSelectedCallback([this](const std::string &ssid)
                                                  { ESP_LOGI("MAIN", "********* Selected Wi-Fi: %s", ssid.c_str()); 
                                                  DDLockGuard lock;
                                                 _inputArea->setInputContent("wifiField",ssid); });
        _wifiSelector->startPeriodicScan();

        _buttonFields = std::make_shared<std::vector<ButtonField>>(
            std::initializer_list<ButtonField>{
                {"Go to home screen", lv_color_hex(0xFFFFFF), lv_color_hex(0x1E90FF), [this]()
                 {
                     ScreenManager::getInstance()->showScreenByType(ScreenType::Main);
                 }},
                {"Change text", lv_color_hex(0x000000), lv_color_hex(0xFF6347), [this]()
                 {
                     _currentTextIndex = (_currentTextIndex + 1) % _texts.size();
                     const char *nextText = _texts[_currentTextIndex].c_str();
                     updateText(nextText);
                 }},
                {"Background change", lv_color_hex(0xFFFFFF), lv_color_hex(0x32CD32), [this]()
                 {
                     changeBackgroud();
                 }}});

        _buttonPanel = std::make_unique<ButtonPanel>(_screen, _buttonFields, 350, 230, 430, 230);

        _labelPanel = std::make_unique<LabelPanel>(_screen, "Push button ->", 400, 60, 10, 400, 4);

        std::vector<InputField> fields = {
            {"field1", "Name:", "", 20, lv_color_hex(0xFF7777), false},
            {"wifiField", "SSID:", "", 20, lv_color_hex(UIStyle::White), false},
            {"field3", "PASSWORD:", "", 20, lv_color_hex(UIStyle::White), true},
            {"field4", "MQTT:", "mqtt://xx.xx.xx.xx", 40, lv_color_hex(UIStyle::White), false},
            {"field5", "TMZ:", "", 20, lv_color_hex(UIStyle::White), false},
            {"field6", "Email:", "test@test.com", 30, lv_color_hex(UIStyle::White), false}};

        _inputArea = std::make_unique<ScrollableInputArea>(_screen, fields, 200, 400, 380, 10, 10);

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

    _wifiSelector.reset();
    _inputArea.reset();
    _buttonPanel.reset();
    _buttonFields.reset();
    _labelPanel.reset();

    ESP_LOGI(TAG, "SettingScreen resources released.");
}

void SettingScreen::show()
{
    if (_screen)
    {
        DDLockGuard lock;
        lv_scr_load(_screen);
    }
}

void SettingScreen::updateText(std::string_view data)
{

    if (_labelPanel && !data.empty())
    {
        DDLockGuard lock;
        _labelPanel->setText(data.data());
    }
    else
    {
        ESP_LOGE(TAG, "Label is not initialized");
    }
}

void SettingScreen::changeBackgroud()
{
    DDLockGuard lock;
    if (_labelPanel)
        _labelPanel->setBackgroundColor(_backgroudTog ? lv_color_hex(UIStyle::White) : lv_color_hex(UIStyle::BackgroundContent));
    if (_buttonPanel)
        _buttonPanel->setBackgroundColor(_backgroudTog ? lv_color_hex(UIStyle::White) : lv_color_hex(UIStyle::BackgroundContent));
    if (_wifiSelector)
        _wifiSelector->setBackgroundColor(_backgroudTog ? lv_color_hex(UIStyle::White) : lv_color_hex(UIStyle::BackgroundContent));
    if (_inputArea)
        _inputArea->setBackgroundColor(_backgroudTog ? lv_color_hex(UIStyle::White) : lv_color_hex(UIStyle::BackgroundContent));

    _backgroudTog = !_backgroudTog;
}