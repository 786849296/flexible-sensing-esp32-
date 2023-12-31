#include "ADC1.h"
#include <string.h>

float raw_ele[1500];
int raw_res[160][8];
int len = 0;
extern uint8_t cd4051bmt_channel;

#ifdef ADC_MODE_CONTINUOUS

#define _ADC_UNIT_STR(unit) #unit
#define ADC_UNIT_STR(unit) _ADC_UNIT_STR(unit)
#define ADC_GET_CHANNEL(p) ((p)->type2.channel)
#define ADC_GET_DATA(p) ((p)->type2.data)

adc_continuous_handle_t adc1_init()
{
    adc_continuous_handle_t handle = NULL;
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = READ_LEN,
        .conv_frame_size = READ_LEN,
        .flags.flush_pool = true
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));
    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2
    };
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = 2;
    adc_channel_t channel[2] = {ADC_CHANNEL_3, ADC_CHANNEL_4};
    for (int i = 0; i < 2; i++)
    {
        adc_pattern[i].atten = ADC_ATTEN_DB_11;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = ADC_UNIT_1;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));
    memset(raw_ele, 0, sizeof(float) * 1500);
    return handle;
}

bool adc1_read(adc_continuous_handle_t handle)
{
    char unit[] = ADC_UNIT_STR(ADC_UNIT_1);
    esp_err_t ret;
    uint32_t ret_num = 0;
    ret = adc_continuous_read(handle, result, READ_LEN, &ret_num, 1);
    bool flag_result = false;
    if (ret == ESP_OK)
        // ESP_LOGI("ADC", "ret is %x, ret_num is %"PRIu32" bytes", ret, ret_num);
        flag_result = true;
    else if (ret == ESP_ERR_TIMEOUT)
        ESP_LOGI("ADC", "time out");
    //ESP_ERROR_CHECK(adc_continuous_stop(handle));
    return flag_result;
}

void get_voltage(adc_cali_handle_t handle, uint8_t channel)
{
    uint32_t data;
    for (int i = 0; i < READ_LEN; i += SOC_ADC_DIGI_RESULT_BYTES)
    {
        adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
        uint32_t chan_num = ADC_GET_CHANNEL(p);
        data = ADC_GET_DATA(p);
        if (chan_num == 4 && (len % 8) < 6)
            raw_res[len / 8][len % 8] = data;
        else if(chan_num == 3)
        {
            int temp;
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(handle, data, &temp));
            raw_ele[len * 8 + cd4051bmt_channel] = temp / 1000.0;
        }
        //ESP_LOGI("cail", "ADC%d Channel[%"PRIu32"] Cali Voltage: %"PRIu32" mV", ADC_UNIT_1 + 1, chan_num, v.voltage[i / SOC_ADC_DIGI_RESULT_BYTES]);
    }
}

#else

adc_oneshot_unit_handle_t adc1_init()
{
    adc_oneshot_unit_handle_t handle;
    adc_oneshot_unit_init_cfg_t adc_init_cfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_init_cfg, &handle));
    adc_oneshot_chan_cfg_t adc_chan_cfg = {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = SOC_ADC_RTC_MAX_BITWIDTH
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_3, &adc_chan_cfg));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_4, &adc_chan_cfg));
    return handle;
}

void adc1_read(adc_oneshot_unit_handle_t adc_handle, adc_cali_handle_t cali_handle, int cd4051_chan)
{
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_3, &adc_data[cd4051_chan + 8]));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_data[cd4051_chan + 8], &adc_data[cd4051_chan + 8]));
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_4, &adc_data[cd4051_chan]));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_data[cd4051_chan], &adc_data[cd4051_chan]));
    return;
}

#endif

adc_cali_handle_t adc_cali_init()
{
    adc_cali_handle_t handle = NULL;
    adc_cali_curve_fitting_config_t cail_cfg = {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = 12,
        .unit_id = ADC_UNIT_1
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cail_cfg, &handle));
    return handle;
}
