# STM32 Timer-Capture Frequency and Duty-Cycle Measurement Instrument

A reusable Embedded C measurement core for estimating PWM frequency, period, and duty cycle from hardware timer capture timestamps. The calculation library is platform-independent, while the STM32 HAL integration example shows where timer input-capture callbacks and UART output fit into real firmware.

> **Validation status:** Portable host tests and strict-warning builds are provided. Physical STM32 measurements have **not** been run as part of this repository work. The HAL adapter is an integration example that must be configured for the exact STM32 part and timer routing.

## Features

- Frequency, period, and duty-cycle calculations using integer arithmetic.
- Unsigned timestamp subtraction for a single 32-bit counter rollover.
- Configurable frequency limits and explicit error statuses.
- Portable C11 core independent of STM32 HAL.
- Host unit tests and CMake/CTest build.
- GitHub Actions CI with warnings treated as errors.
- Wiring, architecture, and bench-test documentation.

## Repository layout

```text
.
├── .github/workflows/ci.yml
├── docs/
│   ├── architecture.md
│   ├── test-plan.md
│   └── wiring.md
├── examples/
│   └── stm32_hal_capture_adapter.c
├── include/pwm_measurement.h
├── src/pwm_measurement.c
├── tests/test_pwm_measurement.c
├── CMakeLists.txt
└── LICENSE
```

## Build and test on a PC

Requirements: CMake 3.16+, a C11 compiler, and CTest.

```sh
cmake -S . -B build -DENABLE_STRICT_WARNINGS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The tests cover a typical 1 kHz / 25% PWM signal, a single counter rollover, zero-period and invalid-width rejection, range limits, and 0%/100% duty boundaries.

## API sketch

```c
#include "pwm_measurement.h"

static const pwm_measurement_config_t config = {
    .timer_clock_hz = 1000000U,
    .min_frequency_hz = 1U,
    .max_frequency_hz = 100000U
};

pwm_measurement_t result;
pwm_measurement_status_t status =
    pwm_measurement_from_captures(rising_1, falling, rising_2,
                                  &config, &result);

if (status == PWM_MEASUREMENT_OK) {
    /* result.frequency_hz, result.period_ns, result.duty_permille */
}
```

The captures must represent rising, falling, then next rising edges. `timer_clock_hz` is the effective timer counter clock after its prescaler. Duty is returned in permille (0–1000); divide by 10 for percent with one decimal digit.

## STM32 integration

1. Create an STM32CubeIDE project for the exact target board and MCU.
2. Use CubeMX to configure timer input-capture/PWM-input and UART.
3. Determine the effective timer counter clock from the clock tree and prescaler.
4. Add `include/pwm_measurement.h` and `src/pwm_measurement.c` to the firmware project.
5. Adapt `examples/stm32_hal_capture_adapter.c` to the selected timer's channel routing and interrupt names.
6. Keep interrupt callbacks short. Format and transmit UART output from foreground code.
7. Follow `docs/wiring.md` and execute the bench plan in `docs/test-plan.md`.

The example adapter is illustrative, not a complete CubeMX-generated firmware project. Timer polarity routing differs across STM32 variants, so verify the setup for the exact MCU.

## Design limits

- Timestamp deltas are 32-bit. A single wrap is handled only when the interval is less than 2^32 timer ticks.
- A 16-bit timer needs overflow extension or a measured-interval constraint.
- Frequency output is rounded to whole Hz; this is not a precision frequency counter.
- Hardware noise, input conditioning, clock accuracy, UART integration, and measured error need target-board verification.
- The core calculates values from timestamps; production firmware should add a no-signal timeout and a safe ISR-to-foreground data hand-off.

## Safety

The example assumes a 0–3.3 V digital input. Never connect 5 V or automotive battery signals directly to an MCU pin; use suitable protection and level shifting.

## Documentation

- [Architecture and formulas](docs/architecture.md)
- [Wiring and timer setup](docs/wiring.md)
- [Host tests and hardware bench plan](docs/test-plan.md)

## License

MIT. See [LICENSE](LICENSE).
