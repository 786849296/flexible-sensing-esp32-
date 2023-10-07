#pragma once

#define ADC_MODE_CONTINUOUS
//#define ADC_MODE_ONESHOT

#ifdef ADC_MODE_CONTINUOUS
    #include "esp_adc/adc_continuous.h"
#else
    #include "esp_adc/adc_oneshot.h"
#endif
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

// #include "freertos/timers.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern int len;
extern uint16_t raw[1024 * 15 + 10];

adc_cali_handle_t adc_cali_init();
#ifdef ADC_MODE_CONTINUOUS

    #define READ_NUM 1024 * 15
    #define READ_LEN SOC_ADC_DIGI_DATA_BYTES_PER_CONV * READ_NUM

    static uint8_t result[READ_LEN] = { 0 };

    // typedef struct Adc_data {
    // uint8_t cd4051bmt_channel;
    // uint32_t channel[READ_NUM];
    // uint32_t voltage[READ_NUM];
    // }Adc_data;
    // extern Adc_data adc_data;

    adc_continuous_handle_t adc1_init();
    bool adc1_read(adc_continuous_handle_t handle);
    void get_voltage(adc_cali_handle_t handle);
    void adc_print(void *par);
#else
    adc_oneshot_unit_handle_t adc1_init();
    void adc1_read(adc_oneshot_unit_handle_t adc_handle, adc_cali_handle_t cali_handle, int cd4051_chan);
#endif
