//
// Created by anton on 14.01.18.
//

#include <stm32f1xx_hal_conf.h>
#include <stm32f1xx_hal.h>

void lesson3(void) {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); //Toggle LED
    HAL_Delay(100); //Delay 1 Seconds
}
