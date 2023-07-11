#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CD4051BMT.h"
#include "ADC1.h"
#include "TIMER.h"
//#include "UART.h"
// #include "soc/clk_tree_defs.h"
// #include "esp_clk_tree.h"
#include "esp_app_trace.h"
#include "esp_log.h"

void app_main(void)
{
    cd4051bmt_init();
    gptimer_handle_t timer_handle = gptimer_init();

    //uart_init();

    // uint32_t freq_cpu, freq_apb, freq_pll_d2, freq_adc;
    // ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq_cpu));
    // ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_APB, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq_apb));
    // ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_PLL_D2, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq_pll_d2));
    // ESP_LOGI("freq", "cpu: %"PRIu32", apb: %"PRIu32", pll_div2: %"PRIu32, freq_cpu, freq_apb, freq_pll_d2);
    // ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(ADC_DIGI_CLK_SRC_APB, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq_adc));
    // ESP_LOGI("freq", "adc_apb: %"PRIu32, freq_adc);
    // ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(ADC_DIGI_CLK_SRC_PLL_F240M, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq_adc));
    // ESP_LOGI("freq", "adc_pll_f240m: %"PRIu32, freq_adc);
    cd4051bmt_channel = 0;
    ESP_ERROR_CHECK(gptimer_start(timer_handle));
    while(1)
    {
        for (int i = 0; i < 16; i++)
        {
            ESP_LOGI("main", "cd4051bmt_channel: %d, Cali Voltage: %d mV", i, oneshot_data[i]);
        }
        
        // for (int i = 0; i < 2; i++)
        // {
        //     esp_err_t error;
        //     char* str = "0";
        //     itoa(adc_data.channel[i], str, 10);
        //     error = esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, str, strlen(str), ESP_APPTRACE_TMO_INFINITE);
        //     ESP_ERROR_CHECK(error);
        //     itoa(adc_data.voltage[i], str, 10);
        //     ESP_ERROR_CHECK(esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, str, strlen(str), ESP_APPTRACE_TMO_INFINITE));
        // }
    }
}
