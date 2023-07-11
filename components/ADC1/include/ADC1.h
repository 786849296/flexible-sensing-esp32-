#pragma once

//ADC_MODE_CONTINUOUS有bug，序号对不上，且有电压采集有时有误
//define ADC_MODE_CONTINUOUS
#define ADC_MODE_ONESHOT

#ifdef ADC_MODE_CONTINUOUS
    #include "esp_adc/adc_continuous.h"
#else
    #include "esp_adc/adc_oneshot.h"
#endif
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

adc_cali_handle_t adc_cail_init();
#ifdef ADC_MODE_CONTINUOUS

    #define READ_NUM 2
    #define READ_LEN SOC_ADC_DIGI_DATA_BYTES_PER_CONV * READ_NUM

    static uint8_t result[READ_LEN] = { 0 };

    typedef struct Adc_data {
    uint8_t cd4051bmt_channel;
    uint32_t channel[READ_NUM];
    uint32_t voltage[READ_NUM];
    }Adc_data;
    extern Adc_data adc_data;

    adc_continuous_handle_t adc1_init();
    bool adc1_read(adc_continuous_handle_t handle);
    Adc_data get_voltage(adc_cali_handle_t handle);
#else
    extern int oneshot_data[16];
    adc_oneshot_unit_handle_t adc1_init();
    void adc1_read(adc_oneshot_unit_handle_t adc_handle, adc_cali_handle_t cali_handle, int cd4051_chan);
#endif
