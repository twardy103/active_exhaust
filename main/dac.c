#include "dac.h"
#include "stdio.h"
#include "audio_file.h"

/**
 * @brief I2S ADC/DAC mode init.
 */
void i2s_init() {
    printf("I2S init");
    int i2s_num = EXAMPLE_I2S_NUM;
    i2s_config_t i2s_config = { .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN
            | I2S_MODE_ADC_BUILT_IN, .sample_rate = EXAMPLE_I2S_SAMPLE_RATE, .bits_per_sample =
    EXAMPLE_I2S_SAMPLE_BITS, .communication_format = I2S_COMM_FORMAT_I2S_MSB, .channel_format =
    EXAMPLE_I2S_FORMAT, .intr_alloc_flags = 0, .dma_buf_count = 2, .dma_buf_len = 1024 };
    //install and start i2s driver
    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    //init DAC pad
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
    //init ADC pad
    //i2s_set_adc_mode(I2S_ADC_UNIT, I2S_ADC_CHANNEL);
}

/**
 * @brief Scale data to 16bit/32bit for I2S DMA output.
 *        DAC can only output 8bit data value.
 *        I2S DMA will still send 16 bit or 32bit data, the highest 8bit contains DAC data.
 */
int i2s_dac_data_scale(uint8_t* d_buff, uint8_t* s_buff, uint32_t len) {
    uint32_t j = 0;
#if (EXAMPLE_I2S_SAMPLE_BITS == 16)
    for (int i = 0; i < len; i++) {
        d_buff[j++] = 0;
        d_buff[j++] = s_buff[i];
    }
    return (len * 2);
#else
    for (int i = 0; i < len; i++) {
        d_buff[j++] = 0;
        d_buff[j++] = 0;
        d_buff[j++] = 0;
        d_buff[j++] = s_buff[i];
    }
    return (len * 4);
#endif
}

/**
 * @brief Set i2s clock for example audio file
 */
void set_file_play_mode() {
    i2s_set_clk(EXAMPLE_I2S_NUM, 16000, EXAMPLE_I2S_SAMPLE_BITS, 1);
}

/**
 * @brief Reset i2s clock and mode
 */
void reset_play_mode() {
    i2s_set_clk(EXAMPLE_I2S_NUM, EXAMPLE_I2S_SAMPLE_RATE,
    EXAMPLE_I2S_SAMPLE_BITS, EXAMPLE_I2S_CHANNEL_NUM);
}

/**
 * @brief debug buffer data
 */
void disp_buf(uint8_t* buf, int length) {
#if EXAMPLE_I2S_BUF_DEBUG
    printf("======\n");
    for (int i = 0; i < length; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("======\n");
#endif
}

void play_wav(void *pvParameter) {
    size_t bytes_written;
    uint8_t* i2s_write_buff = (uint8_t*) calloc(EXAMPLE_I2S_READ_LEN, sizeof(char));
    while (1) {
        printf("Playing file example: \n");
        int offset = 0;
        int tot_size = sizeof(player_defeated);
        set_file_play_mode();
        while (offset < tot_size) {
            int play_len = ((tot_size - offset) > (4 * 1024)) ? (4 * 1024) : (tot_size - offset);
            int i2s_wr_len = i2s_dac_data_scale(i2s_write_buff, (uint8_t*) (player_defeated + offset), play_len);
            i2s_write(EXAMPLE_I2S_NUM, i2s_write_buff, i2s_wr_len, &bytes_written, portMAX_DELAY);
            offset += play_len;
            disp_buf((uint8_t*) i2s_write_buff, 32);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        reset_play_mode();
    }
    free(i2s_write_buff);
    vTaskDelete(NULL);
}
