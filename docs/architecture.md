# Architecture and design notes

## Data path

```text
PWM / digital input -> STM32 timer capture -> minimal HAL callback -> foreground measurement service -> portable C core -> UART
```

The portable library is independent of STM32 HAL and uses integer arithmetic so it can be unit-tested with a native host compiler. The example adapter is an integration template; it must be adapted to the selected STM32 timer routing and CubeMX configuration.

## Capture math

For rising-edge timestamps R1, falling-edge timestamp F, and next rising-edge timestamp R2, using unsigned 32-bit subtraction:

- Period ticks: P = R2 - R1
- High ticks: H = F - R1
- Frequency: f = round(timer_clock_hz / P)
- Duty permille: D = round(1000 * H / P)
- Period nanoseconds: Tns = round(1e9 * P / timer_clock_hz)

Unsigned subtraction handles a single 32-bit timer wrap when the actual elapsed interval fits within the 32-bit range. It cannot distinguish an interval spanning more than one wrap. A 16-bit timer needs explicit overflow extension or carefully designed capture-count bookkeeping.

## Concurrency and timer clock

Capture interrupt handlers should capture timestamps and update a minimal state machine only. Formatting strings, blocking UART writes, and floating-point calculations should remain outside interrupt context. On Cortex-M targets, protect a multi-field snapshot against concurrent updates using a brief critical section, sequence counter, or queue; volatile alone does not make a multi-field snapshot atomic.

`timer_clock_hz` means the timer counter clock after the timer prescaler, not necessarily the APB peripheral clock. Timer clocking depends on APB prescaler settings and family-specific clock-tree rules. Verify the reference manual and CubeMX-generated clock configuration.

## Known limits

- Portable API models rising/falling/rising captures from one input.
- Capture interval must be less than 2^32 ticks.
- Integer frequency output is rounded to Hz.
- The HAL adapter is illustrative; channel polarity and routing differ across STM32 variants.
- Physical measurements, low-frequency overflow extension, signal conditioning, and electrical noise behavior remain board-level validation tasks.
