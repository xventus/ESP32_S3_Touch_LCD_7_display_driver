
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
};