#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <driver/i2c_master.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "ui.h"
#include "vars.h"

#define TAG "ssd1306_demo"

#define PIN_SDA 21
#define PIN_SCL 22

#define LCD_PIXEL_CLOCK_HZ (400 * 1000)
#define LCD_I2C_HW_ADDR 0x3C
#define LCD_H_RES 128
#define LCD_V_RES 64

char my_label[100] = { 0 };
const char *get_var_my_label() {
    return my_label;
}
void set_var_my_label(const char *value) {
    strncpy(my_label, value, sizeof(my_label) / sizeof(char));
    my_label[sizeof(my_label) / sizeof(char) - 1] = 0;
}


void app_main(void)
{
    //Create I2C bus
    ESP_LOGI(TAG, "Create I2C bus");
    
    i2c_master_bus_handle_t busHandle = NULL;
    i2c_master_bus_config_t busConfig = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = PIN_SDA,
        .scl_io_num = PIN_SCL,
        .i2c_port = 0,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    ESP_ERROR_CHECK( i2c_new_master_bus(&busConfig, &busHandle));

    //Allocate an LCD IO device handle for I2C interface
    ESP_LOGI(TAG, "Allocate an LCD IO device handle");

    esp_lcd_panel_io_handle_t ioHandle = NULL;
    esp_lcd_panel_io_i2c_config_t ioConfig = {
        .dev_addr = LCD_I2C_HW_ADDR,
        .scl_speed_hz = LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6
    };

    ESP_ERROR_CHECK( esp_lcd_new_panel_io_i2c(busHandle, &ioConfig, &ioHandle));

    //Install SSD1306 LCD controller driver
    ESP_LOGI(TAG, "Install SSD1306 LCD controller driver");
    esp_lcd_panel_handle_t lcdHandle = NULL;
    esp_lcd_panel_dev_config_t lcdConfig = {
        .reset_gpio_num = -1,
        .bits_per_pixel = 1
    };

    ESP_ERROR_CHECK( esp_lcd_new_panel_ssd1306(ioHandle, &lcdConfig, &lcdHandle));

    ESP_ERROR_CHECK( esp_lcd_panel_reset(lcdHandle));
    ESP_ERROR_CHECK( esp_lcd_panel_init(lcdHandle));
    ESP_ERROR_CHECK( esp_lcd_panel_disp_on_off(lcdHandle, true));

    //Initialize LVGL graphic library
    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvglConfig = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvglConfig);

    //Add LCD screen
    ESP_LOGI(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t displayConfig = {
        .io_handle = ioHandle,
        .panel_handle = lcdHandle,
        .buffer_size = LCD_H_RES * LCD_V_RES,
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false
        }
    };

    lvgl_port_add_disp(&displayConfig);
    ui_init();
    set_var_my_label("Git");

    ESP_LOGI(TAG, "Display LVGL Scroll Text");
    if(lvgl_port_lock(0)) {
        ui_tick();
        lvgl_port_unlock();
    }

}
