#pragma once

#include "driver/uart.h"
#include <string.h>
#include "esp_log.h"

#define UART_BUFFER_SIZE 1024 * 2
extern uint8_t data_receive[128];

void uart_init();
void uart_write(char* str);
//如果有串口接收功能需求需进行修改，目前只能轮询的方式读取数据
void uart_read();