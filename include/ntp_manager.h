#pragma once

// ============================================================
// SOLAR MONITOR — ntp_manager.h
// NTP time sync — sync saat boot + tiap 6 jam
// ============================================================

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "storage.h"

#define NTP_SYNC_INTERVAL_MS  (6UL * 60 * 60 * 1000)  // 6 jam

class NtpManager {
public:
  bool    synced     = false;
  String  lastSync   = "--";
  unsigned long lastSyncMs = 0;

  void begin() {
    if (!storage.getNtpEnabled()) {
      Serial.println("[NTP] Disabled");
      return;
    }
    sync();
  }

  // Panggil dari loop — sync tiap 6 jam
  void handle() {
    if (!storage.getNtpEnabled()) return;
    if (WiFi.status() != WL_CONNECTED) return;
    unsigned long now = millis();
    if (lastSyncMs == 0 || now - lastSyncMs >= NTP_SYNC_INTERVAL_MS) {
      sync();
    }
  }

  // Sync sekarang
  void sync() {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[NTP] WiFi tidak tersambung, skip sync");
      return;
    }
    int offset = storage.getNtpOffset();
    String server = storage.getNtpServer();
    long gmtOffset = (long)offset * 3600;

    Serial.printf("[NTP] Sync dari %s (UTC%+d)...\n", server.c_str(), offset);
    configTime(gmtOffset, 0, server.c_str(), "time.cloudflare.com", "time.google.com");

    // Tunggu sync maksimal 5 detik
    struct tm timeinfo;
    int retry = 0;
    while (!getLocalTime(&timeinfo) && retry < 10) {
      delay(500);
      retry++;
    }

    if (getLocalTime(&timeinfo)) {
      synced = true;
      lastSyncMs = millis();
      char buf[32];
      strftime(buf, sizeof(buf), "%d %b %Y %H:%M:%S", &timeinfo);
      lastSync = String(buf);
      Serial.printf("[NTP] Sync OK: %s\n", buf);
    } else {
      synced = false;
      Serial.println("[NTP] Sync GAGAL");
    }
  }

  // Ambil waktu sebagai string "DD Mon YYYY HH:MM:SS"
  String getTimeStr() {
    if (!synced) return "--:--:--";
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return "--:--:--";
    char buf[32];
    strftime(buf, sizeof(buf), "%d %b %Y %H:%M:%S", &timeinfo);
    return String(buf);
  }

  // Ambil waktu singkat "HH:MM:SS"
  String getTimeShort() {
    if (!synced) return "--:--:--";
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return "--:--:--";
    char buf[12];
    strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
    return String(buf);
  }

  // Ambil epoch timestamp
  time_t getEpoch() {
    time_t now;
    time(&now);
    return now;
  }

  // Terapkan ulang timezone tanpa sync ulang (setelah setting berubah)
  void applyTimezone() {
    int offset = storage.getNtpOffset();
    long gmtOffset = (long)offset * 3600;
    setenv("TZ", "", 1);
    tzset();
    configTime(gmtOffset, 0, storage.getNtpServer().c_str());
    synced = false;
    sync();
  }
};

NtpManager ntpMgr;
