#pragma once

// ============================================================
// SOLAR MONITOR — button_handler.h
// Multi-fungsi tombol GPIO14
//
// 1x tekan  → Ganti halaman OLED
// 2x tekan  → Silence alarm (snooze 10 menit)
// 3x tekan  → Toggle OLED on/off
// Tahan 3d  → Kalibrasi MPU6050
// Tahan 10d → Reset WiFi
// ============================================================

#include "config.h"
#include "oled.h"
#include "buzzer.h"
#include "storage.h"

// Timing
#define BTN_DEBOUNCE_MS     50
#define BTN_DOUBLE_MS       400    // window deteksi multi-klik
#define BTN_HOLD_CALIB_MS   3000   // tahan 3 detik = kalibrasi
#define BTN_HOLD_RESET_MS   10000  // tahan 10 detik = reset WiFi
#define BTN_SNOOZE_MS       600000UL  // 10 menit snooze alarm

class ButtonHandler {
public:
  // Snooze alarm — dicek dari loop() sebelum trigger buzzer
  bool        alarmSnoozed   = false;
  unsigned long snoozeUntil  = 0;

  // Flag dari tombol — diproses di loop() utama
  bool        calibRequested = false;
  bool        wifiResetReq   = false;

  void begin() {
    pinMode(PIN_RESET_BTN, INPUT_PULLUP);
    Serial.println("[BTN] Button handler siap di GPIO" + String(PIN_RESET_BTN));
  }

  // Panggil di setiap loop() — non-blocking
  void handle() {
    _updateSnooze();
    _readButton();
  }

  // Cek apakah alarm sedang di-snooze
  bool isAlarmSnoozed() {
    return alarmSnoozed && (millis() < snoozeUntil);
  }

private:
  // State mesin tombol
  bool          _lastRaw      = HIGH;
  bool          _state        = HIGH;   // debounced state
  unsigned long _debounceTime = 0;
  unsigned long _pressTime    = 0;      // kapan mulai ditekan
  unsigned long _releaseTime  = 0;      // kapan terakhir dilepas
  int           _clickCount   = 0;
  bool          _holdHandled  = false;  // flag hold sudah diproses
  bool          _inHold       = false;

  // ============================================================
  void _updateSnooze() {
    if (alarmSnoozed && millis() >= snoozeUntil) {
      alarmSnoozed = false;
      Serial.println("[BTN] Snooze alarm selesai");
    }
  }

  // ============================================================
  void _readButton() {
    bool raw = digitalRead(PIN_RESET_BTN);
    unsigned long now = millis();

    // Debounce
    if (raw != _lastRaw) {
      _debounceTime = now;
      _lastRaw = raw;
    }
    if (now - _debounceTime < BTN_DEBOUNCE_MS) return;

    bool pressed = (raw == LOW);  // active LOW

    // ---- TOMBOL BARU DITEKAN ----
    if (pressed && _state == HIGH) {
      _state       = LOW;
      _pressTime   = now;
      _holdHandled = false;
      _inHold      = false;
    }

    // ---- TOMBOL MASIH DITEKAN (hold detection) ----
    if (pressed && _state == LOW && !_holdHandled) {
      unsigned long held = now - _pressTime;

      if (held >= BTN_HOLD_RESET_MS) {
        // Tahan 10 detik → Reset WiFi
        _holdHandled = true;
        _inHold      = true;
        _clickCount  = 0;
        _doResetWifi();

      } else if (held >= BTN_HOLD_CALIB_MS) {
        // Tahan 3 detik → tampil warning di OLED (tapi tunggu lepas)
        if (!_inHold) {
          _inHold = true;
          // Tampil instruksi agar user tahu harus diam
          oledDisp.showStatus("KALIBRASI MPU", "Ratakan perangkat", "Lepas tombol...");
          Serial.println("[BTN] Hold 3s — siap kalibrasi MPU");
        }
      }
    }

    // ---- TOMBOL DILEPAS ----
    if (!pressed && _state == LOW) {
      _state = HIGH;
      unsigned long held = now - _pressTime;

      if (_holdHandled) {
        // Hold action sudah diproses, abaikan release
        _holdHandled = false;
        _inHold      = false;
        return;
      }

      if (_inHold && held >= BTN_HOLD_CALIB_MS && held < BTN_HOLD_RESET_MS) {
        // Dilepas setelah 3–10 detik → Kalibrasi MPU
        _inHold     = false;
        _clickCount = 0;
        calibRequested = true;
        Serial.println("[BTN] Kalibrasi MPU diminta");
        return;
      }

      // Klik pendek — tambah counter
      _inHold      = false;
      _clickCount++;
      _releaseTime = now;
    }

    // ---- CEK MULTI-KLIK (setelah window habis) ----
    if (_clickCount > 0 && !pressed && (now - _releaseTime >= BTN_DOUBLE_MS)) {
      _processClicks(_clickCount);
      _clickCount = 0;
    }
  }

  // ============================================================
  void _processClicks(int n) {
    Serial.printf("[BTN] %d klik terdeteksi\n", n);
    switch (n) {
      case 1: _do1Click(); break;
      case 2: _do2Click(); break;
      case 3: _do3Click(); break;
      default: break;  // 4+ klik diabaikan
    }
  }

  // ---- 1x KLIK: Ganti halaman OLED ----
  void _do1Click() {
    oledDisp.nextPage();
    Serial.println("[BTN] Next page OLED");
  }

  // ---- 2x KLIK: Silence alarm (snooze 10 menit) ----
  void _do2Click() {
    if (buzzer.isActive) {
      buzzer.stop();
    }
    alarmSnoozed = true;
    snoozeUntil  = millis() + BTN_SNOOZE_MS;
    Serial.println("[BTN] Alarm snoozed 10 menit");

    // Feedback OLED
    oledDisp.showStatus("Alarm Dibisukan", "Snooze 10 menit", "");
    // Beep 1x pendek sebagai konfirmasi
    if (storage.isBuzzEnabled()) buzzer.play(BUZZ_OK);
    delay(1500);
  }

  // ---- 3x KLIK: Toggle OLED on/off ----
  void _do3Click() {
    bool nowOn = !storage.getOledEnabled();
    storage.setOledEnabled(nowOn);
    oledDisp.setEnabled(nowOn);
    Serial.printf("[BTN] OLED %s\n", nowOn ? "ON" : "OFF");

    if (nowOn) {
      if (storage.isBuzzEnabled()) buzzer.play(BUZZ_OK);       // 1x beep = ON
    } else {
      if (storage.isBuzzEnabled()) {
        buzzer.play(BUZZ_OK); delay(200); buzzer.play(BUZZ_OK); // 2x beep = OFF
      }
    }
  }

  // ---- TAHAN 10 DETIK: Reset WiFi ----
  void _doResetWifi() {
    Serial.println("[BTN] Reset WiFi diminta!");
    oledDisp.showStatus("!! RESET WiFi !!", "Menghapus setting", "Restart...");
    if (storage.isBuzzEnabled()) buzzer.play(BUZZ_WARNING);
    delay(2000);
    storage.clearWifiSettings();
    ESP.restart();
  }
};

ButtonHandler btnHandler;
