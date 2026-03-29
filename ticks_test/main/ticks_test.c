#include "freertos/FreeRTOS.h"
#include "FreeRTOSConfig.h"

void app_main(void) {
    printf("%d\n", CONFIG_FREERTOS_HZ);
}
