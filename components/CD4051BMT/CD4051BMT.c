#include "CD4051BMT.h"

uint8_t cd4051bmt_channel = 0;

void cd4051bmt_init()
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ull << 3) | (1ull << 9) | (1ull << 46) | (1ull << 45),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    gpio_config(&io_config);
}

void cd4051bmt_channel_set(uint8_t channel)
{
    gpio_num_t gpio_pin[3] = {GPIO_NUM_9, GPIO_NUM_46, GPIO_NUM_3};
    for (int i = 0; i < 3; i++)
    {
        if (channel & 0x01)
            gpio_set_level(gpio_pin[i], 1);
        else
            gpio_set_level(gpio_pin[i], 0);
        channel >>= 1;
        //ESP_LOGI(TAG, "gpio_pin is %d, it level is %d", gpio_pin[i], gpio_get_level(gpio_pin[i]));
    }
}