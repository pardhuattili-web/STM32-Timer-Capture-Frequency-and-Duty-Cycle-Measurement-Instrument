# Test plan

## Automated host tests

```sh
cmake -S . -B build -DENABLE_STRICT_WARNINGS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The current test suite covers a 1 kHz signal at 25% duty with a 1 MHz timer clock, a single 32-bit counter rollover, zero-period rejection, pulse width greater than the period, configured frequency limits, 0%/100% duty boundaries, and API argument validation.

## Hardware bench plan (not yet executed)

1. Generate a known PWM output from a second timer channel or external signal generator.
2. Join grounds and connect the output to the timer input through a suitable 3.3 V logic-level signal.
3. Compare 100 Hz, 1 kHz, 10 kHz, and 50 kHz readings at 10%, 50%, and 90% duty cycle.
4. Compare the result against a frequency counter or oscilloscope.
5. Test near the configured minimum/maximum range, then test noise and edge jitter.
6. Record board, timer, prescaler, input, and measured error below.

## Results log

| Date | Board / MCU | Input frequency | Input duty | Measured frequency | Measured duty | Status |
|---|---|---:|---:|---:|---:|---|
| Not run | Not yet hardware-tested | — | — | — | — | Pending bench verification |

Do not mark hardware validation complete until physical readings have been captured and recorded.
