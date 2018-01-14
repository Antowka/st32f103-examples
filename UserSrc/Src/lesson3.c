//
// Created by anton on 14.01.18.
//

#include <stm32f1xx_hal_conf.h>
#include <stm32f1xx_hal.h>

static int statusWasChanged = 0;

void lesson3(void) {
    if (statusWasChanged) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); //Toggle LED
        statusWasChanged = 0;
    }
}

void IRQ_Handler_PB11(void) {
    statusWasChanged = 1;
}
