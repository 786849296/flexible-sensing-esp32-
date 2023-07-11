#include "UART.h"

uint8_t data_receive[128] = {0};

void uart_init()
{
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 0, NULL, 0));
    uart_config_t uart_cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_cfg));
    //uart0: rts-15, cts-16(这里不使用流控)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, 37, 36, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_mode(UART_NUM_0, UART_MODE_UART));
}

void uart_write(char* str)
{
    if (uart_write_bytes(UART_NUM_0, (const char*)str, strlen(str)) == -1)
        ESP_LOGI("uart", "send data failed");
}

void uart_read()
{
    int len;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM_0, (size_t*)&len));
    len = uart_read_bytes(UART_NUM_0, data_receive, len, 100);
    if (len)
    {
        data_receive[len] = '\0';
        ESP_LOGI("uart", "data recieve: %s", (char*)data_receive);
    }
}