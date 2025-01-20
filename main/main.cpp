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

void monitor()
{
    size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t free_internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t free_external = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    printf("Free Heap: %d bytes\n", free_heap);
    printf("Free Internal Heap: %d bytes\n", free_internal);
    printf("Free External Heap: %d bytes\n", free_external);
}

void app_main(void)
{

    auto *screenManager = ScreenManager::getInstance();
    screenManager->initLCD(true, HW_I2C_NUM);
    screenManager->addScreen(std::make_unique<MainScreen>());
    screenManager->addScreen(std::make_unique<SettingScreen>());
    screenManager->showScreenByType(ScreenType::Main);
    screenManager->updateText("Text from main");
 
}
