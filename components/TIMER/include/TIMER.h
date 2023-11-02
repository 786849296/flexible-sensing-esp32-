#pragma once

#include "driver/gptimer.h"
#include "esp_log.h"
#include "ADC1.h"
#include "CD4051BMT.h"

extern bool flag_collect;
extern bool flag_cooldown;
extern int cpm_bodyMove;

//ADC定时器，10ms，周期
gptimer_handle_t gptimer_init();
//体动定时器，10s，单次
gptimer_handle_t gptimer2_init();