#include <esp_vfs_fat.h>
#include <stdio.h>
#include <driver/sdmmc_default_configs.h>
#include <driver/sdmmc_host.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdmmc_cmd.h"

#include "sdkconfig.h"

static const char *TAG = "SD-CAM-example";

#define BLINK_GPIO 33 //4 flash LED,33 build in LED
#define CONFIG_BLINK_PERIOD 1000
#define MOUNT_POINT "/sdcard"

uint8_t led_state = 0;

static void blink_led() {
    gpio_set_level(static_cast<gpio_num_t>(BLINK_GPIO), led_state);
}

static void configure_led() {
    ESP_LOGI(TAG, "Configure GPIO LED!");
    gpio_reset_pin(static_cast<gpio_num_t>(BLINK_GPIO));
    gpio_set_direction(static_cast<gpio_num_t>(BLINK_GPIO), GPIO_MODE_OUTPUT);
}

static void initSD() {
    sdmmc_card_t* card;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    esp_err_t ret;

    ESP_LOGI(TAG, "Mounting filesystem");
    //ret = esp_vfs_fat_sdmmc_mount(&host, &slot_config, "/", &card)
    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);

    ESP_ERROR_CHECK(ret);

    // open file
    FILE* f = fopen("/sdcard/test.txt", "w");
    if (f == NULL) {
        ESP_LOGE("SD_MMC", "Failed to open file for writing");
        return;
    }

    // write text to file
    fprintf(f, "Hello world form ESP32 on sdcard.\n");
    fclose(f);

    ESP_LOGI("SD_MMC", "Data written to SD card");
}

extern "C" void app_main() {
    configure_led();

    initSD();

    while (true) {
        ESP_LOGI(TAG, "Turning the LED %s!", led_state == true ? "ON" : "OFF");
        blink_led();
        led_state = !led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
