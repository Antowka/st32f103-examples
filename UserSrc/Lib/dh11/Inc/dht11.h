#ifndef DHT11_H
#define DHT11_H

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif

#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif

#include <stdbool.h>

/**
 * Possible return values of the functions
 */
typedef enum {
    /**
     * Function call was successful
     */
    DHT11_OK,
    /**
     * Function call was unsuccessful
     */
    DHT11_ERROR,
    /**
     * Timing error was detected
     */
    DHT11_TIMING_ERROR,
    /**
     * Checksum error was detected
     */
    DHT11_CHECKSUM_ERROR
} DHT11_RESULT;

/**
 * Describes the state of the sensor
 */
typedef enum {
    /**
     * The sensor is ready for next measurement
     */
    DHT11_READY,
    /**
     * The sensor is currently receiving data
     */
    DHT11_BUSY,
    /**
     * The sensor finished receiving data, data are awaiting processing
     */
    DHT11_FINISHED
} DHT11_STATE;

/**
 * This structure is for initializing the sensor handle
 */
typedef struct {
    /**
     * GPIO pin connected to the DHT11 sensor
     */
    uint16_t gpio_pin;
    /**
     * GPIO port associated with the `gpio_pin`
     */
    GPIO_TypeDef* gpio_port;
    /**
     * Calibrated timer used for time measurements
     */
    TIM_HandleTypeDef* timer;
    /**
     * Input capture channel of the timer to be used for time measurements
     */
    uint32_t timer_channel;

#ifdef STM32F4
    /**
     * Alternate function of the GPIO pin when it is used by timer input channel
     */
    uint32_t gpio_alternate_function;
#endif
} dht11_config;

/**
 * This structure is the sensor handle
 */
typedef struct {
    /**
     * Buffer for receiving data (40bits = 5bytes)
     */
    uint8_t rx_buffer[5];

    /**
     * Current position of the bit being received (from -2 to 40)
     */
    int8_t bit_pos;

    /**
     * Timestamp of the last input capture. Used for calculating duration
     * between input captures and therefore the pulse length.
     */
    uint16_t last_val;

    /**
     * Temperature reading
     */
    float temp;

    /**
     * Humidity reading
     */
    float hum;

    /**
     * Current state of the sensor
     */
    DHT11_STATE state;

    /**
     * Error flags
     */
    struct {
        /**
         * Set when the checksum is incorrent
         */
        bool parity : 1;

        /**
         * Set when the timing of the pulses coming from the sensor is invalid
         */
        bool timing : 1;
    } error_flags;

    /**
     * Configuration of the sensor
     */
    dht11_config config;
} dht11;

/**
 * Initializes the DHT11 handle using the provided configuration
 * @param   config - a pointer to the initialization structure
 * @param	handle - a pointer to the DHT11 handle you want to initialize
 * @return	whether the function was successful or not
 */
DHT11_RESULT dht11_init(dht11_config* config, dht11* handle);

/**
 * Deinitializes the DHT11 communication
 * @param	handle - a pointer to the DHT11 handle
 * @return	whether the function was successful or not
 */
DHT11_RESULT dht11_deinit(dht11* handle);

/**
 * Reads the current temperature and humidity from the sensor
 * @param	handle - a pointer to the DHT11 handle
 * @return	whether the function was successful or not
 */
DHT11_RESULT dht11_read_data(dht11* handle);

/**
 * Handles the timer interrupt
 * @param	handle - a pointer to the DHT11 handle
 */
void dht11_interrupt_handler(dht11* handle);

#endif /* DHT11_H */
