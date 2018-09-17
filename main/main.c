/* Active Exhaust project v1.0
Features:
 - SD card handling
 - ...
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "sd_card.h"

void printChipInfo() {
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

void ledBlinker(void *pvParameter) {
    printf("[%s]\n", __FUNCTION__);
	gpio_pad_select_gpio(GPIO_NUM_23);
	esp_err_t result = gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
    printf("Result: [%d]\n", result);
	while (1) {
		gpio_set_level(GPIO_NUM_23, 0);
		vTaskDelay(1000/portTICK_PERIOD_MS);
		gpio_set_level(GPIO_NUM_23, 1);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

void app_main()
{
	printf("############################	  Active exhaust v1.0   #################################\n");
	printChipInfo();

    printf("############################	  LED blinker           #################################\n");
   // xTaskCreate(&ledBlinker, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);

    printf("############################	  SD card handler       #################################\n");
    sd_card_handler();
}
