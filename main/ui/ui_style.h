// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   ui_style.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"

class UIStyle
{
public:
    static constexpr uint32_t BorderPanel = 0x8899FF;
    static constexpr uint32_t BackgroundContent = 0x99BBFF;
    static constexpr uint32_t BackgroundScreen = 0x99AAFF;
    static constexpr uint32_t PressedColor = 0xF0F0EE;
    static constexpr uint32_t BorderInput = 0x889FF;
    static constexpr uint32_t White = 0xFFFFFF;

    static constexpr uint32_t BorderInputW = 2;
    static constexpr uint32_t BorderPanelW = 4;
};
