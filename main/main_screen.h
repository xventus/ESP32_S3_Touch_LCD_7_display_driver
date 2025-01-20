// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   main_screen.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"
#include "screen_if.h"
#include "sd_card.h"

class ScreenManager;
class MainScreen : public IScreen
{
private:
    static constexpr const char *TAG = "MScr";
    lv_obj_t *_screen{nullptr};
    lv_obj_t *_led{nullptr};
    bool _ledStat{true};
    lv_obj_t *_overlay {nullptr};

public:
    MainScreen();

    ScreenType getType() const override;

    bool init() override;

    void down() override;

    void show() override;

    void updateLEDState(bool isOn);

    void sdCardTest();

    void message(std::string_view msg);

    //void closeDialog();

private:
};
