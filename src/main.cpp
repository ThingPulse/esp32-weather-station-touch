// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

#include <LittleFS.h>

#include <OpenFontRender.h>
#include <TJpg_Decoder.h>

#include "fonts/open-sans.h"
#include "GfxUi.h"

#include "connectivity.h"
#include "display.h"
#include "persistence.h"
#include "settings.h"
#include "util.h"



// ----------------------------------------------------------------------------
// Globals
// ----------------------------------------------------------------------------
OpenFontRender ofr;
FT6236 ts = FT6236(TFT_HEIGHT, TFT_WIDTH);
TFT_eSPI tft = TFT_eSPI();
GfxUi ui = GfxUi(&tft, &ofr);

int lastMinute = -1;

// time management variables
int timeSyncIntervalMillis = TIME_SYNC_INTERVAL_HOURS * 3600 * 1000;
unsigned long lastTimeSyncMillis = 0;



// ----------------------------------------------------------------------------
// Function prototypes (declarations)
// ----------------------------------------------------------------------------
void initJpegDecoder();
void initOpenFontRender();
bool pushImageToTft(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
void syncTime();



// ----------------------------------------------------------------------------
// setup() & loop()
// ----------------------------------------------------------------------------
void setup(void) {
  Serial.begin(115200);
  delay(1000);

  logBanner();
  logMemoryStats();

  initJpegDecoder();
  initTouchScreen(&ts);
  initTft(&tft);
  logDisplayDebugInfo(&tft);

  initFileSystem();
  initOpenFontRender();


  ui.drawLogo();

  ofr.setFontSize(16);
  ofr.cdrawString(APP_NAME, tft.width() / 2, tft.height() - 50);
  ofr.cdrawString(VERSION, tft.width() / 2, tft.height() - 30);

  ofr.setFontSize(24);
  int pbWidth = tft.width() - 100;
  int pbX = (tft.width() - pbWidth)/2;
  int pbY = 260;
  int progressTextY = 210;

  ofr.cdrawString("Starting WiFi...", tft.width() / 2, progressTextY);
  ui.drawProgressBar(pbX, pbY, pbWidth, 15, 20, TFT_WHITE, TFT_TP_BLUE);
  startWiFi();

  tft.fillRect(0, progressTextY, tft.width(), 40, TFT_BLACK);
  ofr.cdrawString("Synchronizing time...", tft.width() / 2, progressTextY);
  ui.drawProgressBar(pbX, pbY, pbWidth, 15, 60, TFT_WHITE, TFT_TP_BLUE);
  syncTime();

  tft.fillRect(0, progressTextY, tft.width(), 40, TFT_BLACK);
  ofr.cdrawString("Ready", tft.width() / 2, progressTextY);
  ui.drawProgressBar(pbX, pbY, pbWidth, 15, 100, TFT_WHITE, TFT_TP_BLUE);
}

void loop(void) {
  // re-sync time if
  // - never (successfully) synced before OR
  // - last sync too far back
  if (lastTimeSyncMillis == 0 || (millis() - lastTimeSyncMillis) > timeSyncIntervalMillis) {
    syncTime();
  }
}



// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

void initJpegDecoder() {
    // The JPEG image can be scaled by a factor of 1, 2, 4, or 8 (default: 0)
  TJpgDec.setJpgScale(1);
  // The decoder must be given the exact name of the rendering function
  TJpgDec.setCallback(pushImageToTft);
}

void initOpenFontRender() {
  ofr.loadFont(opensans, sizeof(opensans));
  ofr.setDrawer(tft);
  ofr.setFontColor(TFT_WHITE);
  ofr.setBackgroundColor(TFT_BLACK);
}

// Function will be called as a callback during decoding of a JPEG file to
// render each block to the TFT.
bool pushImageToTft(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  // Stop further decoding as image is running off bottom of screen
  if (y >= tft.height()) {
    return 0;
  }

  // Automatically clips the image block rendering at the TFT boundaries.
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}

void syncTime() {
  if (initTime()) {
    lastTimeSyncMillis = millis();
    setTimezone(TIMEZONE);
    log_i("Current local time: %s", getCurrentTimestamp(SYSTEM_TIMESTAMP_FORMAT).c_str());
  }
}
