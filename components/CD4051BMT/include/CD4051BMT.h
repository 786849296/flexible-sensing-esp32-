#pragma once

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"

void cd4051bmt_init();
void cd4051bmt_channel_set(uint8_t channel);