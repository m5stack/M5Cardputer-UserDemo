// /**
//  * @file hal_display.hpp
//  * @author Forairaaaaa
//  * @brief
//  * @version 0.1
//  * @date 2023-06-28
//  *
//  * @copyright Copyright (c) 2023
//  *
//  */
// #pragma once
// // #include <M5GFX.h>
// // #include <lgfx/v1/panel/Panel_ST7789.hpp>

// #define LGFX_USE_V1
// #include <LovyanGFX.hpp>

// #define LCD_MOSI_PIN 35
// #define LCD_MISO_PIN -1
// #define LCD_SCLK_PIN 36
// #define LCD_DC_PIN   34
// #define LCD_CS_PIN   37
// #define LCD_RST_PIN  33
// #define LCD_BUSY_PIN -1
// #define LCD_BL_PIN   38

// class LGFX_Cardputer : public lgfx::LGFX_Device {
//     lgfx::Panel_ST7789 _panel_instance;
//     lgfx::Bus_SPI _bus_instance;
//     lgfx::Light_PWM _light_instance;

//    public:
//     LGFX_Cardputer(void) {
//         {
//             auto cfg = _bus_instance.config();

//             cfg.pin_mosi   = LCD_MOSI_PIN;
//             cfg.pin_miso   = LCD_MISO_PIN;
//             cfg.pin_sclk   = LCD_SCLK_PIN;
//             cfg.pin_dc     = LCD_DC_PIN;
//             cfg.freq_write = 40000000;

//             _bus_instance.config(cfg);
//             _panel_instance.setBus(&_bus_instance);
//         }
//         {
//             auto cfg = _panel_instance.config();

//             cfg.invert       = true;
//             cfg.pin_cs       = LCD_CS_PIN;
//             cfg.pin_rst      = LCD_RST_PIN;
//             cfg.pin_busy     = LCD_BUSY_PIN;
//             // cfg.panel_width  = 240;
//             // cfg.panel_height = 240;
//             // cfg.offset_x     = 52;
//             // cfg.offset_y     = 40;
//             cfg.panel_width  = 135;
//             cfg.panel_height = 240;
//             cfg.offset_x     = 52;
//             cfg.offset_y     = 40;

//             _panel_instance.config(cfg);
//         }
//         {   // バックライト制御の設定を行います。（必要なければ削除）
//             auto cfg = _light_instance.config();    // バックライト設定用の構造体を取得します。

//             cfg.pin_bl = LCD_BL_PIN;              // バックライトが接続されているピン番号
//             cfg.invert = false;           // バックライトの輝度を反転させる場合 true
//             // cfg.freq   = 44100;           // バックライトのPWM周波数
//             cfg.freq   = 200;           // バックライトのPWM周波数
//             // cfg.freq   = 500;           // バックライトのPWM周波数
//             cfg.pwm_channel = 7;          // 使用するPWMのチャンネル番号

//             _light_instance.config(cfg);
//             _panel_instance.setLight(&_light_instance);  // バックライトをパネルにセットします。
//         }

//         setPanel(&_panel_instance);
//     }
// };
