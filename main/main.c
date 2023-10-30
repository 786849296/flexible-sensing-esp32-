//#define APP_TRACE

#include <stdio.h>
#include <string.h>
#include "CD4051BMT.h"
#include "ADC1.h"
#include "TIMER.h"
#include "MIC.h"
#include "algorithm.h"
#include "find_peaks.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "DAC081S101.h"

#ifdef APP_TRACE
#include "esp_app_trace.h"
#endif

int adc_value = 0;
int temp[6];
float zero_crossing_rate = 0;
float short_time_energy = 0;
#define ADC1_CHAN1 7//GPIO5
//列8选1开关的切换
#define ARM2DEC_COL_SEL0 42
#define ARM2DEC_COL_SEL1 41
#define ARM2DEC_COL_SEL2 40
//行的4选1开关切换
#define ARM2DEC_MUX_SEL0 12
#define ARM2DEC_MUX_SEL1 13
#define ARM2DEC_MUX_SEL2 14 
//行8选1开关的切换
#define ARM2DEC_ROW_SEL0 21
#define ARM2DEC_ROW_SEL1 47
#define ARM2DEC_ROW_SEL2 48
//列的使能端
#define ARM2DEC_COL_EN0 46
#define ARM2DEC_COL_EN1 9
#define ARM2DEC_COL_EN2 10
#define ARM2DEC_COL_EN3 11


void app_main(void)
{
    gptimer_handle_t timer_handle = gptimer_init();
    ESP_ERROR_CHECK(gptimer_start(timer_handle));
    while (true)
    {
        if (flag_collect)
        {
            //ESP_ERROR_CHECK(gptimer_stop(timer_handle));
            len = 0;
            get_voltage(NULL);
            //ESP_ERROR_CHECK(gptimer_start(timer_handle));
            zero_crossing_rate = cal_zero_crossing_rate(raw, len);
            short_time_energy = cal_short_time_energy(raw, len);
            for (size_t i = 0; i < len; i++)
                printf("%d\n", raw[i]);
            // printf("%d\n", temp_len);
            flag_collect = false;
            vTaskDelay(1);
        }
    }
}
