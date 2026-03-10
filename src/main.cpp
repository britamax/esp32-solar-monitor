/*
 * ============================================================
 * SOLAR MONITOR — main.cpp
 * Tahap 3: WiFi Manager + AP Setup + Semua Sensor
 *
 * Sensor:
 *   INA3221  → selalu aktif (sensor utama)
 *   BMP280   → on/off via setting
 *   MPU6050  → on/off via setting
 *   Buzzer   → on/off via setting
 *
 * Alur boot:
 *   1. Init semua sensor
 *   2. Coba konek WiFi (jika mode wifi)
 *      - Berhasil → mode normal
 *      - Gagal    → AP Setup Mode
 *   3. Jika mode AP → hotspot permanen
 *   4. Loop: baca sensor → update OLED → akumulasi kWh
 * ============================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "storage.h"
#include "buzzer.h"
#include "ina3221.h"
#include "bmp280.h"
#include "mpu6050.h"
#include "oled.h"
#include "wifi_manager.h"

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

  // 2. Init OLED
  if (oledDisp.begin()) {
    Serial.println("[OLED] OK");
  } else {
    Serial.println("[OLED] GAGAL");
  }

  // 3. Splash
  oledDisp.showSplash(DEVICE_NAME_DEFAULT);

  // 4. Init Storage
  storage.begin();
  Serial.println("[NVS] Storage siap");

  // 5. Init Buzzer
  buzzActive = storage.isBuzzEnabled();
  buzzer.begin();
  buzzer.setEnabled(buzzActive);
  Serial.printf("[Buzzer] %s\n", buzzActive ? "ON" : "OFF");

  // 6. Init INA3221 — selalu aktif
  ina.begin();
  if (ina.isOnline) {
    Serial.println("[INA3221] OK");
    ina.loadKwh(
      storage.getKwhCh1(),
      storage.getKwhCh2In(),
      storage.getKwhCh2Out(),
      storage.getKwhCh3()
    );
    Serial.printf("[kWh] CH1:%.3f CH2in:%.3f CH2out:%.3f CH3:%.3f\n",
      storage.getKwhCh1(), storage.getKwhCh2In(),
      storage.getKwhCh2Out(), storage.getKwhCh3());
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
    oledDisp.setWifiInfo("WiFi", storage.getWifiSSID(), wifiManager.ipAddress, true);
  } else if (wifiManager.isSetupMode) {
    oledDisp.setWifiInfo("Setup", AP_SSID, "192.168.4.1", true);
  } else if (wifiManager.isAPMode) {
    oledDisp.setWifiInfo("AP", AP_SSID, "192.168.4.1", true);
  }

  // Sensor status info ke OLED (bmpActive/mpuActive sudah di-set)
  oledDisp.setBmpData(0, 0, 0, "---");
  if (mpuActive) oledDisp.setMpuData(0, 0, 0, 0, 0, "AMAN");

  // 11. Beep startup
  if (buzzActive) buzzer.play(BUZZ_STARTUP);

  // OLED default: halaman WiFi (PAGE_WIFI = 0) — sudah di-set saat begin()
  // Info WiFi akan diisi oleh wifiManager.begin() di atas
  Serial.println("\n[READY] Semua sistem siap!\n");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  unsigned long now = millis();

  // Handle WiFi/AP
  wifiManager.handle();

  // Baca semua sensor tiap 500ms
  if (now - lastSensorRead >= SENSOR_INTERVAL) {

    // INA3221 — selalu
    ina.read();

    // BMP280
    if (bmpActive) {
      bmp.read();
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

      // Deteksi gempa — hanya jika sudah dikalibrasi
      if (mpu.quakeDetected && mpu.isCalibrated && buzzActive) {
        buzzer.play(BUZZ_QUAKE);
        Serial.printf("[GEMPA] Vibration: %.3fg\n", mpu.vibration);
      }
    }

    lastSensorRead = now;
  }

  // Update OLED
  oledDisp.update(
    storage.getCh1Name(),
    storage.getCh2Name(),
    storage.getCh3Name()
  );

  // Akumulasi kWh tiap 10 detik
  if (now - lastKwhAccum >= KWH_ACCUM_INTERVAL) {
    ina.accumulateKwh((now - lastKwhAccum) / 1000.0f);
    lastKwhAccum = now;
  }

  // Simpan kWh tiap 1 jam
  if (now - lastKwhSave >= KWH_SAVE_MS) {
    storage.saveKwh(ina.kwh[0], ina.kwhCh2In, ina.kwhCh2Out, ina.kwh[2]);
    Serial.printf("[kWh saved] CH1:%.3f CH2in:%.3f CH2out:%.3f CH3:%.3f\n",
      ina.kwh[0], ina.kwhCh2In, ina.kwhCh2Out, ina.kwh[2]);
    lastKwhSave = now;
  }

  // Uptime counter
  if (now - lastUptimeSave >= UPTIME_INTERVAL) {
    uptimeSeconds++;
    lastUptimeSave = now;
    if (uptimeSeconds % 600 == 0) storage.saveUptime(uptimeSeconds);
  }

  delay(50);
}