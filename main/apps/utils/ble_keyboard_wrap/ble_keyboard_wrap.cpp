/**
 * @file ble_keyboard_wrap.cpp
 * @author Forairaaaaa
 * @brief Ref: https://github.com/Gitshaoxiang/M5Cardputer-BLE-HID-Keyboard
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/* This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this software is
   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"

#include "esp_bt_main.h"
#include "esp_bt_device.h"

#include "esp_hidd.h"
#include "esp_hid_gap.h"

// #include <M5Unified.h>
// #include "Keyboard.h"

// Keyboard_Class Keyboard = Keyboard_Class();

#include "ble_keyboard_wrap.h"
static KEYBOARD::Keyboard* _keyboard = nullptr;
static BleKbWrapState_t _keyboard_current_state =  ble_kb_wrap_state_wait_connect;


static const char *TAG = "HID_DEV_DEMO";

typedef struct {
    xTaskHandle task_hdl;
    esp_hidd_dev_t *hid_dev;
    uint8_t protocol_mode;
    uint8_t *buffer;
} local_param_t;

static local_param_t s_ble_hid_param = {0};

const unsigned char hidReportMap[] = {

    0x05, 0x01,  // Usage Pg (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection: (Application)
    0x85, 0x01,  // Report Id (1)
    //
    0x05, 0x07,  //   Usage Pg (Key Codes)
    0x19, 0xE0,  //   Usage Min (224)
    0x29, 0xE7,  //   Usage Max (231)
    0x15, 0x00,  //   Log Min (0)
    0x25, 0x01,  //   Log Max (1)
    //
    //   Modifier byte
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x08,  //   Report Count (8)
    0x81, 0x02,  //   Input: (Data, Variable, Absolute)
    //
    //   Reserved byte
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x08,  //   Report Size (8)
    0x81, 0x01,  //   Input: (Constant)
    //
    //   LED report
    0x95, 0x05,  //   Report Count (5)
    0x75, 0x01,  //   Report Size (1)
    0x05, 0x08,  //   Usage Pg (LEDs)
    0x19, 0x01,  //   Usage Min (1)
    0x29, 0x05,  //   Usage Max (5)
    0x91, 0x02,  //   Output: (Data, Variable, Absolute)
    //
    //   LED report padding
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x03,  //   Report Size (3)
    0x91, 0x01,  //   Output: (Constant)
                 //
                 //   Key arrays (6 bytes)
    0x95, 0x06,  // Report Count (6)
    0x75, 0x08,  // Report Size (8)
    0x15, 0x00,  // Log Min (0)
    0x25, 0xF1,  // Log Max (241)
    0x05, 0x07,  // Usage Pg (Key Codes)
    0x19, 0x00,  // Usage Min (0)
    0x29, 0xf1,  // Usage Max (241)
    0x81, 0x00,  // Input: (Data, Array)
    //
    0xC0,  // End Collection
};

static esp_hid_raw_report_map_t ble_report_maps[] = {
    {.data = hidReportMap, .len = sizeof(hidReportMap)}};

static esp_hid_device_config_t ble_hid_config = {
    .vendor_id         = 0x16C0,
    .product_id        = 0x05DF,
    .version           = 0x0100,
    .device_name       = "M5-Keyboard",
    .manufacturer_name = "Espressif",
    .serial_number     = "1234567890",
    .report_maps       = ble_report_maps,
    .report_maps_len   = 1};

#define HID_RPT_ID_CC_IN  3  // Consumer Control input report ID
#define HID_CC_IN_RPT_LEN 2  // Consumer Control input report Len
void esp_hidd_send_consumer_value(uint8_t key_cmd, bool key_pressed) {
    uint8_t buffer[HID_CC_IN_RPT_LEN] = {0, 0};

    esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 0, HID_RPT_ID_CC_IN, buffer,
                           HID_CC_IN_RPT_LEN);
    return;
}

// void ble_hid_demo_task(void *pvParameters) {
//     // static bool send_volum_up = false;
//     ESP_LOGI(TAG, "ble_hid_demo_task start");

//     // uint8_t buffer1[8] = {0, 0, 0x17, 0, 0, 0, 0, 0};
//     uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//     while (1) {
//         // _keyboard->updateKeyList();
//         // _keyboard->updateKeysState();
//         if (_keyboard->isChanged()) {
//             uint8_t modifier = 0;
//             if (_keyboard->isPressed()) {
//                 memset(buffer, 0, 8);
//                 auto status = _keyboard->keysState();

//                 int count = 0;
//                 for (auto i : status.hidKey) {
//                     if (count < 6) {
//                         buffer[2 + count] = i;
//                         count++;
//                     }
//                 }

//                 if (status.ctrl) {
//                     ESP_LOGI(TAG, "CONTROL");
//                     modifier |= 0x01;
//                 }

//                 if (status.shift) {
//                     ESP_LOGI(TAG, "SHIFT");
//                     modifier |= 0x02;
//                 }

//                 if (status.alt) {
//                     ESP_LOGI(TAG, "ALT");
//                     modifier |= 0x03;
//                 }

//                 buffer[0] = modifier;

//                 ESP_LOG_BUFFER_HEX(TAG, buffer, 8);

//                 esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 0, 1, buffer,
//                                        8);
//             } else {
//                 memset(buffer, 0, 8);
//                 esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 0, 1, buffer,
//                                        8);
//             }
//         }
//         vTaskDelay(50 / portTICK_PERIOD_MS);
//     }
// }


// static StaticTask_t _xTaskBuffer;
// static StackType_t* _ble_hid_update_task_stack = nullptr;

void ble_hid_task_start_up(void) {
    // xTaskCreate(ble_hid_demo_task, "ble_hid_update", 2000, NULL,
    //             configMAX_PRIORITIES - 3, &s_ble_hid_param.task_hdl);

    // // Dynamic 
    // _ble_hid_update_task_stack = new StackType_t[4000];
    // s_ble_hid_param.task_hdl = xTaskCreateStatic(ble_hid_demo_task, "ble_hid_update", 4000, NULL, 
    // configMAX_PRIORITIES - 3, _ble_hid_update_task_stack, &_xTaskBuffer);

    _keyboard_current_state = ble_kb_wrap_state_connected;
}

void ble_hid_task_shut_down(void) {
    // if (s_ble_hid_param.task_hdl) {
    //     vTaskDelete(s_ble_hid_param.task_hdl);
    //     s_ble_hid_param.task_hdl = NULL;

    //     // delete[] _ble_hid_update_task_stack;
    // }

    _keyboard_current_state = ble_kb_wrap_state_wait_connect;
}

static void ble_hidd_event_callback(void *handler_args, esp_event_base_t base,
                                    int32_t id, void *event_data) {
    esp_hidd_event_t event       = (esp_hidd_event_t)id;
    esp_hidd_event_data_t *param = (esp_hidd_event_data_t *)event_data;
    static const char *TAG       = "HID_DEV_BLE";

    // printf("stack remain: %d\n", (unsigned int)uxTaskGetStackHighWaterMark(NULL));

    switch (event) {
        case ESP_HIDD_START_EVENT: {
            // ESP_LOGI(TAG, "START");
            esp_hid_ble_gap_adv_start();
            break;
        }
        case ESP_HIDD_CONNECT_EVENT: {
            // ESP_LOGI(TAG, "CONNECT");
            // ble_hid_task_start_up();  // todo: this should be on auth_complete
            //                           // (in GAP)
            _keyboard_current_state = ble_kb_wrap_state_connected;

            // M5.Display.clear();
            // M5.Display.fillRect(10, M5.Display.height() / 2 - 15,
            //                     M5.Display.width() - 20, 30, GREEN);

            // M5.Display.drawString("CONNECTED!", M5.Display.width() / 2,
            //                       M5.Display.height() / 2);

            break;
        }
        case ESP_HIDD_PROTOCOL_MODE_EVENT: {
            // ESP_LOGI(TAG, "PROTOCOL MODE[%u]: %s",
            //          param->protocol_mode.map_index,
            //          param->protocol_mode.protocol_mode ? "REPORT" : "BOOT");
            break;
        }
        case ESP_HIDD_CONTROL_EVENT: {
            // ESP_LOGI(TAG, "CONTROL[%u]: %sSUSPEND", param->control.map_index,
            //          param->control.control ? "EXIT_" : "");
            break;
        }
        case ESP_HIDD_OUTPUT_EVENT: {
            // ESP_LOGI(TAG, "OUTPUT[%u]: %8s ID: %2u, Len: %d, Data:",
            //          param->output.map_index,
            //          esp_hid_usage_str(param->output.usage),
            //          param->output.report_id, param->output.length);
            // ESP_LOG_BUFFER_HEX(TAG, param->output.data, param->output.length);
            break;
        }
        case ESP_HIDD_FEATURE_EVENT: {
            // ESP_LOGI(TAG, "FEATURE[%u]: %8s ID: %2u, Len: %d, Data:",
            //          param->feature.map_index,
            //          esp_hid_usage_str(param->feature.usage),
            //          param->feature.report_id, param->feature.length);
            // ESP_LOG_BUFFER_HEX(TAG, param->feature.data, param->feature.length);
            break;
        }
        case ESP_HIDD_DISCONNECT_EVENT: {
            // ESP_LOGI(TAG, "DISCONNECT: %s",
            //          esp_hid_disconnect_reason_str(
            //              esp_hidd_dev_transport_get(param->disconnect.dev),
            //              param->disconnect.reason));
            // ble_hid_task_shut_down();
            esp_hid_ble_gap_adv_start();
            _keyboard_current_state = ble_kb_wrap_state_wait_connect;

            // M5.Display.clear();

            // M5.Display.fillRect(10, M5.Display.height() / 2 - 15,
            //                     M5.Display.width() - 20, 30, RED);

            // M5.Display.drawString("DISCONNECT", M5.Display.width() / 2,
            //                       M5.Display.height() / 2);

            break;
        }
        case ESP_HIDD_STOP_EVENT: {
            // ESP_LOGI(TAG, "STOP");
            break;
        }
        default:
            break;
    }
    return;
}

void ble_keyboard_wrap_init(KEYBOARD::Keyboard* keyboard) {

    // Copy pointer 
    _keyboard = keyboard;


    esp_err_t ret;
#if HID_DEV_MODE == HIDD_IDLE_MODE
    ESP_LOGE(TAG, "Please turn on BT HID device or BLE!");
    return;
#endif
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // auto cfg = M5.config();

    // M5.begin();

    // M5.Display.begin();

    // M5.begin(cfg);
    // M5.Display.setRotation(1);
    // M5.Display.setTextColor(BLACK);
    // M5.Display.setTextDatum(middle_center);
    // M5.Display.setTextFont(&fonts::FreeSerifBoldItalic12pt7b);
    // M5.Display.setTextSize(1);

    // M5.Display.fillRect(10, M5.Display.height() / 2 - 15,
    //                     M5.Display.width() - 20, 30, BLUE);
    // M5.Display.drawString("BLE KEYBOARD", M5.Display.width() / 2,
    //                       M5.Display.height() / 2);

    // Keyboard.begin();

    ESP_LOGI(TAG, "setting hid gap, mode:%d", HID_DEV_MODE);
    ret = esp_hid_gap_init(HID_DEV_MODE);
    ESP_ERROR_CHECK(ret);

    ret = esp_hid_ble_gap_adv_init(ESP_HID_APPEARANCE_GENERIC,
                                   ble_hid_config.device_name);
    ESP_ERROR_CHECK(ret);

    if ((ret = esp_ble_gatts_register_callback(esp_hidd_gatts_event_handler)) !=
        ESP_OK) {
        ESP_LOGE(TAG, "GATTS register callback failed: %d", ret);
        return;
    }
    ESP_LOGI(TAG, "setting ble device");
    ESP_ERROR_CHECK(esp_hidd_dev_init(&ble_hid_config, ESP_HID_TRANSPORT_BLE,
                                      ble_hidd_event_callback,
                                      &s_ble_hid_param.hid_dev));
}


void ble_keyboard_wrap_deinit()
{
    esp_hidd_dev_deinit(s_ble_hid_param.hid_dev);
}


void ble_keyboard_wrap_update_input(uint8_t* input)
{
    esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 0, 1, input, 8);
}


BleKbWrapState_t ble_keyboard_wrap_get_current_state()
{
    return _keyboard_current_state;
}


const char* ble_keyboard_wrap_get_device_name()
{
    return ble_hid_config.device_name;
}
