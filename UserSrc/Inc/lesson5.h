//
// Created by anton on 26.01.18.
//

#include "stm32f1xx_hal.h"
#include "dht11.h"

#ifndef BLINK_LESSON5_H
#define BLINK_LESSON5_H

extern TIM_HandleTypeDef  htim3;
extern osSemaphoreId sendTelemetryToUsbSemaphoreHandle;

#define PIN GPIO_PIN_6
#define PORT GPIOA
#define TIMER &htim3
#define TIMER_CHANNEL TIM_CHANNEL_1

void lesson5();
void DisplayOutputTask(void const *argument);
void SendTelemetryToUsb(void const *argument);
void TemperatureTask(void const *argument);

#endif //BLINK_LESSON5_H
