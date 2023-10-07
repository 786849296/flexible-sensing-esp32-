#pragma once

#include "driver/gpio.h"
#include "esp_log.h"

#define DAC_SYNC GPIO_NUM_16
#define DAC_CLK GPIO_NUM_17
#define DAC_DIN GPIO_NUM_18

void dac081s101_init();
void dac(uint8_t data);
