/**
 * @file ir_wrap.c
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ir_wrap.h"


/* IR protocols example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "ir_tools.h"


#define CONFIG_EXAMPLE_RMT_TX_CHANNEL 0
#define CONFIG_EXAMPLE_RMT_RX_CHANNEL 0
#define CONFIG_EXAMPLE_RMT_TX_GPIO    44
#define CONFIG_EXAMPLE_RMT_RX_GPIO    -1

#define CONFIG_EXAMPLE_IR_PROTOCOL_NEC 1



static const char *TAG = "ir warp";

static rmt_channel_t example_tx_channel = CONFIG_EXAMPLE_RMT_TX_CHANNEL;
static rmt_channel_t example_rx_channel = CONFIG_EXAMPLE_RMT_RX_CHANNEL;

/**
 * @brief RMT Receive Task
 *
 */
static void example_ir_rx_task(void *arg)
{
    uint32_t addr = 0;
    uint32_t cmd = 0;
    size_t length = 0;
    bool repeat = false;
    RingbufHandle_t rb = NULL;
    rmt_item32_t *items = NULL;

    rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(CONFIG_EXAMPLE_RMT_RX_GPIO, example_rx_channel);
    rmt_config(&rmt_rx_config);
    rmt_driver_install(example_rx_channel, 1000, 0);
    ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)example_rx_channel);
    ir_parser_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
    ir_parser_t *ir_parser = NULL;
#if CONFIG_EXAMPLE_IR_PROTOCOL_NEC
    ir_parser = ir_parser_rmt_new_nec(&ir_parser_config);
#elif CONFIG_EXAMPLE_IR_PROTOCOL_RC5
    ir_parser = ir_parser_rmt_new_rc5(&ir_parser_config);
#endif

    //get RMT RX ringbuffer
    rmt_get_ringbuf_handle(example_rx_channel, &rb);
    assert(rb != NULL);
    // Start receive
    rmt_rx_start(example_rx_channel, true);
    while (1) {
        items = (rmt_item32_t *) xRingbufferReceive(rb, &length, portMAX_DELAY);
        if (items) {
            length /= 4; // one RMT = 4 Bytes
            if (ir_parser->input(ir_parser, items, length) == ESP_OK) {
                if (ir_parser->get_scan_code(ir_parser, &addr, &cmd, &repeat) == ESP_OK) {
                    ESP_LOGI(TAG, "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);
                }
            }
            //after parsing the data, return spaces to ringbuffer.
            vRingbufferReturnItem(rb, (void *) items);
        }
    }
    ir_parser->del(ir_parser);
    rmt_driver_uninstall(example_rx_channel);
    vTaskDelete(NULL);
}

/**
 * @brief RMT Transmit Task
 *
 */
static void example_ir_tx_task(void *arg)
{
    uint32_t addr = 0x10;
    uint32_t cmd = 0x20;
    rmt_item32_t *items = NULL;
    size_t length = 0;
    ir_builder_t *ir_builder = NULL;

    rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(CONFIG_EXAMPLE_RMT_TX_GPIO, example_tx_channel);
    rmt_tx_config.tx_config.carrier_en = true;
    rmt_config(&rmt_tx_config);
    rmt_driver_install(example_tx_channel, 0, 0);
    ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)example_tx_channel);
    ir_builder_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
#if CONFIG_EXAMPLE_IR_PROTOCOL_NEC
    ir_builder = ir_builder_rmt_new_nec(&ir_builder_config);
#elif CONFIG_EXAMPLE_IR_PROTOCOL_RC5
    ir_builder = ir_builder_rmt_new_rc5(&ir_builder_config);
#endif
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI(TAG, "Send command 0x%x to address 0x%x", cmd, addr);
        // Send new key code
        ESP_ERROR_CHECK(ir_builder->build_frame(ir_builder, addr, cmd));
        ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &items, &length));
        //To send data according to the waveform items.
        rmt_write_items(example_tx_channel, items, length, false);
        // Send repeat code
        vTaskDelay(pdMS_TO_TICKS(ir_builder->repeat_period_ms));
        ESP_ERROR_CHECK(ir_builder->build_repeat_frame(ir_builder));
        ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &items, &length));
        rmt_write_items(example_tx_channel, items, length, false);
        cmd++;
    }
    ir_builder->del(ir_builder);
    rmt_driver_uninstall(example_tx_channel);
    vTaskDelete(NULL);
}



static uint8_t _is_inited = 0;
static ir_builder_t *ir_builder = NULL;


void ir_wrap_init(void)
{
    // xTaskCreate(example_ir_rx_task, "ir_rx_task", 2048, NULL, 10, NULL);
    // xTaskCreate(example_ir_tx_task, "ir_tx_task", 2048, NULL, 10, NULL);

    if (_is_inited == 0)
    {
        // uint32_t addr = 0x10;
        // uint32_t cmd = 0x20;
        // rmt_item32_t *items = NULL;
        // size_t length = 0;
        // ir_builder_t *ir_builder = NULL;

        rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(CONFIG_EXAMPLE_RMT_TX_GPIO, example_tx_channel);
        rmt_tx_config.tx_config.carrier_en = true;
        rmt_config(&rmt_tx_config);
        rmt_driver_install(example_tx_channel, 0, 0);
        ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)example_tx_channel);
        ir_builder_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
        #if CONFIG_EXAMPLE_IR_PROTOCOL_NEC
            ir_builder = ir_builder_rmt_new_nec(&ir_builder_config);
        #elif CONFIG_EXAMPLE_IR_PROTOCOL_RC5
            ir_builder = ir_builder_rmt_new_rc5(&ir_builder_config);
        #endif

        _is_inited = 1;
    }
}


void ir_wrap_send(uint8_t addr, uint8_t cmd)
{
    // uint32_t addr = 0x10;
    // uint32_t cmd = 0x20;
    rmt_item32_t *items = NULL;
    size_t length = 0;
    // ir_builder_t *ir_builder = NULL;

    ESP_LOGI(TAG, "Send command 0x%x to address 0x%x", cmd, addr);
    // Send new key code
    ESP_ERROR_CHECK(ir_builder->build_frame(ir_builder, addr, cmd));
    ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &items, &length));
    //To send data according to the waveform items.
    rmt_write_items(example_tx_channel, items, length, false);
    // Send repeat code
    vTaskDelay(pdMS_TO_TICKS(ir_builder->repeat_period_ms));
    ESP_ERROR_CHECK(ir_builder->build_repeat_frame(ir_builder));
    ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &items, &length));
    rmt_write_items(example_tx_channel, items, length, false);
    // cmd++;
}


void ir_wrap_deinit(void)
{
    if (_is_inited == 0)
        return;

    ir_builder->del(ir_builder);
    rmt_driver_uninstall(example_tx_channel);
}

