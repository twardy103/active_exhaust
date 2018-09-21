/* Active Exhaust project v1.0
Features:
 - SD card handling (reading, writing)
 - DAC handling (file from SD card pushed on DAC using I2S)
 - Switch interruption
*/
#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "sd_card.h"
#include "dac.h"

SemaphoreHandle_t xSemaphore = NULL;
bool led_status = false;

// interrupt service routine, called when the button is pressed
void IRAM_ATTR button_isr_handler(void* arg) {

	// notify the button task
	xSemaphoreGiveFromISR(xSemaphore, NULL);
}

void printChipInfo() {
	/* Print chip information */
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ", chip_info.cores,
			(chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
			(chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

	printf("silicon revision %d, ", chip_info.revision);

	printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
			(chip_info.features & CHIP_FEATURE_EMB_FLASH) ?
					"embedded" : "external");
}

void led_blinker(void *pvParameter) {
	printf("[%s]\n", __FUNCTION__);
	gpio_pad_select_gpio(GPIO_NUM_23);
	esp_err_t result = gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
	printf("Result: [%d]\n", result);
	while (1) {
		gpio_set_level(GPIO_NUM_23, 0);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		gpio_set_level(GPIO_NUM_23, 1);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

// task that will react to button clicks
void button_task(void* arg) {
	// infinite loop
	while (1) {
		// wait for the notification from the ISR
		if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
			printf("Button pressed! Led status: [%d]\n", led_status);
			led_status = !led_status;
			gpio_set_level(GPIO_NUM_5, led_status);
		}
	}
	vTaskDelete(NULL);
}

void init_gpio_for_switch() {
	printf("[%s]\n", __FUNCTION__);
	// create the binary semaphore
	xSemaphore = xSemaphoreCreateBinary();

	gpio_pad_select_gpio(GPIO_NUM_4);
	esp_err_t result = gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
	printf("Result: [%d]\n", result);

	gpio_set_intr_type(GPIO_NUM_4, GPIO_PIN_INTR_NEGEDGE);

	// install ISR service with default configuration
	gpio_install_isr_service(0);

	// attach the interrupt service routine
	gpio_isr_handler_add(GPIO_NUM_4, button_isr_handler, NULL);
}

void init_gpio_for_led() {
	//Configure GPIO for LED (for switch interruption)
	gpio_pad_select_gpio(GPIO_NUM_5);
	gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
}

esp_err_t app_main() {
	printf("############################	  Active exhaust v1.0   #################################\n");
	printChipInfo();

	printf("############################	  LED blinker           #################################\n");
	xTaskCreate(led_blinker, "ledBlinker", 1024 * 2, NULL, 5, NULL);

	printf("############################	  SWITCH handler        #################################\n");
	init_gpio_for_switch();
	init_gpio_for_led();
	xTaskCreate(button_task, "button_task", 1024 * 2, NULL, 5, NULL);

	printf("############################	  SD card handler       #################################\n");
	sd_card_init();
	char *file = "/sdcard/test.txt";
	write_to_sd(file);
	read_from_sd(file);

	printf("############################	  DAC  handler          #################################\n");
	i2s_init();
	xTaskCreate(play_wav, "play_wav", 1024 * 2, NULL, 5, NULL);
	return ESP_OK;
}
