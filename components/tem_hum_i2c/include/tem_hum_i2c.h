#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "EXAMPLE";
#define EXAMPLE_ADC_OUTPUT_TYPE ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define EXAMPLE_ADC_GET_CHANNEL(p_data) ((p_data)->type2.channel)
#define EXAMPLE_ADC_GET_DATA(p_data) ((p_data)->type2.data)
#define I2C_MASTER_SCL_IO_1 8                                        /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO_1 18                                       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM_1 1                                           /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ_1 2000 /*!< I2C master clock frequency */ // 200000
#define I2C_MASTER_TX_BUF_DISABLE 0                                  /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                                  /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000
#define ATH20_RED_ADDR 0x71  /*!< Register addresses of the "who am I" register */
#define ATH20_INIT_ADDR 0xBE /*!< Register addresses of the power managment register */
#define ATH20_AC_ADDR 0xAC
#define ATH20_SENSOR_ADDR 0x38 // 温湿度

void i2c_master_init_1(void);
void register_read(uint8_t SENSOR_ADDR, uint8_t reg_addr, uint8_t *data, size_t len); // 读寄存器
void register_write_bytes(uint8_t SENSOR_ADDR, uint8_t write_buf[], size_t len); // 写寄存器
void AHT20_Start_Init(void);
void AHT20_Read_CTdata(uint32_t *ct);
