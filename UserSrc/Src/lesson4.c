//
// Created by anton on 14.01.18.
//

#include <stm32f1xx_hal_conf.h>
#include <ssd1306.h>

int counter = 0;

void lesson4(void) {

    ssd1306_Init();
    HAL_Delay(1000);
    ssd1306_Fill(Black);

    while (1) {

        ssd1306_SetCursor(10, 23);
        char str[2] = "";
        sprintf(str, "Timer: %d", counter);
        ssd1306_WriteString(str, Font_11x18, White);
        ssd1306_UpdateScreen();
        HAL_Delay(500);
    }
}


void timHandler(TIM_HandleTypeDef *htim2) {
    counter++;
    if (counter > 60) {
        counter = 0;
    }
}

