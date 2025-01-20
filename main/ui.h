
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   ui.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"

class UI
{

    const uint32_t green{0x00DD00};
    const uint32_t red{0x550000};
    const uint32_t ltgreen{0xA8E6CF};
    const uint32_t ltred{0xFFC4C4};
    const uint32_t yellow{0x005555};
    const uint32_t white{0xFFFFFF};
    const uint32_t black{0x000000};
    const uint32_t blue{0x000055};

    static lv_obj_t *addIcon(lv_obj_t *parent, const lv_img_dsc_t *icon, lv_align_t align, int x, int y)
    {
        lv_obj_t *img = lv_img_create(parent);
        lv_img_set_src(img, icon);
        lv_obj_align(img, align, x, y);
        return img;
    }

    static lv_obj_t *addLabel(lv_obj_t *parent, const char *text, lv_align_t align, int x, int y, const lv_font_t *font)
    {
        lv_obj_t *label = lv_label_create(parent);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_font(label, font, 0);
        lv_obj_set_style_text_color(label, lv_color_black(), 0);
        lv_obj_align(label, align, x, y);
        return label;
    }

    void Dashboard::createExitButton(lv_obj_t *parent)
    {

        lv_obj_t *exitBtn = lv_btn_create(parent);
        lv_obj_set_size(exitBtn, 100, 50);
        lv_obj_align(exitBtn, LV_ALIGN_TOP_LEFT, 20, 20);

        lv_obj_t *exitLabel = lv_label_create(exitBtn);
        lv_label_set_text(exitLabel, "EXIT");
        lv_obj_center(exitLabel);

        lv_obj_add_event_cb(exitBtn, [](lv_event_t *e)
                            {
                            auto *dashboard = static_cast<Dashboard *>(lv_event_get_user_data(e));
                            if (dashboard)
                            {
                                dashboard->showMainScreen();
                            }
                            else
                            {
                                ESP_LOGE("Dashboard", "Dashboard instance is null!");
                            } }, LV_EVENT_CLICKED, this);
    }
};