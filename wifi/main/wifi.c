#include "esp_netif.h"
#include "esp_wifi_types_generic.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include <assert.h>
#include <stdint.h>
#define RECORDS_LEN 5

const char* TAG = "wifi";

void scanner() {
    // need to create network interface first
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t* sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    const wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&conf)); // default wifi mode: STA (station mode)
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "scanning...\n");
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
    // stack version
    uint16_t nm = RECORDS_LEN;
    wifi_ap_record_t records[nm];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&nm, records));
    for (uint8_t i = 0; i < RECORDS_LEN; i++) {
        ESP_LOGI(TAG, "SSID: %s\nMAC: %s\n\n\n", records[i].ssid, records[i].bssid);
    }
}

void app_main() {
    esp_log_level_set("wifi", CONFIG_LOG_MAXIMUM_LEVEL);
    ESP_ERROR_CHECK(nvs_flash_init());
    scanner();
}
