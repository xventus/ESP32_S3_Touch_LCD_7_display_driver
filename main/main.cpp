//
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   main.cpp
/// @author Petr Vanek

#include <stdio.h>
#include "screen_manager.h"

#include "main_screen.h"
#include "setting_screen.h"

extern "C"
{
    void app_main(void);
}

void app_main(void)
{

    auto *screenManager = ScreenManager::getInstance();
    screenManager->initLCD();
    screenManager->addScreen(std::make_unique<MainScreen>());
    screenManager->addScreen(std::make_unique<SettingScreen>());
    screenManager->showScreenByType(ScreenType::Main);
    screenManager->updateText("Text from main");
}
