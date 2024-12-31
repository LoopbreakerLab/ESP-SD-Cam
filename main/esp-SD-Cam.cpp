#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "sdkconfig.h"

static const char *TAG = "SD-CAM-example";

#define BLINK_GPIO 33 //4 flash LED,33 build in LED
#define CONFIG_BLINK_PERIOD 1000

uint8_t led_state = 0;

static void blink_led() {
    gpio_set_level(static_cast<gpio_num_t>(BLINK_GPIO), led_state);
}

static void configure_led() {
    ESP_LOGI(TAG, "Configure GPIO LED!");
    gpio_reset_pin(static_cast<gpio_num_t>(BLINK_GPIO));
    gpio_set_direction(static_cast<gpio_num_t>(BLINK_GPIO), GPIO_MODE_OUTPUT);
}
extern "C" void app_main() {
    configure_led();

    while (true) {
        ESP_LOGI(TAG, "Turning the LED %s!", led_state == true ? "ON" : "OFF");
        blink_led();
        led_state = !led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
