// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

#include <LittleFS.h>

#include <OpenFontRender.h>
#include <TJpg_Decoder.h>

#include "fonts/open-sans.h"
#include "GfxUi.h"

#include <JsonListener.h>
#include <OpenWeatherMapCurrent.h>
#include <OpenWeatherMapForecast.h>

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
TFT_eSprite timeSprite = TFT_eSprite(&tft);
GfxUi ui = GfxUi(&tft, &ofr);

int lastMinute = -1;

// time management variables
int updateIntervalMillis = UPDATE_INTERVAL_MINUTES * 60 * 1000;
unsigned long lastTimeSyncMillis = 0;
unsigned long lastUpdateMillis = 0;

OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapForecastData forecasts[MAX_FORECASTS];



// ----------------------------------------------------------------------------
// Function prototypes (declarations)
// ----------------------------------------------------------------------------
void drawProgress(const char *text, int8_t percentage);
void drawTime();
void initJpegDecoder();
void initOpenFontRender();
bool pushImageToTft(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
void syncTime();
void update();
void updateData(boolean updateProgressBar);



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
  timeSprite.createSprite(timeSpritePos.width, timeSpritePos.height);
  logDisplayDebugInfo(&tft);

  initFileSystem();
  initOpenFontRender();
}

void loop(void) {
  // update if
  // - never (successfully) updated before OR
  // - last sync too far back
  if (lastTimeSyncMillis == 0 ||
      lastUpdateMillis == 0 ||
      (millis() - lastUpdateMillis) > updateIntervalMillis) {
    update();
  } else {
    drawTime();
  }
  delay(1000);
}



// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

void drawProgress(const char *text, int8_t percentage) {
  ofr.setFontSize(24);
  int pbWidth = tft.width() - 100;
  int pbX = (tft.width() - pbWidth)/2;
  int pbY = 260;
  int progressTextY = 210;

  tft.fillRect(0, progressTextY, tft.width(), 40, TFT_BLACK);
  ofr.cdrawString(text, tft.width() / 2, progressTextY);
  ui.drawProgressBar(pbX, pbY, pbWidth, 15, percentage, TFT_WHITE, TFT_TP_BLUE);
}

void drawTime() {
  timeSprite.fillSprite(TFT_BLACK);
  ofr.setFontSize(48);
  ofr.setDrawer(timeSprite);
  ofr.drawString(getCurrentTimestamp(UI_TIME_FORMAT).c_str(), timePosX, 0);
  timeSprite.pushSprite(timeSpritePos.x, timeSpritePos.y);
  // set the drawer back since we temporarily changed it to the time sprite above
  ofr.setDrawer(tft);
}

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

void update() {
  tft.fillScreen(TFT_BLACK);
  ui.drawLogo();

  ofr.setFontSize(16);
  ofr.cdrawString(APP_NAME, tft.width() / 2, tft.height() - 50);
  ofr.cdrawString(VERSION, tft.width() / 2, tft.height() - 30);

  drawProgress("Starting WiFi...", 10);
  if (WiFi.status() != WL_CONNECTED) {
    startWiFi();
  }

  drawProgress("Synchronizing time...", 30);
  syncTime();

  updateData(true);

  drawProgress("Ready", 100);
  lastUpdateMillis = millis();

  tft.fillScreen(TFT_BLACK);

  ofr.setFontSize(16);
  ofr.cdrawString(String("Last weather update: " + getCurrentTimestamp(UI_TIME_FORMAT_NO_SECONDS)).c_str(), tft.width() / 2, 10);
  drawTime();
}

void updateData(boolean updateProgressBar) {
  if(updateProgressBar) drawProgress("Updating weather...", 70);
  OpenWeatherMapCurrent *currentWeatherClient = new OpenWeatherMapCurrent();
  currentWeatherClient->setMetric(IS_METRIC);
  currentWeatherClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  currentWeatherClient->updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_API_KEY, OPEN_WEATHER_MAP_LOCATION_ID);
  delete currentWeatherClient;
  currentWeatherClient = nullptr;
  log_i("Current weather in %s: %s, %.1fC°", currentWeather.cityName, currentWeather.description.c_str(), currentWeather.feelsLike);

  if(updateProgressBar) drawProgress("Updating forecast...", 90);
  OpenWeatherMapForecast *forecastClient = new OpenWeatherMapForecast();
  forecastClient->setMetric(IS_METRIC);
  forecastClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  uint8_t allowedHours[] = {12, 0};
  forecastClient->setAllowedHours(allowedHours, sizeof(allowedHours));
  forecastClient->updateForecastsById(forecasts, OPEN_WEATHER_MAP_API_KEY, OPEN_WEATHER_MAP_LOCATION_ID, MAX_FORECASTS);
  delete forecastClient;
  forecastClient = nullptr;
}
