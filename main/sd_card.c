#include "sd_card.h"

static const char *TAG = "SD_CARD";
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13

void sd_card_handler() {
	ESP_LOGI(TAG, "Initializing SD card");
	ESP_LOGI(TAG, "Using SPI peripheral");

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
	slot_config.gpio_miso = PIN_NUM_MISO;
	slot_config.gpio_mosi = PIN_NUM_MOSI;
	slot_config.gpio_sck  = PIN_NUM_CLK;
	slot_config.gpio_cs   = PIN_NUM_CS;
	// This initializes the slot without card detect (CD) and write protect (WP) signals.
	// Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.

	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
		.format_if_mount_failed = 0,
		.max_files = 5
	};

	sdmmc_card_t* card;
	esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount filesystem. "
				"If you want the card to be formatted, set format_if_mount_failed = true.");
		} else {
			ESP_LOGE(TAG, "Failed to initialize the card (%s). "
				"Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
		}
		return;
	}

	sdmmc_card_print_info(stdout, card);

	ESP_LOGI(TAG, "Opening file");
	FILE* f = fopen("/sdcard/hello.txt", "w");
	if (f == NULL) {
		ESP_LOGE(TAG, "Failed to open file for writing");
		return;
	}
	fprintf(f, "Hello %s!\n", card->cid.name);
	fclose(f);
	ESP_LOGI(TAG, "File written");

	// Check if destination file exists before renaming
	struct stat st;
	if (stat("/sdcard/foo.txt", &st) == 0) {
		// Delete it if it exists
		unlink("/sdcard/foo.txt");
	}

	// Rename original file
	ESP_LOGI(TAG, "Renaming file");
	if (rename("/sdcard/hello.txt", "/sdcard/foo.txt") != 0) {
		ESP_LOGE(TAG, "Rename failed");
		return;
	}

	// Open renamed file for reading
	ESP_LOGI(TAG, "Reading file");
	f = fopen("/sdcard/foo.txt", "r");
	if (f == NULL) {
		ESP_LOGE(TAG, "Failed to open file for reading");
		return;
	}
	char line[64];
	fgets(line, sizeof(line), f);
	fclose(f);
	// strip newline
	char* pos = strchr(line, '\n');
	if (pos) {
		*pos = '\0';
	}
	ESP_LOGI(TAG, "Read from file: '%s'", line);

	//esp_vfs_fat_sdmmc_unmount();
	//ESP_LOGI(TAG, "Card unmounted");
}
