//#define APP_TRACE

#include <stdio.h>
#include <string.h>
#include "CD4051BMT.h"
#include "ADC1.h"
#include "TIMER.h"
#include "algorithm.h"
#include "find_peaks.h"
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

const int peak_distance_bcg = 59; // 心率寻峰算法distance参数
const int peak_distance_breath = 200; // 呼吸率寻峰算法distance参数
const float peak_height_breath = 1.3; // 呼吸率寻峰算法height参数

int now_rate_bcg = 0; // 当前心率值，用于输出
int now_rate_breath = 0; // 当心呼吸率值，用于输出
bool round_one_flag = true; // 判断首次在床的标志
int count_breath = 0; // 用于判断呼吸暂停的计数器

float signal_bcg[1200];
float signal_breath[1200];

void app_main(void)
{
    gptimer_handle_t timer_handle = gptimer_init();
    gptimer_handle_t timer2_handle = gptimer2_init();
    ESP_ERROR_CHECK(gptimer_start(timer_handle));

    cd4051bmt_init();
    //uart_init();
    bool led = 0;
    
    //等待采集一轮数据，100ms
    //esp_rom_delay_us(100 * 1000);

    BWBandPass* filter_bd = create_bw_band_pass_filter(FILTER_N, FILTER_FS, FILTER_BP_WC_LOW, FILTER_BP_WC_HIGH);
    BWLowPass* filter_low = create_bw_low_pass_filter(FILTER_N, FILTER_FS, FILTER_LOW_WC);
    while(!flag_collect)
        vTaskDelay(1);

    while(1)
    {
#ifdef APP_TRACE
        char buf[10] = {0};
        esp_err_t error;
        while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_JTAG));
        for (int i = 0; i < 16; i++)
        {

            int temp = adc_data[i];
            ESP_LOGI("main", "cd4051bmt_channel: %d, Cali Voltage: %d mV", i, temp);
            snprintf(buf, sizeof(buf), "%d ", temp);
            error = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, buf, strlen(buf), ESP_APPTRACE_TMO_INFINITE);
            esp_apptrace_flush(ESP_APPTRACE_DEST_JTAG, 100);
        }
        error = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, "\n", sizeof(char), ESP_APPTRACE_TMO_INFINITE);
        esp_apptrace_flush(ESP_APPTRACE_DEST_JTAG, 100);
#endif
        //state: 0离床， 1在床， 2移动
        int state = 0;
        if (flag_collect)
        {
            // printf("len: %d\n", len);
            int len_temp = len;
            // 判断状态
            for (int i = len_temp - 6; i < len_temp - 1; i++)
                if (is_bodyMove(raw_res[i], raw_res[i + 1], 6))
                {
                    state = 2;
                    round_one_flag = true;
                    break;
                }
                else if (state != 1 && is_onBed(raw_res[i], 6))
                    state = 1;
            if (state == 0 && round_one_flag == false)
                round_one_flag = true;
            
            // printf("state: %d\n", state);
            switch (state)
            {
            case 0:
                printf("状态: 离床 \n");
                break;
            case 1:
                printf("状态: 在床 \n");
                break;
            case 2:
                if (!flag_cooldown)
                {
                    flag_cooldown = true;
                    ESP_ERROR_CHECK(gptimer_start(timer2_handle));
                }
                printf("状态: 体动 \n");
                break;
            }
            
            if (state == 1)
            {
                float max_bcg = 0, min_bcg = 5;
                // 双向滤波 分离心冲击信号与呼吸信号
                for (int i = len_temp * 8; i > -1; i--)
                {
                    signal_bcg[i] = bw_band_pass(filter_bd, raw_ele[i]);
                    signal_breath[i] = bw_low_pass(filter_low, raw_ele[i]);
                }
                for (int i = 0; i < len_temp * 8; i++)
                {
                    signal_bcg[i] = bw_band_pass(filter_bd, raw_ele[i]);
                    signal_breath[i] = bw_low_pass(filter_low, raw_ele[i]);
                    max_bcg = fmax(max_bcg, signal_bcg[i]);
                    min_bcg = fmin(min_bcg, signal_bcg[i]);
                }
                memset(filter_bd->w0, 0, sizeof(float) * filter_bd->n);
                memset(filter_bd->w1, 0, sizeof(float) * filter_bd->n);
                memset(filter_bd->w2, 0, sizeof(float) * filter_bd->n);
                memset(filter_bd->w3, 0, sizeof(float) * filter_bd->n);
                memset(filter_bd->w4, 0, sizeof(float) * filter_bd->n);
                memset(filter_low->w0, 0, sizeof(float) * filter_low->n);
                memset(filter_low->w1, 0, sizeof(float) * filter_low->n);
                memset(filter_low->w2, 0, sizeof(float) * filter_low->n);
                int len_bcg = len_temp * 8;
                int len_breath = len_temp * 8;
                // bcg信号差分
                len_bcg = cal_diff(signal_bcg, len_bcg);
                // bcg信号幅度缩放
                int a = 2; // 缩放到(-2， 2)区间
                float k = 2.0 * a / (max_bcg - min_bcg);
                // 信号指数放大
                for (int x = 0; x < len_bcg; x++)
                {
                    signal_bcg[x] = k * (signal_bcg[x] - max_bcg) + a;
                    signal_bcg[x] = exp(signal_bcg[x]) - 1;
                }
                
                // 心率计算
                int peak_count_bcg = 0;
                int* peak_all_bcg = fun_findAllPeaks(signal_bcg, len_bcg, &peak_count_bcg);
                int* peak_byheight_bcg = fun_selectbyHeight(signal_bcg, peak_all_bcg, peak_count_bcg, &peak_count_bcg);
                int* peak_bcg = fun_selectbyDistance(signal_bcg, peak_byheight_bcg, peak_count_bcg, peak_distance_bcg, &peak_count_bcg);
                len_bcg = peak_diff(peak_bcg, peak_count_bcg);
                len_bcg = peak3_zip(peak_bcg, len_bcg);

                free(peak_all_bcg);
                free(peak_byheight_bcg);


                int *rate_bcg = (int*)malloc(len_bcg * sizeof(int));
                for (int i = 0; i < len_bcg; i++)
                {
                    rate_bcg[i] = 180 / (peak_bcg[i] / FILTER_FS);
                    // printf("rate_bcg: %d ", rate_bcg[i]);
                }
                // printf("\n");
                free(peak_bcg);

                if (len_bcg > 0)
                {
                    // 判断是否为首次在床
                    if (round_one_flag)
                        {
                            now_rate_bcg = rate_bcg[len_bcg - 1];
                            round_one_flag = false;
                        }
                    // 判断是否心率发生改变或过度异常
                    if (now_rate_bcg != rate_bcg[len_bcg - 1] && (abs(now_rate_bcg - rate_bcg[len_bcg - 1]) < 15))
                        now_rate_bcg = rate_bcg[len_bcg - 1];
                }
                
                free(rate_bcg);
                
                printf("心率: %d \n", now_rate_bcg);
                if (now_rate_bcg < 50)
                    printf("心率过慢 \n");
                else if (now_rate_bcg > 120)
                    printf("心率过快 \n");
                

                // 呼吸率计算
                int peak_count_breath = 0;
                int* peak_all_breath = fun_findAllPeaks(signal_breath, len_breath, &peak_count_breath);
                int* peak_byheight_breath = fun_selectbyHeight(signal_breath, peak_all_breath, peak_count_breath, &peak_count_breath);
                int* peak_breath = fun_selectbyDistance(signal_breath, peak_byheight_breath, peak_count_breath, peak_distance_breath, &peak_count_breath);
                len_breath = peak_diff(peak_breath, peak_count_breath);
                
                free(peak_all_breath);
                free(peak_byheight_breath);
                // printf("%d \n", len_breath);
                if (len_breath > 0)
                {
                    int *rate_breath = (int*)malloc(len_breath * sizeof(int));
                    for (int i = 0; i < len_breath; i++)
                    {
                        rate_breath[i] = 60 / (peak_breath[i] / FILTER_FS);
                        // printf("rate_breath: %d ", rate_breath[i]);
                    }
                    // printf("\n");

                    // if (round_one_flag)
                    //     now_rate_breath = rate_breath[len_breath - 1];

                    // printf("%d\n", now_rate_breath);
                    // printf("%d\n", rate_breath[len_breath - 1]);

                    if (now_rate_breath != rate_breath[len_breath - 1])
                        now_rate_breath = rate_breath[len_breath - 1];
                    printf("呼吸率: %d \n", now_rate_breath);
                    count_breath = 0;
                    free(rate_breath);
                }
                else
                {
                    //添加定时效果
                    count_breath++;
                    if (count_breath >= 20) // 单次循环为0.5秒,20次为10秒
                    {
                        now_rate_breath = 0;
                        printf("呼吸率: %d \n", now_rate_breath);
                    }
                    else // 维持原有数值
                        printf("呼吸率: %d \n", now_rate_breath);
                }
                free(peak_breath);
            }
            // printf("%d \n", count_breath);
            for (int i = 0; i < len - 5; i++)
                for (int c = 0; c < 8; c++)
                {
                    raw_ele[i * 8 + c] = raw_ele[(i + 5) * 8 + c];
                    if (c < 6)
                        raw_res[i][c] = raw_res[i + 5][c];
                }
            len -= 5;
            flag_collect = false;
            led = !led;
            gpio_set_level(GPIO_NUM_45, led);
            printf("\n");
            vTaskDelay(1);
        }

        // if (flag_collect)
        // {
        //     //ESP_ERROR_CHECK(gptimer_stop(timer_handle));
        //     for (int i = 0; i < 16; i++)
        //         if (i < 6)
        //             printf("%d ", raw_res[len - 1][i]);
        //         else if (i > 7)
        //             printf("%f ", raw_ele[(len - 1) * 8 + i]);
        //     printf("\n");
        //     flag_collect = false;
        //     //ESP_ERROR_CHECK(gptimer_start(timer_handle));
        //     led = !led;
        //     gpio_set_level(GPIO_NUM_45, led);
        // }
        // // printf(" ");
        // // printf("cd4051bmt_channel: %d\n", cd4051bmt_channel);
        

    }
}
