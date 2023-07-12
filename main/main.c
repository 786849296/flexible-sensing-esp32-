//#define APP_TRACE

#include <stdio.h>
#include <string.h>
#include "CD4051BMT.h"
#include "ADC1.h"
#include "TIMER.h"
// #include "UART.h"
// #include "soc/clk_tree_defs.h"
// #include "esp_clk_tree.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "esp_task_wdt.h"

#ifdef APP_TRACE
#include "esp_app_trace.h"
#endif

void app_main(void)
{
    cd4051bmt_init();
    gptimer_handle_t timer_handle = gptimer_init();
    //uart_init();

    cd4051bmt_channel = 0;
    ESP_ERROR_CHECK(gptimer_start(timer_handle));
    //等待采集一轮数据，100ms
    esp_rom_delay_us(100 * 1000);

    while(1)
    {
#ifdef APP_TRACE
        char buf[10] = {0};
        esp_err_t error;
        while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_JTAG));
        for (int i = 0; i < 16; i++)
        {

            int temp = oneshot_data[i];
            ESP_LOGI("main", "cd4051bmt_channel: %d, Cali Voltage: %d mV", i, temp);
            snprintf(buf, sizeof(buf), "%d ", temp);
            error = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, buf, strlen(buf), ESP_APPTRACE_TMO_INFINITE);
            esp_apptrace_flush(ESP_APPTRACE_DEST_JTAG, 100);
        }
        error = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, "\n", sizeof(char), ESP_APPTRACE_TMO_INFINITE);
        esp_apptrace_flush(ESP_APPTRACE_DEST_JTAG, 100);

#else

        if (flag_collect)
        {
            ESP_ERROR_CHECK(gptimer_stop(timer_handle));
            for (int i = 0; i < 16; i++)
                printf("%d ", oneshot_data[i]);
            printf("\n");
            flag_collect = false;
            ESP_ERROR_CHECK(gptimer_start(timer_handle));
        }
        vTaskDelay(1);

#endif    
    }

}
