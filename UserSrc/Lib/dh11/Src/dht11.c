#include <sched.h>
#include <stm32f1xx_hal_gpio.h>
#include <dht11.h>
#include <cmsis_os.h>


/**
 * Sets up the pin as an output
 * @param	handle - a pointer to the DHT11 handle
 */
static void set_pin_out(dht11* handle) {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin  = handle->config.gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
#ifdef STM32F1
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
#else
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
#endif
    HAL_GPIO_Init(handle->config.gpio_port, &GPIO_InitStruct);
}

/**
 * Sets up the pin as an input
 * @param	handle - a pointer to the DHT11 handle
 */
static void set_pin_in(dht11* handle) {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin  = handle->config.gpio_pin;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
#ifdef STM32F1
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
#else
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = handle->config.gpio_alternate_function;
#endif
    HAL_GPIO_Init(handle->config.gpio_port, &GPIO_InitStruct);
}

DHT11_RESULT dht11_read_data(dht11* handle) {
    handle->hasNewTemperature = false;
    if (handle->state != DHT11_READY) return DHT11_ERROR;

    handle->state = DHT11_BUSY;

    // clear previous state
    handle->bit_pos            = -1;
    handle->last_val           = 0;
    handle->error_flags.parity = false;
    handle->error_flags.timing = false;
    for (int i = 0; i < 5; i++) handle->rx_buffer[i] = 0;

    // generate the start sequence
    set_pin_out(handle);
    HAL_GPIO_WritePin(handle->config.gpio_port, handle->config.gpio_pin, GPIO_PIN_SET);
    osDelay(10);
    HAL_GPIO_WritePin(handle->config.gpio_port, handle->config.gpio_pin, GPIO_PIN_RESET);
    osDelay(18);
    HAL_GPIO_WritePin(handle->config.gpio_port, handle->config.gpio_pin, GPIO_PIN_SET);
    set_pin_in(handle);

    __HAL_TIM_SET_COUNTER(handle->config.timer, 0);

    // enable the timer interrupts
    if (HAL_TIM_IC_Start_IT(handle->config.timer, handle->config.timer_channel) != HAL_OK) {
        return DHT11_ERROR;
    }

    uint32_t start_tick = xTaskGetTickCount();

    // wait for the state machine to finish
    while (handle->state != DHT11_FINISHED && xTaskGetTickCount() - start_tick < 1000) {
        if (handle->system.hasTimInterrupt) {
            handle->system.hasTimInterrupt = false;
            dht11_interrupt_handler(handle);
        }
    }

    // disable the timer interrupts
    if (HAL_TIM_IC_Stop_IT(handle->config.timer, handle->config.timer_channel) != HAL_OK) {
        return DHT11_ERROR;
    }

    if (handle->error_flags.timing) return DHT11_TIMING_ERROR;
    if (handle->error_flags.parity) return DHT11_CHECKSUM_ERROR;

    if (handle->state == DHT11_FINISHED) {
        handle->state = DHT11_READY;
        return DHT11_OK;
    }
    return DHT11_ERROR;
}

DHT11_RESULT dht11_init(dht11_config* config, dht11* handle) {
    handle->config = *config;
    return DHT11_OK;
}

DHT11_RESULT dht11_deinit(dht11* handle) { return DHT11_OK; }

/**
 * Writes a bit to the current bit position (bit_pos) in the rx_buffer
 * @param handle DHT11 handle
 * @param bit    bit to store
 */
static void write_bit(dht11* handle, bool bit) {
    uint8_t byten = handle->bit_pos / 8;
    uint8_t bitn  = 7 - handle->bit_pos % 8;
    if (bit) {
        handle->rx_buffer[byten] |= 1 << bitn;
    } else {
        handle->rx_buffer[byten] &= ~(1 << bitn);
    }
}

/**
 * Converts two bytes from rx_buffer into a float according to the format
 * specified in the datasheet
 * @param  dht_data pointer to the two bytes to be converted
 * @return          result of the conversion
 */
static paramsContainer get_float(const uint8_t* dht_data) {

    paramsContainer value;
    value.number = dht_data[0];
    value.fraction = dht_data[1];

    return value;
}

/**
 * Perform last steps of reception like parity check, etc.
 * @param handle DHT11 handle
 */
static void finish_rx(dht11* handle) {
    uint8_t sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += handle->rx_buffer[i];
    }

    if (sum == handle->rx_buffer[4]) { // checksums match
        handle->hum  = get_float(&handle->rx_buffer[0]);
        handle->temp = get_float(&handle->rx_buffer[2]);
    } else {
        handle->error_flags.parity = true;
    }
    handle->state = DHT11_FINISHED;
    handle->hasNewTemperature = true;
}

#define BETWEEN(a, b) (dt >= (a) && dt <= (b))

#define TIMING_ERROR()                               \
    do {                                             \
        handle->error_flags.timing = true;           \
        handle->state              = DHT11_FINISHED; \
    } while (0)

void dht11_interrupt_handler(dht11* handle) {
    uint32_t val = HAL_TIM_ReadCapturedValue(handle->config.timer, handle->config.timer_channel);

    uint32_t dt = val - handle->last_val;

    if (handle->bit_pos == -1) { // end of the start bit
        if (BETWEEN(120, 200)) { // [20 to 40]us + 80us + 80us - CPU time
            handle->bit_pos++;
        } else if (BETWEEN(0, 40)) { // [20-40]us - CPU time
            // fast CPU, caught beginning of the start bit
            // do nothing
        } else {
            TIMING_ERROR();
        }
    } else {                    // data bits
        if (BETWEEN(70, 100)) { // zero: 50us + [26-28]us
            write_bit(handle, false);
            handle->bit_pos++;
        } else if (BETWEEN(110, 150)) { // one: 50us + 70us
            write_bit(handle, true);
            handle->bit_pos++;
        } else { // invalid
            TIMING_ERROR();
        }
    }

    if (handle->bit_pos == 40) { // finished
        finish_rx(handle);
    }

    handle->last_val = val;
}