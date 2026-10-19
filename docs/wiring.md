# Wiring and target configuration

## Example bench setup

| Signal source | STM32 target |
|---|---|
| PWM generator ground | Board GND |
| PWM output (0–3.3 V logic) | Chosen timer input-capture pin |
| USB-UART adapter RX (optional) | STM32 UART TX |
| USB-UART adapter TX (optional) | STM32 UART RX |
| USB-UART ground | Board GND |

Use the exact timer input pin for the selected MCU package and alternate-function mapping. Pin numbers, timer channels, and IRQ names vary by STM32 model and package; confirm them in the datasheet and CubeMX.

## Electrical cautions

- Keep the input within the MCU pin absolute maximum and recommended voltage limits; this example assumes 0–3.3 V logic.
- Do not connect 5 V or automotive battery signals directly to an MCU input.
- Use suitable level shifting, protection, and conditioning when the source voltage or environment requires it.
- Ensure the signal generator and target share a reference ground unless an appropriate isolated interface is intentionally used.

## Timer setup checklist

- Select a timer with enough counter width and a supported input-capture route.
- Configure the effective counter clock in Hz after the prescaler.
- Configure rising and falling edge captures; PWM-input mode is often suitable where supported.
- Enable capture interrupts and clear pending flags before starting.
- Set measurement bounds that match the expected input range.
- For 16-bit timers, implement overflow extension or constrain the measurement interval so elapsed count is unambiguous.
