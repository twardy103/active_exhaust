#ifndef __BT_H__
#define __BT_H__

#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"

void bt_main();
void bt_app_gap_start_up(void);
bool get_name_from_eir(uint8_t *eir, uint8_t *bdname, uint8_t *bdname_len);

#endif /* __BT_H__ */
