#include "driver/dac.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spi_flash.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "driver/i2s.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

//i2s number
#define EXAMPLE_I2S_NUM           (0)
//i2s data bits
#define EXAMPLE_I2S_SAMPLE_BITS   (16)
//i2s sample rate
#define EXAMPLE_I2S_SAMPLE_RATE   (16000)
//I2S data format
#define EXAMPLE_I2S_FORMAT        (I2S_CHANNEL_FMT_RIGHT_LEFT)
//I2S channel number
#define EXAMPLE_I2S_CHANNEL_NUM   ((EXAMPLE_I2S_FORMAT < I2S_CHANNEL_FMT_ONLY_RIGHT) ? (2) : (1))
//I2S read buffer length
#define EXAMPLE_I2S_READ_LEN      (16 * 1024)
#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_7)
#define V_REF   				  1100
#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

void dac_init();
void example_i2s_init();
int example_i2s_dac_data_scale(uint8_t* d_buff, uint8_t* s_buff, uint32_t len);
void example_set_file_play_mode();
void example_reset_play_mode();
void example_disp_buf(uint8_t* buf, int length);
void play_wav();
