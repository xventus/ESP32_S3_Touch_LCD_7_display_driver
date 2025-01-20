// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   setting_screen.h
/// @author Petr Vanek
///

#pragma once


#include "screen_manager.h"


class SettingScreen : public IScreen
{
private:
    static constexpr const char *TAG = "SScr";
    lv_obj_t *_screen{nullptr};
    lv_obj_t *_label{nullptr};
    size_t _currentTextIndex{0}; 
    std::vector<std::string> _texts; 

public:
    
    ScreenType getType() const override;

    SettingScreen();

    bool init() override;

    void down() override;

    void show() override;

    void updateText(std::string_view data);
};
