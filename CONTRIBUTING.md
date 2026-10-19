# Contributing

Thanks for helping improve this project.

## Before opening a change

1. Keep the portable measurement core free of STM32 HAL dependencies.
2. Add regression tests for changes to calculation or error handling.
3. Run the host checks:

   ```sh
   cmake -S . -B build -DENABLE_STRICT_WARNINGS=ON
   cmake --build build --parallel
   ctest --test-dir build --output-on-failure
   ```

4. For hardware changes, document the exact MCU, timer instance, clock source, prescaler, channel routing, and test equipment.
5. Do not describe simulator or host-test results as physical-board verification.

## Style

- Use C11 and fixed-width integer types for calculation boundaries.
- Prefer explicit status codes to ambiguous sentinel values.
- Keep interrupt handlers bounded and free of blocking output or formatting.
- Document timer-clock assumptions and counter-width limitations.

## Pull requests

Describe the motivation, implementation, tests run, and any unverified hardware assumptions. Include measured results only when you have actually captured them.
