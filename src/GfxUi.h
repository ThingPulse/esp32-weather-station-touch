// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

// Based on https://github.com/Bodmer/OpenWeather/blob/main/examples/TFT_eSPI_OpenWeather_LittleFS/GfxUi.h

#pragma once

#include <FS.h>
#include <LittleFS.h>
#include <OpenFontRender.h>
#include <TFT_eSPI.h>

// JPEG decoder library
#include <TJpg_Decoder.h>

// Maximum of 85 for BUFFPIXEL as 3 x this value is stored in an 8 bit variable!
// 32 is an efficient size for LittleFS due to SPI hardware pipeline buffer size
// A larger value of 80 is better for SD cards
#define BUFFPIXEL 32

class GfxUi {
public:
  GfxUi(TFT_eSPI *tft, OpenFontRender *render);
  void drawBmp(String filename, uint16_t x, uint16_t y);
  void drawLogo();
  void drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                       uint8_t percentage, uint16_t frameColor,
                       uint16_t barColor);

private:
  TFT_eSPI *_tft;
  OpenFontRender *_ofr;
  uint16_t read16(fs::File &f);
  uint32_t read32(fs::File &f);
};
