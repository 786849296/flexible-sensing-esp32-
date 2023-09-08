#include "TIMER.h"

adc_cali_handle_t cali_handle;
bool flag_collect = false;

#ifdef ADC_MODE_CONTINUOUS

adc_continuous_handle_t adc_handle;

static bool timer_on_alarm_cb_cd4051bmt_channel_change(gptimer_handle_t handle, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    bool adc_flag;
    uint8_t channel = cd4051bmt_channel;
    cd4051bmt_channel = (cd4051bmt_channel + 1) % 8;
    cd4051bmt_channel_set(cd4051bmt_channel);
    adc_flag = adc1_read(adc_handle);
    if (adc_flag)
        get_voltage(cali_handle, channel);
    if (!cd4051bmt_channel)
    {
        len++;
        if (len >= 150)
            flag_collect = true;
        if (len == 170)
            len = 0;
    }
    //ESP_ERROR_CHECK(gptimer_stop(timer_handle));
    //cd4051bmt_channel_temp = cd4051bmt_channel;
    //ESP_ERROR_CHECK(gptimer_start(timer_handle));
    //ESP_LOGI("timer", "timer alarm on, cd4051bmt_channel: %d", cd4051bmt_channel);
    return true;
}

#else

adc_oneshot_unit_handle_t adc_handle;

static bool timer_on_alarm_cb_cd4051bmt_channel_change(gptimer_handle_t handle, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    esp_err_t ret;
    adc1_read(adc_handle, cali_handle, cd4051bmt_channel);
    //ret = adc_oneshot_get_calibrated_result(adc_handle, cali3_handle, ADC_CHANNEL_3, &adc_data[cd4051bmt_channel + 8]);
    //ret = adc_oneshot_get_calibrated_result(adc_handle, cali4_handle, ADC_CHANNEL_4, &adc_data[cd4051bmt_channel]);
    cd4051bmt_channel = (cd4051bmt_channel + 1) % 8;
    cd4051bmt_channel_set(cd4051bmt_channel);
    if (!cd4051bmt_channel)
        flag_collect = true;
    // gptimer_set_raw_count(handle, 0);
    return true;
}

#endif

gptimer_handle_t gptimer_init()
{
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        //只能选择TIMER_SRC_CLK_XTAL时钟源。选择TIMER_SRC_CLK_APB时钟源导致初始化失败。原因未知。
        .clk_src = TIMER_SRC_CLK_XTAL,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    gptimer_alarm_config_t alarm_cfg = 
    {
        .alarm_count = 10,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_cfg));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_on_alarm_cb_cd4051bmt_channel_change
    };
    uint64_t test[10];
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, test));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    adc_handle = adc1_init();
    cali_handle = adc_cali_init();

#ifdef ADC_MODE_CONTINUOUS

    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));

#endif

    return gptimer;
}