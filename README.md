# ESPHome Sonoff L1 (UART) Light Platform

This repository provides an ESPHome external component that adds a `light:` platform for the Sonoff L1 LED strip controller.

The Sonoff L1 controller typically contains an ESP8285 (ESP8266 family) plus a second microcontroller that drives the LEDs. The LED controller accepts AT style commands over a UART connection at **19200 baud**. This component talks to that UART using the ESP8266 hardware serial port.

## Safety first

The Sonoff L1 controller is normally powered from mains. Always unplug it before opening the enclosure. If you are not comfortable working around mains powered devices, get help from someone who is.

## What you need

- A Sonoff L1 controller
- A 3.3 V USB to TTL serial adapter (FTDI, CP2102, CH340, etc). Do not use 5 V.
- A few jumper wires, plus a way to connect to the Sonoff L1 programming pads (soldering, pogo pins, or clips)
- Home Assistant with the ESPHome add-on installed, or ESPHome installed on your computer

## Step 1. Flash ESPHome onto the Sonoff L1

### 1) Open the controller and find the programming pads

Most Sonoff L1 controllers expose pads for **3V3, GND, RX, TX**. Some boards also expose **GPIO0** or a button used to enter bootloader mode.

### 2) Wire your USB to TTL adapter

- Adapter **GND** to device **GND**
- Adapter **TX** to device **RX**
- Adapter **RX** to device **TX**
- Adapter **3V3** to device **3V3**

Power the device from the adapter **3V3** while flashing. Keep it disconnected from mains.

### 3) Enter flashing mode

For ESP8266/ESP8285, flashing mode usually requires **GPIO0 held low** during power on:

- Hold **GPIO0** to **GND**
- Apply 3V3 power
- Release GPIO0 after a second

If your board has a button, it may do the same thing.

### 4) Flash with ESPHome

**Home Assistant route (recommended):**

1. Install the ESPHome add-on.
2. Open ESPHome, click **New Device**, follow the prompts.
3. When it is time to install, choose **Manual download** to download the firmware `.bin`.
4. Use a browser flasher like **ESPHome Web** to flash the downloaded `.bin` via your USB to TTL adapter.

**Command line route (alternative):**

- Create a YAML file (see examples below)
- Run `esphome run <your-yaml>.yaml` and select the serial port when prompted

Once flashed and booted on Wi-Fi, Home Assistant will discover the device via the ESPHome integration.

## Step 2. Use this component in your YAML

This component must be pulled from GitHub via `external_components`.

### GitHub install (default)

Use the GitHub based example:

- `examples/sonoff-l1-device-github.yaml`

That file is ready to paste into ESPHome. It uses `substitutions` for device name and friendly name, and uses `YOURPASSWORDHERE` placeholders for Wi-Fi, OTA, and API encryption.

### Local install (optional)

If you prefer to keep the component in your ESPHome config instead of pulling from GitHub:

1. Copy `components/sonoff_l1/` into your ESPHome config folder as:

   `/config/esphome/components/sonoff_l1/`

2. Use the local example:

- `examples/sonoff-l1-device-local.yaml`

## Important. Disable UART logging

The Sonoff L1 uses the ESP8266 hardware UART for LED control. You must disable serial logging:

```yaml
logger:
  baud_rate: 0
```

## Common troubleshooting

- If the device does not respond to color changes, confirm `logger: baud_rate: 0` is set.
- If flashing fails, double check you are using 3.3 V (not 5 V), and that RX/TX are crossed.
- If the device will not enter flashing mode, confirm GPIO0 is held low at power on.

## License

MIT, see `LICENSE`.
