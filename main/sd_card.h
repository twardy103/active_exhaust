#ifndef __SD_CARD_H__
#define __SD_CARD_H__

#include <string.h>
#include <sys/unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include <string.h>

void sd_card_init();
void read_from_sd(char* file);
void write_to_sd(char* file);

#endif /* __SD_CARD_H__ */
