#include "TIMER.h"

adc_cali_handle_t cali_handle;
bool flag_collect = false;
int count_bodyMove = 0; // 体动的计数器
int cnt = 0;
int cpm_bodyMove = 0;   // 每分钟体动的计数
float count_rate_bcg = 0;
double cpm_rate_bcg = 0;
double cpm_rate_bcg_wake = 0;
//在床：0 清醒：1 浅睡：2 深睡：3
int status = 1;

#ifdef ADC_MODE_CONTINUOUS

adc_continuous_handle_t adc_handle;

static bool timer_on_alarm_cb_cd4051bmt_channel_change(gptimer_handle_t handle, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    bool adc_flag;
    uint8_t channel = cd4051bmt_channel;
    adc_flag = adc1_read(adc_handle);
    if (adc_flag)
        get_voltage(cali_handle, channel);
    cd4051bmt_channel = (cd4051bmt_channel + 1) % 8;
    cd4051bmt_channel_set(cd4051bmt_channel);
    if (!cd4051bmt_channel)
    {
        len++;
        if (len >= 150)
            flag_collect = true;
        if (len == 170)
            len = 0;
    }
    
    if (state_flag)
    {
        cnt = 0;
        count_bodyMove = 0;
        count_rate_bcg = 0;
        status = 1;
    }

    if (cnt++ == 100 * 60 * SLEEP_MONITORING_PERIOD)
    {
        cnt = 0;
        cpm_bodyMove = count_bodyMove;
        count_bodyMove = 0;
        cpm_rate_bcg = count_rate_bcg / (150.0 * SLEEP_MONITORING_PERIOD);
        count_rate_bcg = 0;
        switch (status)
        {
        case 1:
            if (cpm_bodyMove < 3)
            {
                if (cpm_rate_bcg < cpm_rate_bcg_wake * 0.98)
                {
                    //TODO: 开启鼾声检测
                    status = 2;
                    break;
                }
            }
            cpm_rate_bcg_wake = cpm_rate_bcg;
            break;
        case 2:
            if (cpm_bodyMove >= 3 || cpm_rate_bcg > cpm_rate_bcg_wake * 1.02)
            {
                status = 1;
                // cpm_rate_bcg_wake = cpm_rate_bcg;
            }
            else if (cpm_rate_bcg < cpm_rate_bcg_wake * 0.94)
                status = 3;
            break;
        case 3:
            if (cpm_bodyMove >= 3)
            {
                status = 1;
                // cpm_rate_bcg_wake = cpm_rate_bcg;
            }
            else if (cpm_bodyMove >= 1 || cpm_rate_bcg > cpm_rate_bcg_wake * 0.94)
                status = 2;
            break;
        default:
            status = 1;
            cpm_rate_bcg_wake = cpm_rate_bcg;
            break;
        }
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

bool flag_cooldown = false;

static bool timer2_on_alarm_cb_bodyMove(gptimer_handle_t handle, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    count_bodyMove++;
    flag_cooldown = false;
    ESP_ERROR_CHECK(gptimer_stop(handle));
    return true;
}

gptimer_handle_t gptimer2_init()
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
        .alarm_count = 10 * 1000,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_cfg));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer2_on_alarm_cb_bodyMove
    };
    uint64_t test[10];
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, test));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    return gptimer;
}