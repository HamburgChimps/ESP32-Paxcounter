#ifndef OTA_H
#define OTA_H

#ifdef USE_OTA

#include "globals.h"
#if (HAS_LED != NOT_A_PIN)
#include "led.h"
#endif
#ifdef HAS_DISPLAY
#include "display.h"
#endif

#include <Update.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <BintrayClient.h>
#include <string>
#include <algorithm>

int do_ota_update();
void start_ota_update();
int version_compare(const String v1, const String v2);
void ota_display(const uint8_t row, const std::string status,
                 const std::string msg);
void show_progress(unsigned long current, unsigned long size);

#endif // USE_OTA

#endif // OTA_H
