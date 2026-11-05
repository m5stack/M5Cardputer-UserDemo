/**
 * @file sntp_wrap.c
 * @author Forairaaaaa
 * @brief Network time synchronization for SetWiFi.
 * @copyright Copyright (c) 2023
 */
#include "sntp_wrap.h"
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sntp.h"

static const char* TAG = "sntp";

static void time_sync_notification_cb(struct timeval* tv) { ESP_LOGI(TAG, "Notification of a time synchronization event"); }

bool sntp_warp_init(void)
{
    // Each SetWiFi request must obtain fresh time, even if the current year
    // looks valid. Stop the previous client before configuring another request.
    esp_sntp_stop();
#if LWIP_DHCP_GET_NTP_SRV
    esp_sntp_servermode_dhcp(1);
#endif
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
    sntp_set_sync_status(SNTP_SYNC_STATUS_RESET);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);

    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_init();

    bool synced = false;
    const int retry_count = 10;
    for (int retry = 0; retry <= retry_count; ++retry)
    {
        // Read this once: a COMPLETED status is cleared by the getter.
        if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED)
        {
            synced = true;
            break;
        }
        if (retry == retry_count)
        {
            break;
        }
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry + 1, retry_count);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    // SetWiFi disconnects Wi-Fi after this call; do not leave a client running
    // after a timeout or let an old request satisfy the next attempt.
    esp_sntp_stop();
    if (!synced)
    {
        ESP_LOGW(TAG, "Time synchronization timed out");
        return false;
    }

    // Preserve the demo's existing local timezone.
    setenv("TZ", "CST-8", 1);
    tzset();
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
    return true;
}
