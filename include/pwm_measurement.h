#ifndef PWM_MEASUREMENT_H
#define PWM_MEASUREMENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PWM_MEASUREMENT_OK = 0,
    PWM_MEASUREMENT_INVALID_ARGUMENT,
    PWM_MEASUREMENT_ZERO_PERIOD,
    PWM_MEASUREMENT_INVALID_PULSE_WIDTH,
    PWM_MEASUREMENT_OUT_OF_RANGE
} pwm_measurement_status_t;

typedef struct {
    uint32_t timer_clock_hz;
    uint32_t max_frequency_hz;
    uint32_t min_frequency_hz;
} pwm_measurement_config_t;

typedef struct {
    uint32_t period_ticks;
    uint32_t high_ticks;
    uint32_t frequency_hz;
    uint32_t period_ns;
    /* Duty in permille: 0 = 0%, 1000 = 100%. */
    uint16_t duty_permille;
} pwm_measurement_t;

/*
 * Convert rising/falling/rising capture timestamps to a measurement.
 * Unsigned arithmetic supports a single 32-bit counter wrap when the entire
 * elapsed period is less than 2^32 ticks. The middle timestamp must be the
 * falling edge within the measured period. Configure timer polarity/routing
 * so the supplied captures really have this order.
 */
pwm_measurement_status_t pwm_measurement_from_captures(
    uint32_t rising_1,
    uint32_t falling,
    uint32_t rising_2,
    const pwm_measurement_config_t *config,
    pwm_measurement_t *out);

/* Calculate rounded integer Hz from a nonzero number of timer ticks. */
pwm_measurement_status_t pwm_frequency_from_ticks(
    uint32_t period_ticks,
    uint32_t timer_clock_hz,
    uint32_t *frequency_hz);

#ifdef __cplusplus
}
#endif

#endif
