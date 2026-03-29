#include "esp_netif.h"
#include "esp_wifi_types_generic.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include <assert.h>
#include <stdint.h>

#define RECORDS_LEN 5
#define LED_GPIO GPIO_NUM_2

const char* TAG = "wifi";

void blinker(void* pvParameters) {
    uint8_t* done = (uint8_t*)pvParameters;
    esp_err_t rt = gpio_reset_pin(LED_GPIO);
    if (rt != ESP_OK) {
        ESP_LOGE(TAG, "error resetting gpio %d\n", LED_GPIO);
        exit(1);
    }
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    while (!(*done)) {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(CONFIG_FREERTOS_HZ);
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(CONFIG_FREERTOS_HZ);
    }
    while (1) {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(CONFIG_FREERTOS_HZ / 500e-3);
    }
}

void scanner() {
    // need to create network interface first
    uint8_t done = 0;
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t* sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    const wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT(); // sta mode (aka station mode)
    ESP_ERROR_CHECK(esp_wifi_init(&conf));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    TaskHandle_t handler = NULL;
    xTaskCreatePinnedToCore(blinker, "blinker", 3000, &done, 1, &handler, tskNO_AFFINITY);
    ESP_LOGI(TAG, "scanning...\n");
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
    // vTaskDelete(handler);
    done = 1;
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
