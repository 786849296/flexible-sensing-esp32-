#include <stdio.h>
#include "tem_hum_i2c.h"

void i2c_master_init_1(void) // TVOC和温湿度和光照
{
    int i2c_master_port = I2C_MASTER_NUM_1;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO_1,
        .scl_io_num = I2C_MASTER_SCL_IO_1,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ_1,
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void register_read(uint8_t SENSOR_ADDR, uint8_t reg_addr, uint8_t *data, size_t len) // 读寄存器
{    
    i2c_master_write_read_device(I2C_MASTER_NUM_1, SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}
// void MY_register_write_bytes(uint8_t SENSOR_ADDR, uint8_t write_buf[], size_t len) // 写寄存器
//     i2c_master_write_to_device(I2C_MASTER_NUM_1, SENSOR_ADDR, write_buf, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

void register_write_bytes(uint8_t SENSOR_ADDR, uint8_t write_buf[], size_t len) // 写寄存器
{
    i2c_master_write_to_device(I2C_MASTER_NUM_1, SENSOR_ADDR, write_buf, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}
void AHT20_Start_Init(void) // 温湿度初始化
{
    uint8_t write_buf[3] = {0xBE, 0x08, 0x00};
    register_write_bytes(ATH20_SENSOR_ADDR, write_buf, sizeof(write_buf));
}

void AHT20_Read_CTdata(uint32_t *ct) // 读取温湿度
{
    uint8_t data_TEST[6];
    uint32_t RetuData = 0;
    uint8_t write_buf[3] = {0xAC, 0x33, 0x00};
    // mpu9250_register_write_byte(ATH20_SENSOR_ADDR,0xAC,0x33,0X00);
    register_write_bytes(ATH20_SENSOR_ADDR, write_buf, sizeof(write_buf));
    vTaskDelay(800 / portTICK_PERIOD_MS);
    register_read(ATH20_SENSOR_ADDR, ATH20_RED_ADDR, data_TEST, 6);
    //ESP_LOGI("AHT20", "%02x%02x%02x%02x%02x%02x", data_TEST[0], data_TEST[1], data_TEST[2], data_TEST[3], data_TEST[4], data_TEST[5]);
    /*湿度*/
    RetuData = (data_TEST[3] & 0xF0) >> 4;
    RetuData |= data_TEST[2] << 4;
    RetuData |= data_TEST[1] << 12;
    ct[0] = RetuData;

    /*温度*/
    RetuData = 0;
    RetuData = data_TEST[5];
    RetuData |= data_TEST[4] << 8;
    RetuData |= (data_TEST[3] & 0x0F) << 16;
    ct[1] = RetuData;

    ct[2] = ct[0] * 100 * 10 / 1024 / 1024;       // 计算得到湿度值（放大了10倍）
    ct[3] = ct[1] * 200 * 10 / 1024 / 1024 - 500; // 计算得到温度值（放大了10倍）
}
