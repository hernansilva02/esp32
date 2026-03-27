#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_2   // En muchas placas ESP32 clásicas el LED onboard suele ser GPIO2

void app_main(void) {
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(LED_GPIO, 1);   // LED encendido
        vTaskDelay(pdMS_TO_TICKS(1000));

        printf("toy anadando");

        gpio_set_level(LED_GPIO, 0);   // LED apagado
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
