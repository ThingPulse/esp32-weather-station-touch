// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

#pragma once

#include <FT6236.h>
#include <TFT_eSPI.h>

#include "settings.h"

// The library defines the type "setup_t" as a struct
// Calling tft.getSetup(user) populates it with the settings
setup_t user;

uint8_t readRegister8(uint8_t reg);

void initTft(TFT_eSPI *tft) {
  tft->init();
  tft->setRotation(TFT_ROTATION);
  // We need to swap the colour bytes (endianess)
  // -> https://github.com/Bodmer/TJpg_Decoder/blob/master/examples/LittleFS/LittleFS_Jpg/LittleFS_Jpg.ino#L60
  tft->setSwapBytes(true);

  // Check for backlight pin if not connected to VCC
#ifndef TFT_BL
  log_i("No TFT backlight pin defined.");
#else
  log_d("Configuring TFT backlight at pin %d.", TFT_BL);
  // Setup PWM channel, ledc is a LED Control Function
  ledcSetup(0, 5000, 8);
  ledcAttachPin(TFT_BL, 0);
  ledcWrite(0, TFT_LED_BRIGHTNESS);
#endif
  tft->fillScreen(TFT_BLACK);
}

void initTouchScreen(FT6236 *ts) {
  if (ts->begin(TOUCH_SENSITIVITY, TOUCH_SDA, TOUCH_SCL)) {
    log_i("Capacitive touch started.");
  } else {
    log_e("Failed to start the capacitive touchscreen.");
  }
  ts->setRotation(TOUCH_ROTATION);
}

void logDisplayDebugInfo(TFT_eSPI *tft) {
  tft->getSetup(user);

  log_i("TFT info");
  log_i("===========================");
  // inspired by https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Test%20and%20diagnostics/Read_User_Setup/Read_User_Setup.ino
  log_i("TFT_eSPI version:  %s", user.version);
  log_i("Transactions:      %s", (user.trans  ==  1) ? "yes" : "no");
  log_i("Display driver:    0x%04x", user.tft_driver);
  log_i("Display width:     %d (at rotation 0)", user.tft_width);
  log_i("Display height:    %d (at rotation 0)", user.tft_height);
  log_i("Fonts loaded:");
  uint16_t fonts = tft->fontsLoaded();
  if (fonts & (1 << 1))       log_i("- font GLCD        loaded");
  if (fonts & (1 << 2))       log_i("- font 2           loaded");
  if (fonts & (1 << 4))       log_i("- font 4           loaded");
  if (fonts & (1 << 6))       log_i("- font 6           loaded");
  if (fonts & (1 << 7))       log_i("- font 7           loaded");
  if (fonts & (1 << 9))       log_i("- font 8N          loaded");
  else
  if (fonts & (1 << 8))       log_i("- font 8           loaded");
  if (fonts & (1 << 15))      log_i("Smooth font:       enabled");
  if (user.serial == 1)       log_i("SPI frequency:     %.2f", user.tft_spi_freq/10.0);
  log_i("");
  log_i("Touch screen info");
  log_i("===========================");
  // ESP-IDF-logging equivalent to ts.debug(), this required to copy the ts.readRegister8() function as it's private
  log_i("Vendor ID:         0x%02x", readRegister8(FT6236_REG_VENDID));
  log_i("Chip ID:           0x%02x", readRegister8(FT6236_REG_CHIPID));
  log_i("Firmware version:  %d", readRegister8(FT6236_REG_FIRMVERS));
  log_i("Point rate:        %dHz", readRegister8(FT6236_REG_POINTRATE));
  log_i("Sensitivity:       %d (threshold)", readRegister8(FT6236_REG_THRESHHOLD));
}

uint8_t readRegister8(uint8_t reg) {
  uint8_t x;

  Wire.beginTransmission(FT6236_ADDR);
  Wire.write((byte)reg);
  Wire.endTransmission();

  Wire.requestFrom((byte)FT6236_ADDR, (byte)1);
  x = Wire.read();

  return x;
}
