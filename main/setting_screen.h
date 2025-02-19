// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   setting_screen.h
/// @author Petr Vanek
///

#pragma once


#include "ui/screen_manager.h"
#include "ui/scrool_input_area.h"
#include "ui/wifi_selector.h"
#include "ui/button_panel.h"
#include "ui/label_panel.h"

class SettingScreen : public IScreen
{
private:
    static constexpr const char *TAG = "SScr";
    lv_obj_t *_screen{nullptr};
    size_t _currentTextIndex{0}; 
    std::vector<std::string> _texts; 
    std::unique_ptr<ScrollableInputArea> _inputArea;
    std::unique_ptr<WifiSsidSelector> _wifiSelector;
    std::unique_ptr<ButtonPanel> _buttonPanel;
    std::shared_ptr<std::vector<ButtonField>> _buttonFields;
    std::shared_ptr<LabelPanel> _labelPanel;
    bool _backgroudTog {true};
   

public:
    
    ScreenType getType() const override;

    SettingScreen();

    bool init() override;

    void down() override;

    void show() override;

    void updateText(std::string_view data);

    void changeBackgroud();
};
