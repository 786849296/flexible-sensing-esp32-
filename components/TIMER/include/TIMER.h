#pragma once

#include "driver/gptimer.h"
#include "esp_log.h"
#include "ADC1.h"
#include "CD4051BMT.h"

gptimer_handle_t gptimer_init();