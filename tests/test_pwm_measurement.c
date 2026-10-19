#include "pwm_measurement.h"

#include <assert.h>
#include <stdio.h>

static const pwm_measurement_config_t cfg = {
    .timer_clock_hz = 1000000U,
    .min_frequency_hz = 1U,
    .max_frequency_hz = 100000U
};

static void test_typical_pwm(void)
{
    pwm_measurement_t m = {0};
    /* 1 kHz, 25% duty, timer running at 1 MHz. */
    assert(pwm_measurement_from_captures(100U, 350U, 1100U, &cfg, &m) ==
           PWM_MEASUREMENT_OK);
    assert(m.period_ticks == 1000U);
    assert(m.high_ticks == 250U);
    assert(m.frequency_hz == 1000U);
    assert(m.period_ns == 1000000U);
    assert(m.duty_permille == 250U);
}

static void test_timer_wrap(void)
{
    pwm_measurement_t m = {0};
    assert(pwm_measurement_from_captures(0xFFFFFF00U, 0xFFFFFFFAU,
                                         0x000002E8U, &cfg, &m) ==
           PWM_MEASUREMENT_OK);
    assert(m.period_ticks == 1000U);
    assert(m.high_ticks == 250U);
    assert(m.frequency_hz == 1000U);
}

static void test_zero_period(void)
{
    pwm_measurement_t m = {0};
    assert(pwm_measurement_from_captures(10U, 20U, 10U, &cfg, &m) ==
           PWM_MEASUREMENT_ZERO_PERIOD);
}

static void test_invalid_pulse_width(void)
{
    pwm_measurement_t m = {0};
    assert(pwm_measurement_from_captures(100U, 1200U, 1100U, &cfg, &m) ==
           PWM_MEASUREMENT_INVALID_PULSE_WIDTH);
}

static void test_out_of_range(void)
{
    pwm_measurement_t m = {0};
    pwm_measurement_config_t narrow = cfg;
    narrow.max_frequency_hz = 500U;
    assert(pwm_measurement_from_captures(0U, 250U, 1000U, &narrow, &m) ==
           PWM_MEASUREMENT_OUT_OF_RANGE);
}

static void test_zero_and_full_duty(void)
{
    pwm_measurement_t m = {0};
    assert(pwm_measurement_from_captures(0U, 0U, 1000U, &cfg, &m) ==
           PWM_MEASUREMENT_OK);
    assert(m.duty_permille == 0U);
    assert(pwm_measurement_from_captures(0U, 1000U, 1000U, &cfg, &m) ==
           PWM_MEASUREMENT_OK);
    assert(m.duty_permille == 1000U);
}

static void test_frequency_api_validation(void)
{
    uint32_t frequency = 0U;
    assert(pwm_frequency_from_ticks(0U, 1000000U, &frequency) ==
           PWM_MEASUREMENT_ZERO_PERIOD);
    assert(pwm_frequency_from_ticks(10U, 0U, &frequency) ==
           PWM_MEASUREMENT_INVALID_ARGUMENT);
    assert(pwm_frequency_from_ticks(1000U, 1000000U, &frequency) ==
           PWM_MEASUREMENT_OK);
    assert(frequency == 1000U);
}

int main(void)
{
    test_typical_pwm();
    test_timer_wrap();
    test_zero_period();
    test_invalid_pulse_width();
    test_out_of_range();
    test_zero_and_full_duty();
    test_frequency_api_validation();
    puts("All PWM measurement tests passed.");
    return 0;
}
