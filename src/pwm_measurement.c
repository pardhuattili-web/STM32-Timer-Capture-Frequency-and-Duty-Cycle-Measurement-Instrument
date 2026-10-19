#include "pwm_measurement.h"

#include <limits.h>
#include <stddef.h>

pwm_measurement_status_t pwm_frequency_from_ticks(
    uint32_t period_ticks,
    uint32_t timer_clock_hz,
    uint32_t *frequency_hz)
{
    if (frequency_hz == NULL || timer_clock_hz == 0U) {
        return PWM_MEASUREMENT_INVALID_ARGUMENT;
    }
    if (period_ticks == 0U) {
        return PWM_MEASUREMENT_ZERO_PERIOD;
    }

    /* Rounded integer division without a floating-point dependency. */
    *frequency_hz = (uint32_t)(((uint64_t)timer_clock_hz +
                                ((uint64_t)period_ticks / 2U)) /
                               (uint64_t)period_ticks);
    return PWM_MEASUREMENT_OK;
}

pwm_measurement_status_t pwm_measurement_from_captures(
    uint32_t rising_1,
    uint32_t falling,
    uint32_t rising_2,
    const pwm_measurement_config_t *config,
    pwm_measurement_t *out)
{
    uint32_t period;
    uint32_t high;
    uint32_t frequency;

    if (config == NULL || out == NULL || config->timer_clock_hz == 0U ||
        config->min_frequency_hz > config->max_frequency_hz) {
        return PWM_MEASUREMENT_INVALID_ARGUMENT;
    }

    /* Unsigned arithmetic intentionally handles a single 32-bit wrap. */
    high = (uint32_t)(falling - rising_1);
    period = (uint32_t)(rising_2 - rising_1);

    if (period == 0U) {
        return PWM_MEASUREMENT_ZERO_PERIOD;
    }
    if (high > period) {
        return PWM_MEASUREMENT_INVALID_PULSE_WIDTH;
    }

    if (pwm_frequency_from_ticks(period, config->timer_clock_hz, &frequency) !=
        PWM_MEASUREMENT_OK) {
        return PWM_MEASUREMENT_INVALID_ARGUMENT;
    }
    if (frequency < config->min_frequency_hz ||
        frequency > config->max_frequency_hz) {
        return PWM_MEASUREMENT_OUT_OF_RANGE;
    }

    out->period_ticks = period;
    out->high_ticks = high;
    out->frequency_hz = frequency;

    /* Round nanoseconds and duty in permille; use 64-bit intermediates. */
    out->period_ns = (uint32_t)((((uint64_t)period * 1000000000ULL) +
                                  config->timer_clock_hz / 2U) /
                                 config->timer_clock_hz);
    out->duty_permille = (uint16_t)((((uint64_t)high * 1000ULL) +
                                     period / 2U) /
                                    period);
    return PWM_MEASUREMENT_OK;
}
