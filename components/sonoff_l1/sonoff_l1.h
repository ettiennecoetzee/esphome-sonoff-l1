#pragma once

#include <Arduino.h>
#include <cmath>
#include <pgmspace.h>
#include <string>

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"

namespace esphome {
namespace sonoff_l1 {

/**
 * Sonoff L1 UART-controlled light output.
 *
 * The Sonoff L1 has a second microcontroller that implements an AT-command interface at 19200 baud.
 * This class drives that interface over the ESP's hardware Serial.
 */
class SonoffL1Light : public Component, public light::LightOutput {
 public:
  // Mode numbers exactly as in the original Sonoff L1 AT protocol.
  static constexpr int SONOFF_L1_MODE_COLORFUL          = 1;   // [Color key] Colorful (static color)
  static constexpr int SONOFF_L1_MODE_COLORFUL_GRADIENT = 2;   // [SMOOTH] Colorful Gradient
  static constexpr int SONOFF_L1_MODE_COLORFUL_BREATH   = 3;   // [FADE] Colorful Breath
  static constexpr int SONOFF_L1_MODE_DIY_GRADIENT      = 4;   // DIY Gradient (fade in/out) [Speed 1-100, color]
  static constexpr int SONOFF_L1_MODE_DIY_PULSE         = 5;   // DIY Pulse  (faster fade in/out) [Speed 1-100, color]
  static constexpr int SONOFF_L1_MODE_DIY_BREATH        = 6;   // DIY Breath (toggle on/off) [Speed 1-100, color]
  static constexpr int SONOFF_L1_MODE_DIY_STROBE        = 7;   // DIY Strobe (faster toggle on/off) [Speed 1-100, color]
  static constexpr int SONOFF_L1_MODE_RGB_GRADIENT      = 8;   // RGB Gradient
  static constexpr int SONOFF_L1_MODE_RGB_PULSE         = 9;   // [STROBE] RGB Pulse
  static constexpr int SONOFF_L1_MODE_RGB_BREATH        = 10;  // RGB Breath
  static constexpr int SONOFF_L1_MODE_RGB_STROBE        = 11;  // [FLASH] RGB strobe
  static constexpr int SONOFF_L1_MODE_SYNC_TO_MUSIC     = 12;  // Sync to music [Speed 1-100, sensitivity 1-10]

  void setup() override {
    // Sonoff L1 AT interface speed
    Serial.begin(19200);
  }

  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::RGB});
    return traits;
  }

  void write_state(light::LightState *state) override {
    // If HA switches effect to "None", revert to static mode while keeping color/brightness.
    const std::string effect = state->get_effect_name();
    if (effect == "None") {
      mode_ = SONOFF_L1_MODE_COLORFUL;
    }

    // Current RGB (0.0-1.0)
    float red = 0.0f, green = 0.0f, blue = 0.0f;
    state->current_values_as_rgb(&red, &green, &blue);

    // Convert to 0-255
    const int redValue = (int) floorf(red * 255.0f);
    const int greenValue = (int) floorf(green * 255.0f);
    const int blueValue = (int) floorf(blue * 255.0f);

    // On/off
    bool ledState = false;
    state->current_values_as_binary(&ledState);

    // Brightness 0-100
    float brightnessPercent = 0.0f;
    state->current_values_as_brightness(&brightnessPercent);
    const int brightness = (int) floorf(brightnessPercent * 100.0f);

    const uint32_t ms = millis();
    char buffer[160];

    // In sync mode, include sensitive/speed so we don't accidentally reset them
    if (mode_ == SONOFF_L1_MODE_SYNC_TO_MUSIC) {
      snprintf_P(
          buffer, sizeof(buffer),
          PSTR("AT+UPDATE=\"sequence\":\"%lu%03lu\",\"switch\":\"%s\",\"light_type\":1,"
               "\"colorR\":%d,\"colorG\":%d,\"colorB\":%d,\"bright\":%d,\"mode\":%d,"
               "\"sensitive\":%d,\"speed\":%d"),
          (unsigned long) ms, (unsigned long) (ms % 1000UL),
          ledState ? "on" : "off",
          redValue, greenValue, blueValue,
          brightness,
          mode_,
          sensitive_, speed_);
    } else {
      snprintf_P(
          buffer, sizeof(buffer),
          PSTR("AT+UPDATE=\"sequence\":\"%lu%03lu\",\"switch\":\"%s\",\"light_type\":1,"
               "\"colorR\":%d,\"colorG\":%d,\"colorB\":%d,\"bright\":%d,\"mode\":%d"),
          (unsigned long) ms, (unsigned long) (ms % 1000UL),
          ledState ? "on" : "off",
          redValue, greenValue, blueValue,
          brightness,
          mode_);
    }

    send_(buffer);
  }

  // Optional helper if you ever want to force static mode explicitly
  void setModeColorful() {
    mode_ = SONOFF_L1_MODE_COLORFUL;
    send_mode_only_(mode_);
  }

  void setModeGradient() {
    mode_ = SONOFF_L1_MODE_COLORFUL_GRADIENT;
    send_mode_only_(mode_);
  }

  void setModeBreath() {
    mode_ = SONOFF_L1_MODE_COLORFUL_BREATH;
    send_mode_only_(mode_);
  }

  void setModeRGBGradient() {
    mode_ = SONOFF_L1_MODE_RGB_GRADIENT;
    send_mode_only_(mode_);
  }

  void setModeRGBPulse() {
    mode_ = SONOFF_L1_MODE_RGB_PULSE;
    send_mode_only_(mode_);
  }

  void setModeRGBBreath() {
    mode_ = SONOFF_L1_MODE_RGB_BREATH;
    send_mode_only_(mode_);
  }

  void setModeRGBStrobe() {
    mode_ = SONOFF_L1_MODE_RGB_STROBE;
    send_mode_only_(mode_);
  }

  void setModeSync(int sensitive = 10, int speed = 50) {
    // Clamp to documented ranges
    if (sensitive < 1) sensitive = 1;
    if (sensitive > 10) sensitive = 10;
    if (speed < 1) speed = 1;
    if (speed > 100) speed = 100;

    mode_ = SONOFF_L1_MODE_SYNC_TO_MUSIC;
    sensitive_ = sensitive;
    speed_ = speed;

    const uint32_t ms = millis();
    char buffer[160];
    snprintf_P(
        buffer, sizeof(buffer),
        PSTR("AT+UPDATE=\"sequence\":\"%lu%03lu\",\"mode\":%d,\"sensitive\":%d,\"speed\":%d"),
        (unsigned long) ms, (unsigned long) (ms % 1000UL),
        mode_,
        sensitive_, speed_);

    send_(buffer);
  }

 protected:
  int mode_{SONOFF_L1_MODE_COLORFUL};
  int sensitive_{10};
  int speed_{50};

  void send_(const char *cmd) {
    Serial.print(cmd);
    Serial.write(0x1B);
    Serial.flush();
  }

  void send_mode_only_(int mode) {
    const uint32_t ms = millis();
    char buffer[120];
    snprintf_P(
        buffer, sizeof(buffer),
        PSTR("AT+UPDATE=\"sequence\":\"%lu%03lu\",\"mode\":%d"),
        (unsigned long) ms, (unsigned long) (ms % 1000UL),
        mode);
    send_(buffer);
  }
};

}  // namespace sonoff_l1
}  // namespace esphome
