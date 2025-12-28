# ESPHome Sonoff L1 (UART) Light Platform

This repository provides an **ESPHome external component** that adds a `light:` platform for the **Sonoff L1** LED strip controller.

It uses the Sonoff L1 AT-command interface over the ESP's hardware `Serial` at **19200 baud**.

ESPHome external component syntax and defaults are documented here. citeturn19search3

## Install

### Option A, use directly from GitHub (recommended)

```yaml
external_components:
  - source: github://YOUR_GITHUB_USER/esphome-sonoff-l1@main
    components: [sonoff_l1]
```

ESPHome expects components in `components/` by default. citeturn19search3

### Option B, copy it locally

Copy `components/sonoff_l1/` into your ESPHome config folder, for example:

`/config/esphome/components/sonoff_l1/`

Then in YAML:

```yaml
external_components:
  - source:
      type: local
      path: ./components
    components: [sonoff_l1]
```

## Important, serial logging must be disabled

Add:

```yaml
logger:
  baud_rate: 0
```

ESPHome documents that `baud_rate: 0` disables UART logging. citeturn20search0

## “Effects: None” behavior

ESPHome reports `LightState::get_effect_name()` as `"None"` when no effect is active. citeturn21search0  
This component detects that case and resets the Sonoff L1 mode back to static color mode while keeping your current color and brightness.

## Examples

See `examples/`.
