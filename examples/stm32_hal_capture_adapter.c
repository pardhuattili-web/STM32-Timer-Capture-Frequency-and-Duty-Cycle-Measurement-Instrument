/*
 * STM32Cube HAL integration sketch — adapt before use.
 *
 * Configure the chosen timer for rising/falling/rising captures of one input,
 * using the timer's supported PWM-input routing or polarity-switch strategy.
 * Channel routing differs by MCU. This example intentionally is not a complete
 * CubeMX-generated project and must be adapted to the chosen target.
 */
#include "main.h"
#include "pwm_measurement.h"
#include <stdio.h>
#include <stddef.h>

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;

static volatile uint32_t rise_1;
static volatile uint32_t fall_edge;
static volatile uint32_t rise_2;
static volatile uint8_t phase;
static volatile uint8_t ready;

static const pwm_measurement_config_t config = {
    .timer_clock_hz = 1000000U, /* Replace with actual counter clock after PSC. */
    .min_frequency_hz = 1U,
    .max_frequency_hz = 100000U
};

/*
 * In the CubeMX configuration, map CH1 to rising captures and CH2 to falling
 * captures for the same input if supported. Ensure HAL active-channel semantics
 * match the selected timer mode before using these comparisons.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM2) {
        return;
    }

    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        uint32_t value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        if (phase == 0U) {
            rise_1 = value;
            phase = 1U;
        } else if (phase == 2U) {
            rise_2 = value;
            phase = 0U;
            ready = 1U;
        }
    } else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 && phase == 1U) {
        fall_edge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
        phase = 2U;
    }
}

/*
 * Call regularly from the foreground main loop, never directly from an ISR.
 * For production code, protect the multi-field snapshot using a critical
 * section/sequence counter or move capture tuples through a queue; volatile
 * does not make the tuple atomic. Add timeout/no-signal reporting as required.
 */
void measurement_service(void)
{
    pwm_measurement_t m;
    char line[112];
    int n;

    if (ready == 0U) {
        return;
    }
    ready = 0U;

    if (pwm_measurement_from_captures(rise_1, fall_edge, rise_2,
                                      &config, &m) != PWM_MEASUREMENT_OK) {
        return;
    }

    n = snprintf(line, sizeof line, "freq=%lu Hz period=%lu ns duty=%u.%u%%\r\n",
                 (unsigned long)m.frequency_hz,
                 (unsigned long)m.period_ns,
                 (unsigned)(m.duty_permille / 10U),
                 (unsigned)(m.duty_permille % 10U));
    if (n > 0 && (size_t)n < sizeof line) {
        (void)HAL_UART_Transmit(&huart1, (uint8_t *)line, (uint16_t)n, 50U);
    }
}

/* In main after generated peripheral initialization:
 *   HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
 *   HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
 * and call measurement_service() in the foreground loop.
 */
