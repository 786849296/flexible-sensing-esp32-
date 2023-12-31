#pragma once

#include "driver/gptimer.h"
#include "esp_log.h"
#include "ADC1.h"
#include "CD4051BMT.h"

#define SLEEP_MONITORING_PERIOD 2 //睡眠状态检测周期 单位(min)

extern bool flag_collect;
extern bool flag_cooldown;
extern bool flag_snoring;
extern int cpm_bodyMove;
extern float count_rate_bcg;
extern double cpm_rate_bcg;
extern double cpm_rate_bcg_wake;
extern int status;
extern bool state_flag;

//ADC定时器，10ms，周期
gptimer_handle_t gptimer_init();
//体动定时器，10s，单次
gptimer_handle_t gptimer2_init();