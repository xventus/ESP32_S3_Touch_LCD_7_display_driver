

#include "display_driver.h"


DisplayDriver::DisplayDriver()
{
}

DisplayDriver::~DisplayDriver()
{
}

void DisplayDriver::initBus()
{
    initLCD();
    initI2C();
    initGPIO();
    resetTouch();
    initTouch();
}



void DisplayDriver::initI2C()
{

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_8,
        .scl_io_num = GPIO_NUM_9,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = 400000,
        },
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(HW_I2C_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(HW_I2C_NUM, conf.mode, 0, 0, 0));
}

void DisplayDriver::initGPIO()
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);
}

void DisplayDriver::initTouch()
{
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)HW_I2C_NUM, &tp_io_config, &_touchIOHandle));
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = HW_LCD_V_RES,
        .y_max = HW_LCD_H_RES,
        .rst_gpio_num = static_cast<gpio_num_t>(-1),
        .int_gpio_num = static_cast<gpio_num_t>(-1),
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gt911(_touchIOHandle, &tp_cfg, &_touchHandle));
    lv_init();
    void *buf1 = nullptr;
    void *buf2 = nullptr;
    buf1 = heap_caps_malloc(HW_LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    lv_disp_draw_buf_init(&_displayBuff, buf1, buf2, HW_LCD_H_RES * 100);
    lv_disp_drv_init(&_displayDriver);
    _displayDriver.hor_res = HW_LCD_H_RES;
    _displayDriver.ver_res = HW_LCD_V_RES;
    _displayDriver.flush_cb = lvglFlush;
    _displayDriver.draw_buf = &_displayBuff;
    _displayDriver.user_data = _panelHandle;
}

void DisplayDriver::start(const uint32_t usStackDepth, UBaseType_t uxPriority)
{
   lv_disp_t *disp = lv_disp_drv_register(&_displayDriver);
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lvglTick,
        .name = "LGVLTCK"};

    lv_indev_drv_init(&_devDr);
    _devDr.type = LV_INDEV_TYPE_POINTER;
    _devDr.disp = disp;
    _devDr.read_cb = lvglTouch;
    _devDr.user_data = _touchHandle;

    lv_indev_drv_register(&_devDr);
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 2000));

    _lvglLock = xSemaphoreCreateRecursiveMutex();
    assert(_lvglLock);
    xTaskCreate(lvglWorkingTask, "LVGLTSK", usStackDepth, this, uxPriority, nullptr);
}

void DisplayDriver::resetTouch()
{
    constexpr uint8_t resetCommands[][2] = {
        {0x24, 0x01}, // Write 0x01 to address 0x24
        {0x38, 0x2C}, // Write 0x2C to address 0x38
        {0x38, 0x2E}  // Write 0x2E to address 0x38
    };

    for (const auto &cmd : resetCommands)
    {
        i2c_master_write_to_device(HW_I2C_NUM, cmd[0], &cmd[1], 1, HW_I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
        esp_rom_delay_us((cmd[1] == 0x2C) ? 100 * 1000 : ((cmd[1] == 0x2E) ? 200 * 1000 : 0));
    }

    // Reset
    gpio_set_level(GPIO_INPUT_IO_4, 0);
    esp_rom_delay_us(100 * 1000);
}

bool DisplayDriver::lock(int timeout_ms)
{
    const TickType_t tc = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(_lvglLock, tc) == pdTRUE;
}

void DisplayDriver::unlock()
{
    xSemaphoreGiveRecursive(_lvglLock);
}

void DisplayDriver::lvglWorkingTask(void *arg)
{
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1

    DisplayDriver *dd = static_cast<DisplayDriver *>(arg);

    uint32_t task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
    while (true)
    {

        if (dd->lock(-1))
        {
            task_delay_ms = lv_timer_handler();
            dd->unlock();
        }
        if (task_delay_ms > LVGL_TASK_MAX_DELAY_MS)
        {
            task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
        }
        else if (task_delay_ms < LVGL_TASK_MIN_DELAY_MS)
        {
            task_delay_ms = LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

bool DisplayDriver::lvglVsynEvent(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data)
{
    BaseType_t awoken = pdFALSE;
    return awoken == pdTRUE;
}

void DisplayDriver::lvglTick(void *arg)
{
    lv_tick_inc(3);
}

void DisplayDriver::lvglFlush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;

    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    lv_disp_flush_ready(drv);
}

void DisplayDriver::lvglTouch(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t touchpadX[1] = {0};
    uint16_t touchpadY[1] = {0};
    uint8_t touchpadCounter = 0;

    esp_lcd_touch_handle_t touchHandle = static_cast<esp_lcd_touch_handle_t>(drv->user_data);
    esp_lcd_touch_read_data(touchHandle);

    bool touched = esp_lcd_touch_get_coordinates(touchHandle, touchpadX, touchpadY, NULL, &touchpadCounter, 1);

    if (touched && touchpadCounter > 0)
    {
        data->point.x = touchpadX[0];
        data->point.y = touchpadY[0];
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void DisplayDriver::initLCD()
{
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = HW_LCD_PIXEL_CLOCK,
            .h_res = HW_LCD_H_RES,
            .v_res = HW_LCD_V_RES,
            .hsync_pulse_width = 4,
            .hsync_back_porch = 8,
            .hsync_front_porch = 8,
            .vsync_pulse_width = 4,
            .vsync_back_porch = 16,
            .vsync_front_porch = 16,

            .flags = {
                .pclk_active_neg = true,
            },
        },
        .data_width = 16, // RGB565
        .bits_per_pixel = 0,
        .num_fbs = 1, // only one frame buffer
        .dma_burst_size = 64,
        .hsync_gpio_num = HW_LCD_HSYNC,
        .vsync_gpio_num = HW_LCD_VSYNC,
        .de_gpio_num = HW_LCD_DE,
        .pclk_gpio_num = HW_LCD_PCLK,
        .disp_gpio_num = LCD_PIN_NUM_DISP_EN,
        .data_gpio_nums = {
            HW_LCD_DATA_0,
            HW_LCD_DATA_1,
            HW_LCD_DATA_2,
            HW_LCD_DATA_3,
            HW_LCD_DATA_4,
            HW_LCD_DATA_5,
            HW_LCD_DATA_6,
            HW_LCD_DATA_7,
            HW_LCD_DATA_8,
            HW_LCD_DATA_9,
            HW_LCD_DATA_10,
            HW_LCD_DATA_11,
            HW_LCD_DATA_12,
            HW_LCD_DATA_13,
            HW_LCD_DATA_14,
            HW_LCD_DATA_15,
        },
        .flags = {
            .fb_in_psram = true,
        },
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &_panelHandle));
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = lvglVsynEvent,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(_panelHandle, &cbs, &_displayDriver));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(_panelHandle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(_panelHandle));
}
