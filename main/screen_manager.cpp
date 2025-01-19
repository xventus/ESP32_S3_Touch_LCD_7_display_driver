

// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   screen_manager.cpp
/// @author Petr Vanek
///

#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include "screen_manager.h"
#include "setting_screen.h"
#include "main_screen.h"
#include "esp_log.h"

// Singleton instance getter
ScreenManager *ScreenManager::getInstance()
{
    static ScreenManager instance; // Thread-safe static initialization
    return &instance;
}

// Overloaded -> operator
ScreenManager *ScreenManager::operator->()
{
    return ScreenManager::getInstance();
}

ScreenManager *ScreenManager::operator->() const
{
    return ScreenManager::getInstance();
}

void ScreenManager::initLCD()
{
    _dd.initBus();
    _dd.start();
}

std::optional<size_t> ScreenManager::addScreen(std::unique_ptr<IScreen> screen)
{
    if (!screen)
    {
        ESP_LOGE(TAG, "Invalid screen pointer");
        return std::nullopt;
    }

    if (!screen->init())
    {
        ESP_LOGE(TAG, "Failed to initialize screen");
        return std::nullopt;
    }

    const auto tag = screen->getType();
    if (_screenIndexByName.find(tag) != _screenIndexByName.end())
    {
        ESP_LOGE(TAG, "Screen already exists");
        return std::nullopt;
    }

    size_t index = _screens.size();
    _screenIndexByName[tag] = index;
    _screens.push_back(std::move(screen));
    return index;
}

void ScreenManager::lock()
{
    _dd.lock();
}

void ScreenManager::unlock()
{
    _dd.unlock();
}

bool ScreenManager::showScreen(std::size_t index)
{
    if (index >= _screens.size() || !_screens[index])
    {
        ESP_LOGE(TAG, "Invalid screen index: %zu", index);
        return false;
    }

    {
        DDLockGuard lock();
        _screens[index]->show();
    }

    _currentScreen = _screens[index].get();
    return true;
}

bool ScreenManager::showScreenByType(ScreenType tag)
{
    auto it = _screenIndexByName.find(tag);
    if (it == _screenIndexByName.end())
    {
        ESP_LOGE(TAG, "No screen found ");
        return false;
    }
    return showScreen(it->second);
}

void ScreenManager::updateText(std::string_view data)
{

    auto it = _screenIndexByName.find(ScreenType::Setting);
    if (it == _screenIndexByName.end())
    {
        return;
    }

    auto &screen = _screens[it->second];
    if (screen->getType() == ScreenType::Setting)
    {
        // RTTI disabled dynamic_cast not allowed
        static_cast<SettingScreen *>(screen.get())->updateText(data);
    }
}

void ScreenManager::updateLEDState(bool isOn)
{
    auto it = _screenIndexByName.find(ScreenType::Main);
    if (it == _screenIndexByName.end())
    {
        return;
    }

    auto &screen = _screens[it->second];
    if (screen->getType() == ScreenType::Setting)
    {
        // RTTI disabled dynamic_cast not allowed
        static_cast<MainScreen *>(screen.get())->updateLEDState(isOn);
    }
}
