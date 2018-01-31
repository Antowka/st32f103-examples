//
// Created by anton on 26.01.18.
//

#include <ssd1306.h>
#include <dht11.h>
#include "lesson5.h"

dht11 dht;
extern TIM_HandleTypeDef htim3;

static void error(DHT11_RESULT res, int bit_pos) {
    ssd1306_SetCursor(10, 23);
    char msg[64];
    snprintf(msg, 64, "e=%d, p=%d", res, bit_pos);
    ssd1306_WriteString(msg, Font_11x18, White);
    ssd1306_UpdateScreen();
}

void lesson5() {

    ssd1306_Init();
    HAL_Delay(1000);
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    DHT11_RESULT r;

    dht11_config dht_config;
    dht_config.gpio_pin = PIN;
    dht_config.gpio_port = PORT;
    dht_config.timer = TIMER;
    dht_config.timer_channel = TIMER_CHANNEL;

    if ((r = dht11_init(&dht_config, &dht)) != DHT11_OK) error(r, -10);

    while (1) {

        if ((r = dht11_read_data(&dht)) != DHT11_OK) error(r, dht.bit_pos);

        char tmp[15];
        sprintf(tmp, "t = %d.%dC", dht.temp.number, dht.temp.fraction);
        ssd1306_SetCursor(10, 23);
        ssd1306_WriteString(tmp, Font_7x10, White);

        char hum[15];
        sprintf(hum, "h = %d.%d%c", dht.hum.number, dht.hum.fraction, '%');
        ssd1306_SetCursor(10, 43);
        ssd1306_WriteString(hum, Font_7x10, White);

        ssd1306_UpdateScreen();
        HAL_Delay(3000);
    }
}
