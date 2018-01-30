//
// Created by anton on 26.01.18.
//

#include <ssd1306.h>
#include <dht11.h>
#include "lesson5.h"

dht11 dht;
extern TIM_HandleTypeDef htim2;

static void error(void) {
    ssd1306_SetCursor(10, 23);
    ssd1306_WriteString("Error", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void lesson5() {

    ssd1306_Init();
    HAL_Delay(1000);
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    dht11_config dht_config;
    dht_config.gpio_pin = GPIO_PIN_0;
    dht_config.gpio_port = GPIOA;
    dht_config.timer = &htim2;
    dht_config.timer_channel = TIM_CHANNEL_1;

    if (dht11_init(&dht_config, &dht) != DHT11_OK) error();

    while (1) {

        if (dht11_read_data(&dht) != DHT11_OK) error();
        char str[5] = "";
        sprintf(str, "%.2f", dht.temp);
        ssd1306_SetCursor(10, 23);
        ssd1306_WriteString(str, Font_11x18, White);
        ssd1306_UpdateScreen();
        HAL_Delay(1000);
    }
}
