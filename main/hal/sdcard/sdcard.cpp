/**
 * @file sdcard.cpp
 * @author Anderson Antunes
 * @brief 
 * @version 0.1
 * @date 2024-01-14
 *
 * @copyright Copyright (c) 2024
 * 
 */
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sdcard.h"

#define PIN_NUM_MISO GPIO_NUM_39
#define PIN_NUM_MOSI GPIO_NUM_14
#define PIN_NUM_CLK  GPIO_NUM_40
#define PIN_NUM_CS   GPIO_NUM_12

static const char *MOUNT_POINT = "/sdcard";
static const char *TAG = "SDCARD";

bool SDCard::mount(bool format_if_mount_failed) {
    if (mounted) {
        ESP_LOGI(TAG, "SD card already mounted");
        return true;
    }
    esp_err_t ret;
    ESP_LOGI(TAG, "Initializing SD card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT(); 
    spi_bus_config_t bus_cfg;
    bus_cfg.mosi_io_num = PIN_NUM_MOSI;
    bus_cfg.miso_io_num = PIN_NUM_MISO;
    bus_cfg.sclk_io_num = PIN_NUM_CLK;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.data4_io_num = -1,
    bus_cfg.data5_io_num = -1,
    bus_cfg.data6_io_num = -1,
    bus_cfg.data7_io_num = -1,
    bus_cfg.max_transfer_sz = 4000; 
    bus_cfg.flags = (SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI);
    bus_cfg.intr_flags = 0; 

    ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return false;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = (spi_host_device_t)host.slot;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = format_if_mount_failed,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        card = nullptr;
        spi_bus_free((spi_host_device_t)host.slot);
        ESP_LOGE(TAG, "Failed to mount filesystem.");
        return false;
    }

    sdmmc_card_print_info(stdout, card);
    mounted = true;
    return true;
}

bool SDCard::eject() {
    if (!mounted) {
        ESP_LOGI(TAG, "SD card not mounted");
        return true;
    }
    esp_err_t ret = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unmount SD card");
        return false;
    }
    card = nullptr;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT(); 
    spi_bus_free((spi_host_device_t)host.slot);
    ESP_LOGI(TAG, "Card unmounted and SPI bus freed");
    mounted = false;
    return true;
}

bool SDCard::is_mounted() {
    return mounted;
}

char *SDCard::get_mount_point() {
    char *mount_point = (char*)malloc(strlen(MOUNT_POINT) + 1);
    strcpy(mount_point, MOUNT_POINT);
    return mount_point;
}

char *SDCard::get_filepath(const char *path) {
    int len = strlen(MOUNT_POINT) + strlen(path) + 2;
    ESP_LOGI(TAG, "get_filepath: %d", len);
    char *full_path = (char*)malloc(len);
    strcpy(full_path, MOUNT_POINT);
    strcat(full_path, "/");
    strcat(full_path, path);
    return full_path;
}

bool SDCard::file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}