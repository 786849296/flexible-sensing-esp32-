#include "DAC081S101.h"

void dac081s101_init()
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ull << DAC_SYNC) | (1ull << DAC_CLK) | (1ull << DAC_DIN),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    gpio_config(&io_config);
}

void dac(uint8_t data)
{
    gpio_set_level(DAC_SYNC, 1);
    esp_rom_delay_us(1);
    gpio_set_level(DAC_SYNC, 0);
    for (int i = 0; i < 16; i++)
    {
        gpio_set_level(DAC_CLK, 1);
        //dac模式选择
        if (i == 2 || i == 3)
            gpio_set_level(DAC_DIN, 0);
        else if (i < 12)
            gpio_set_level(DAC_DIN, (data >> (11 - i)) & 0x01);
        //esp_rom_delay_us(1);
        gpio_set_level(DAC_CLK, 0);
    }
    gpio_set_level(DAC_SYNC, 1);
}