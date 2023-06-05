// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

#pragma once

#include "time.h"
#include "settings.h"

char timestampBuffer[26];

uint8_t getCurrentWeekday();

/**
 * Use the 3h/5d OWM forecast data to condense it into minimal daily forecasts (as required by this app).
 * Algo:
 * - iterate over all OWM forecasts
 * - skip the ones from the current day
 * - find min/max temp for each day by comparing the temp from the current 3h forecast against the min/max found so far
 * - use the condition code (i.e. the weather) of the one 3h forecast closest to 12 moon
 *
 * @param forecasts NUMBER_OF_DAY_FORECASTS 3h/5d OWM forecast containers
 * @return DayForecast* array of NUMBER_OF_DAY_FORECASTS minimal daily forecast containers
 */
DayForecast* calculateDayForecasts(OpenWeatherMapForecastData *forecasts) {
  uint8_t weekday = getCurrentWeekday();
  static DayForecast dayForecasts[NUMBER_OF_DAY_FORECASTS];
  for (int i = 0; i < NUMBER_OF_DAY_FORECASTS; i++) {
    dayForecasts[i] = {200.0, -200.0, 0, 23};
  }
  int k = -1;
  int currentForecastDay = -1;

  for (uint8_t i = 0; i < NUMBER_OF_FORECASTS; i++) {
    OpenWeatherMapForecastData forecast = forecasts[i];
    time_t forecastTimeUtc = forecast.observationTime;
    struct tm *forecastLocalTime = localtime(&forecastTimeUtc);

    if (weekday == forecastLocalTime->tm_wday) {
      strftime(timestampBuffer, sizeof(timestampBuffer), SYSTEM_TIMESTAMP_FORMAT, forecastLocalTime);
      log_d("Skipping forecast for today %s", timestampBuffer);
      continue;
    }

    if (forecastLocalTime->tm_wday != currentForecastDay) {
      currentForecastDay = forecastLocalTime->tm_wday;
      k++;
      dayForecasts[k].day = currentForecastDay;
    }
    log_d("Current forecast day: %d, array index: %d, hour: %d, temp: %.1f", currentForecastDay, k, forecastLocalTime->tm_hour, forecast.temp);
    if (forecast.temp < dayForecasts[k].minTemp) dayForecasts[k].minTemp = forecast.temp;
    if (forecast.temp > dayForecasts[k].maxTemp) dayForecasts[k].maxTemp = forecast.temp;
    // find the condition closest to 12 noon (tm_hour is 0-23)
    if (abs(12 - forecastLocalTime->tm_hour) < abs(12 - dayForecasts[k].conditionHour)) {
      dayForecasts[k].conditionCode = forecast.weatherId;
      dayForecasts[k].conditionHour = forecastLocalTime->tm_hour;
    }
  }
  return dayForecasts;
}

uint8_t getCurrentWeekday() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    log_e("Failed to obtain time.");
    return -1;
  }
  return timeinfo.tm_wday;
}

String getCurrentTimestamp(const char* format) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    log_e("Failed to obtain time.");
    return "";
  }
  strftime(timestampBuffer, sizeof(timestampBuffer), format, &timeinfo);
  return String(timestampBuffer);
}

boolean initTime() {
  struct tm timeinfo;

  log_i("Synchronizing time.");
  // Connect to NTP server with 0 TZ offset, call setTimezone() later
  configTime(0, 0, "pool.ntp.org");
  if (!getLocalTime(&timeinfo)) {
    log_e("Failed to obtain time.");
    return false;
  }
  log_i("UTC time: %s", getCurrentTimestamp(SYSTEM_TIMESTAMP_FORMAT).c_str());
  return true;
}

void logBanner() {
  log_i("**********************************************");
  log_i("* ThingPulse Weather Station Touch v%s *", VERSION);
  log_i("**********************************************");
}

void logMemoryStats() {
  log_i("Total heap: %d", ESP.getHeapSize());
  log_i("Free heap: %d", ESP.getFreeHeap());
  log_i("Total PSRAM: %d", ESP.getPsramSize());
  log_i("Free PSRAM: %d", ESP.getFreePsram());
}

void setTimezone(const char* timezone) {
  log_i("Setting timezone to '%s'.", timezone);
  // Clock settings are adjusted to show the new local time
  setenv("TZ", timezone, 1);
  tzset();
}

// Algorithm: http://howardhinnant.github.io/date_algorithms.html
int days_from_epoch(int y, int m, int d) {
  y -= m <= 2;
  int era = y / 400;
  int yoe = y - era * 400;                                  // [0, 399]
  int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0, 365]
  int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;          // [0, 146096]
  return era * 146097 + doe - 719468;
}
// https://stackoverflow.com/a/58037981/131929
// aka timegm() but that's already defined in the Weather Station lib but not accessible
time_t mkgmtime(struct tm const *t) {
  int year = t->tm_year + 1900;
  int month = t->tm_mon; // 0-11
  if (month > 11) {
    year += month / 12;
    month %= 12;
  } else if (month < 0) {
    int years_diff = (11 - month) / 12;
    year -= years_diff;
    month += 12 * years_diff;
  }
  int days_since_epoch = days_from_epoch(year, month + 1, t->tm_mday);

  return 60 * (60 * (24L * days_since_epoch + t->tm_hour) + t->tm_min) + t->tm_sec;
}
