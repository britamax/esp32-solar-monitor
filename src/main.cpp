/*
 * ============================================================
 * SOLAR MONITOR — main.cpp
 * Versi 2.0.0 — Finalisasi
 *
 * Hardware:
 *   Board    : ESP32 LOLIN32 Lite 4MB
 *   I2C SDA  : GPIO13 | SCL: GPIO12
 *   Buzzer   : GPIO27 (Active LOW)
 *   Button   : GPIO14
 *
 * Sensor:
 *   INA3221  → 3-channel power monitoring (selalu aktif)
 *   BMP280   → suhu, tekanan, ketinggian (on/off)
 *   MPU6050  → akselerometer, deteksi gempa (on/off)
 *   Buzzer   → alarm aktif (on/off)
 *
 * Fitur:
 *   - Web dashboard realtime (/)
 *   - Web setting (/setting)
 *   - System info + OTA update (/system)
 *   - Login session token (/login)
 *   - MQTT dual broker (TCP/SSL/WebSocket auto-detect)
 *   - Home Assistant MQTT Discovery otomatis
 *   - OLED display 4 halaman
 *   - Alarm threshold tegangan/arus/gempa
 *   - Tombol multi-fungsi GPIO14
 *   - kWh akumulasi dengan moving average
 *   - OTA update via browser
 *
 * GitHub: https://github.com/britamax/esp32-solar-monitor
 * ============================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "storage.h"
#include "session_manager.h"
#include "log_manager.h"
#include "ntp_manager.h"
#include "buzzer.h"
#include "ina3221.h"
#include "bmp280.h"
#include "mpu6050.h"
#include "oled.h"
#include "web_dashboard.h"
#include "web_setting.h"
#include "web_login.h"
#include "web_system.h"
#include "web_compressed.h"
#include "wifi_manager.h"
#include "mqtt_client.h"
#include "button_handler.h"

// Handler /log — didefinisikan di main.cpp agar LOGPAGE_HTML_GZ accessible
void handleLogPage(AsyncWebServerRequest* req) {
  if (!sessionMgr.isValid(req)) { req->redirect("/login"); return; }
  sendGzip(req, LOGPAGE_HTML_GZ, LOGPAGE_HTML_GZ_LEN, "text/html");
}
// ============================================================
// TIMING
// ============================================================
unsigned long lastSensorRead   = 0;
unsigned long lastKwhAccum     = 0;
unsigned long lastKwhSave      = 0;
unsigned long lastUptimeSave   = 0;
unsigned long uptimeSeconds    = 0;

const unsigned long SENSOR_INTERVAL    = 500;
const unsigned long KWH_ACCUM_INTERVAL = 10000;
const unsigned long UPTIME_INTERVAL    = 1000;

// Status sensor aktif
bool bmpActive  = false;
bool mpuActive  = false;
bool buzzActive = false;

// Flag kalibrasi MPU — diset dari web handler, diproses di loop()
volatile bool calibRequested = false;
volatile bool calibDone      = false;
volatile bool calibSuccess   = false;

// Flag reload MQTT setting
bool mqttReloadRequested = false;

// Status koneksi MQTT — dibaca oleh wifi_manager route handlers
bool mqttAOk = false;
bool mqttBOk = false;

// ============================================================
// MQTT TASK — berjalan di Core 0 (terpisah dari AsyncWebServer)
// ============================================================
void mqttTask(void* param) {
  Serial.println("[MQTT-TASK] Dimulai di Core 0");
  // Tunggu WiFi ready
  while (!wifiManager.isConnected) {
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  mqttMgr.begin();
  Serial.println("[MQTT-TASK] Broker diinisialisasi");

  for (;;) {
    if (wifiManager.isConnected) {
      if (mqttReloadRequested) {
        mqttReloadRequested = false;
        mqttMgr.reload();
        Serial.println("[MQTT] Reload dari setting web");
      }
      mqttMgr.handle(bmpActive, mpuActive);
      mqttAOk = mqttMgr.isAConnected();
      mqttBOk = mqttMgr.isBConnected();
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// IP untuk dashboard
String _serverIP = "";

// ============================================================
// RESET BUTTON BOOT CHECK — tahan saat boot = reset WiFi
// (fungsi multi-klik di loop() via ButtonHandler)
// ============================================================
void checkResetTrigger() {
  // Tahan saat boot:
  //   3 detik  = reset password ke default (admin/123123)
  //   10 detik = reset WiFi
  pinMode(PIN_RESET_BTN, INPUT_PULLUP);
  if (digitalRead(PIN_RESET_BTN) != LOW) return;

  Serial.println("[Boot] Tombol ditekan saat boot...");
  oledDisp.showStatus("Tahan 3s=Password", "Tahan 10s=WiFi", "Lepas=Batal");
  unsigned long start = millis();
  unsigned long held  = 0;
  bool reached3 = false;

  while (digitalRead(PIN_RESET_BTN) == LOW) {
    held = millis() - start;
    if (!reached3 && held >= 3000) {
      reached3 = true;
      oledDisp.showStatus("Lepas=Reset Pwd", "Tahan=Reset WiFi", "");
      if (storage.isBuzzEnabled()) buzzer.play(BUZZ_OK);
    }
    if (held >= 10000) {
      // Reset WiFi
      oledDisp.showStatus("!! RESET WiFi !!", "Menghapus setting", "Restart...");
      if (storage.isBuzzEnabled()) buzzer.play(BUZZ_WARNING);
      delay(2000);
      storage.clearWifiSettings();
      ESP.restart();
    }
    delay(50);
  }

  // Tombol dilepas — tentukan aksi dari durasi tahan
  if (held < 500) {
    // Lepas terlalu cepat — batal
    oledDisp.showStatus("Reset Dibatalkan", "", "");
    delay(1500);
  } else if (held >= 3000) {
    // Reset password
    oledDisp.showStatus("Reset Password", "admin / 123123", "Restart...");
    Serial.println("[Boot] Reset password ke default");
    storage.resetAuthToDefault();
    sessionMgr.invalidateAll();
    if (storage.isBuzzEnabled()) { buzzer.play(BUZZ_OK); delay(300); buzzer.play(BUZZ_OK); }
    delay(2000);
    ESP.restart();
  } else {
    oledDisp.showStatus("Reset Dibatalkan", "", "");
    delay(1500);
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n============================================");
  Serial.println("  Solar Monitor - Tahap 3 (Semua Sensor)");
  Serial.println("============================================");

  // 1. Init I2C
  Wire.begin(PIN_SDA, PIN_SCL);
  Serial.println("[I2C] SDA=13 SCL=12");

  // 2. Init Storage DULU — sebelum baca setting apapun
  storage.begin();
  Serial.println("[NVS] Storage siap");

  // 3. Init OLED
  if (oledDisp.begin()) {
    Serial.println("[OLED] OK");
    // Terapkan setting dari NVS (storage sudah siap)
    oledDisp.setBrightness(storage.getOledBrightness());
    oledDisp.setEnabled(storage.getOledEnabled());
    oledDisp.setAutoScroll(storage.getOledAutoScroll(), storage.getOledDuration());
    if (!storage.getOledAutoScroll()) oledDisp.setPage(storage.getOledPage());
  } else {
    Serial.println("[OLED] GAGAL");
  }

  // 4. Splash
  oledDisp.showSplash(storage.getDeviceName());

  // 5. Init Buzzer
  buzzActive = storage.isBuzzEnabled();
  buzzer.begin();
  buzzer.setEnabled(buzzActive);
  Serial.printf("[Buzzer] %s\n", buzzActive ? "ON" : "OFF");

  // 5b. Cek reset trigger (3x on/off dalam 10 detik)
  checkResetTrigger();

  // 6. Init INA3221 — selalu aktif
  ina.begin();
  if (ina.isOnline) {
    ina.setShunt(storage.getShuntOhms());
    Serial.println("[INA3221] OK");

    float ch1    = storage.getKwhCh1();
    float ch2in  = storage.getKwhCh2In();
    float ch2out = storage.getKwhCh2Out();
    float ch3    = storage.getKwhCh3();

    // Sanity check — nilai > 9999 kWh tidak wajar, reset
    bool sane = (ch1 < 9999.0f && ch2in < 9999.0f && ch2out < 9999.0f && ch3 < 9999.0f);
    // Sanity check — nilai negatif tidak wajar
    sane = sane && (ch1 >= 0 && ch2in >= 0 && ch2out >= 0 && ch3 >= 0);

    if (!sane) {
      Serial.println("[kWh] WARN: nilai tidak wajar, reset ke 0");
      storage.resetKwh();
      ch1 = ch2in = ch2out = ch3 = 0.0f;
    }

    ina.loadKwh(ch1, ch2in, ch2out, ch3);
    Serial.printf("[kWh] Load: CH1:%.3f CH2in:%.3f CH2out:%.3f CH3:%.3f\n",
      ch1, ch2in, ch2out, ch3);
  } else {
    Serial.println("[INA3221] GAGAL!");
    oledDisp.showError("INA3221 not found");
    delay(2000);
  }

  // 7. Init BMP280 — opsional
  if (storage.isBmpEnabled()) {
    bmp.begin();
    bmpActive = bmp.isOnline;
    Serial.printf("[BMP280] %s\n",
      bmpActive ? "OK" : "GAGAL/tidak terpasang");
    if (bmpActive) oledDisp.setBmpData(0, 0, 0, "---");
  } else {
    Serial.println("[BMP280] Dinonaktifkan");
  }

  // 8. Init MPU6050 — opsional
  if (storage.isMpuEnabled()) {
    mpu.begin();
    mpuActive = mpu.isOnline;
    Serial.printf("[MPU6050] %s\n",
      mpuActive ? "OK" : "GAGAL/tidak terpasang");

    if (mpuActive) {
      if (storage.isMpuCalibrated()) {
        mpu.loadCalibration(
          storage.getMpuCalAX(), storage.getMpuCalAY(), storage.getMpuCalAZ(),
          storage.getMpuCalGX(), storage.getMpuCalGY(), storage.getMpuCalGZ()
        );
        Serial.println("[MPU6050] Kalibrasi dimuat");
      } else {
        Serial.println("[MPU6050] Belum dikalibrasi");
      }
      oledDisp.setMpuData(0, 0, 0, 0, 0, "---");
    }
  } else {
    Serial.println("[MPU6050] Dinonaktifkan");
  }

  // 9. Muat uptime tersimpan
  uptimeSeconds = storage.getUptime();
  Serial.printf("[Uptime] Lanjut dari %lu detik\n", uptimeSeconds);

  // 10. WiFi Manager
  Serial.println("[WiFi] Memulai...");
  wifiManager.begin();

  // Update info WiFi ke OLED (halaman default)
  if (wifiManager.isConnected) {
    _serverIP = wifiManager.ipAddress;
    oledDisp.setWifiInfo("WiFi", storage.getWifiSSID(), wifiManager.ipAddress, true);
  } else if (wifiManager.isSetupMode) {
    _serverIP = "192.168.4.1";
    oledDisp.setWifiInfo("Setup", AP_SSID, "192.168.4.1", true);
  } else if (wifiManager.isAPMode) {
    _serverIP = "192.168.4.1";
    oledDisp.setWifiInfo("AP", AP_SSID, "192.168.4.1", true);
  }

  // Sensor status info ke OLED (bmpActive/mpuActive sudah di-set)
  oledDisp.setBmpData(0, 0, 0, "---");
  if (mpuActive) oledDisp.setMpuData(0, 0, 0, 0, 0, "AMAN");

  // 11. Beep startup
  if (buzzActive) buzzer.play(BUZZ_STARTUP);

  // 12. NTP — sync waktu setelah WiFi connect
  if (wifiManager.isConnected) {
    ntpMgr.begin();
    String ntpTs = ntpMgr.synced ? ntpMgr.getTimeStr() : String("");
    if (ntpMgr.synced) {
      logger.add(LOG_SYS, ("NTP sync OK: " + ntpTs).c_str(), ntpTs.c_str());
    } else {
      logger.add(LOG_SYS, "NTP sync gagal", nullptr);
    }
  }

  // Log boot event
  {
    String ts = ntpMgr.synced ? ntpMgr.getTimeStr() : String("");
    logger.addf(LOG_SYS, ts.length() ? ts.c_str() : nullptr,
      "Boot — FW v%s build %s", FIRMWARE_VERSION, FIRMWARE_BUILD);
    String wifiMsg = wifiManager.isConnected
      ? "Terhubung IP: " + wifiManager.ipAddress
      : String("AP Mode");
    logger.add(LOG_WIFI, wifiMsg.c_str(), ts.length() ? ts.c_str() : nullptr);
  }

  // 13. MQTT — jalankan di Core 0 via FreeRTOS (AsyncWebServer di Core 1)
  xTaskCreatePinnedToCore(
    mqttTask,   // fungsi task
    "mqttTask", // nama
    8192,       // stack size
    NULL,       // parameter
    1,          // prioritas
    NULL,       // handle
    0           // Core 0
  );
  Serial.println("[MQTT-TASK] Task dibuat di Core 0");

  // 14. Button handler
  btnHandler.begin();

  // 15. Init timing kWh dari millis() saat ini agar interval pertama tidak melonjak
  lastKwhAccum = millis();
  lastKwhSave  = millis();

  Serial.println("\n[READY] Semua sistem siap!\n");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  unsigned long now = millis();

  // Handle tombol fisik GPIO14
  btnHandler.handle();

  // Handle WiFi/AP
  wifiManager.handle();

  // Handle buzzer alarm (non-blocking)
  buzzer.handle();

  // Proses kalibrasi MPU — dari web ATAU dari tombol fisik
  if (calibRequested || btnHandler.calibRequested) {
    calibRequested             = false;
    btnHandler.calibRequested  = false;
    if (mpuActive) {
      oledDisp.showStatus("Kalibrasi MPU", "Jangan gerakkan!", "Proses...");
      mpu.calibrate(500);
      if (mpu.isCalibrated) {
        storage.saveMpuCalib(
          mpu.offAccX, mpu.offAccY, mpu.offAccZ,
          mpu.offGyroX, mpu.offGyroY, mpu.offGyroZ
        );
        calibSuccess = true;
        oledDisp.showStatus("Kalibrasi OK!", "Offset disimpan", "");
        if (storage.isBuzzEnabled()) { buzzer.play(BUZZ_OK); delay(200); buzzer.play(BUZZ_OK); }
        Serial.println("[MPU] Kalibrasi berhasil!");
      } else {
        calibSuccess = false;
        oledDisp.showStatus("Kalibrasi GAGAL", "Cek koneksi MPU", "");
        if (storage.isBuzzEnabled()) buzzer.play(BUZZ_WARNING);
        Serial.println("[MPU] Kalibrasi gagal!");
      }
      delay(2000);
    } else {
      calibSuccess = false;
      oledDisp.showStatus("MPU Tidak Aktif", "", "");
      delay(1500);
    }
    calibDone = true;
  }

  // Baca semua sensor tiap 500ms
  if (now - lastSensorRead >= SENSOR_INTERVAL) {

    // INA3221 — selalu
    ina.read();

    // BMP280
    if (bmpActive) {
      bmp.read();
      bmp.temperature += storage.getBmpTempOffset();  // koreksi offset
      oledDisp.setBmpData(
        bmp.temperature, bmp.pressure,
        bmp.altitude, bmp.weatherIcon()
      );
    }

    // MPU6050
    if (mpuActive) {
      mpu.read();
      oledDisp.setMpuData(
        mpu.accX, mpu.accY,
        mpu.gyroX, mpu.gyroZ,
        mpu.vibration, mpu.vibrationStatus()
      );

      // Deteksi gempa — hanya jika sudah dikalibrasi dan tidak snooze
      if (mpu.quakeDetected && mpu.isCalibrated && buzzActive
          && !buzzer.isActive && !btnHandler.isAlarmSnoozed()) {
        buzzer.playAlarm(
          storage.getBuzzQuakeOnMs(),
          storage.getBuzzQuakeOffMs(),
          storage.getBuzzQuakeDurSec()
        );
        mqttMgr.publishAlarm("GEMPA", "vibration=" + String(mpu.vibration, 3));
        Serial.printf("[GEMPA] Vibration: %.3fg\n", mpu.vibration);
      }
    }

    // Cek threshold tegangan baterai (CH2) — tidak bunyi saat snooze
    float vbat = ina.ch[1].voltage;
    if (vbat > 0.5f && !btnHandler.isAlarmSnoozed()) {
      if (vbat < storage.getAlarmVMin() && buzzActive && !buzzer.isActive) {
        buzzer.playAlarm(storage.getBuzzVoltOnMs(), storage.getBuzzVoltOffMs(), storage.getBuzzVoltDurSec());
        mqttMgr.publishAlarm("VOLT_LOW", "v=" + String(vbat, 2));
      } else if (vbat > storage.getAlarmVMax() && buzzActive && !buzzer.isActive) {
        buzzer.playAlarm(storage.getBuzzVoltOnMs(), storage.getBuzzVoltOffMs(), storage.getBuzzVoltDurSec());
        mqttMgr.publishAlarm("VOLT_HIGH", "v=" + String(vbat, 2));
      }
    }

    lastSensorRead = now;
  }

  // Handle MQTT — dihandle oleh mqttTask di Core 0
  // (dihapus dari loop untuk hindari konflik dengan AsyncWebServer)

  // Update OLED
  oledDisp.update(
    storage.getCh1Name(),
    storage.getCh2Name(),
    storage.getCh3Name()
  );

  // Akumulasi kWh tiap 10 detik
  if (now - lastKwhAccum >= KWH_ACCUM_INTERVAL) {
    float intervalSec = (now - lastKwhAccum) / 1000.0f;
    Serial.printf("[kWh] interval=%.1fs P0=%.3fW P1=%.3fW P2=%.3fW\n",
      intervalSec, ina.ch[0].power, ina.ch[1].power, ina.ch[2].power);

    // Log ke web jika interval tidak normal (>30 detik = potensi bug)
    if (intervalSec > 30.0f) {
      String ts = ntpMgr.synced ? ntpMgr.getTimeStr() : String("");
      logger.addf(LOG_KWH, ts.length() ? ts.c_str() : nullptr,
        "WARN interval=%.1fs (tidak normal!) P:%.2fW B:%.2fW L:%.2fW",
        intervalSec, ina.ch[0].power, ina.ch[1].power, ina.ch[2].power);
    }

    ina.accumulateKwh(intervalSec);
    lastKwhAccum = now;
  }

  // Simpan kWh tiap 5 menit + log
  if (now - lastKwhSave >= KWH_SAVE_MS) {
    storage.saveKwh(ina.kwh[0], ina.kwhCh2In, ina.kwhCh2Out, ina.kwh[2]);
    String ts = ntpMgr.synced ? ntpMgr.getTimeStr() : String("");
    float lastInterval = (now - lastKwhAccum) / 1000.0f;
    logger.addf(LOG_KWH, ts.length() ? ts.c_str() : nullptr,
      "Save — Panel:%.3f BatIn:%.3f BatOut:%.3f Beban:%.3f kWh | interval:%.1fs",
      ina.kwh[0], ina.kwhCh2In, ina.kwhCh2Out, ina.kwh[2], lastInterval);
    lastKwhSave = now;
  }

  // NTP handle — sync tiap 6 jam
  ntpMgr.handle();

  // Uptime counter
  if (now - lastUptimeSave >= UPTIME_INTERVAL) {
    uptimeSeconds++;
    lastUptimeSave = now;
    if (uptimeSeconds % 600 == 0) storage.saveUptime(uptimeSeconds);
  }

  delay(50);
}