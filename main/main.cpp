//
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   main.cpp
/// @author Petr Vanek

#include <stdio.h>

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

bool sysInit()
{
     auto ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    return ret;
}

void app_main(void)
{
    sysInit();

    auto *screenManager = ScreenManager::getInstance();
    screenManager->initLCD(true, HW_I2C_NUM);
    screenManager->addScreen(std::make_unique<MainScreen>());
    screenManager->showScreenByType(ScreenType::Main);
    screenManager->updateText("Text from main");
 
}
