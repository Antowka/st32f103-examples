//
// Created by anton on 14.01.18.
//

#include <stm32f1xx_hal_conf.h>
#include <stm32f1xx_hal.h>
#include <ssd1306.h>

static int statusWasChanged = 0;

void lesson4(void) {

    ssd1306_Init();
    HAL_Delay(1000);
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    ssd1306_SetCursor(10, 23);
    ssd1306_WriteString("Anton Nik", Font_11x18, White);

    int x = 10;
    int y = 51;
    SSD1306_COLOR color = White;
    int direction = 0;

    while (1) {
        ssd1306_DrawPixel(x, y, color);
        ssd1306_UpdateScreen();
        HAL_Delay(50);

        if (!direction) {
            x++;
        } else {
            x--;
        }

        if (x > 100) {
            direction = !direction;
            color = color == White ? Black : White;
        } else if (x < 10) {
            direction = !direction;
            color = color == White ? Black : White;
        }
    }
}
