#pragma once

// ============================================================
// SOLAR MONITOR — storage.h
// Simpan & baca setting dari NVS (Preferences)
// Data tidak hilang saat restart
// ============================================================

#include <Preferences.h>
#include "config.h"

class Storage {
private:
  Preferences prefs;

public:

  // --- Buka koneksi NVS ---
  void begin() {
    prefs.begin(NVS_NAMESPACE, false);
  }

  void end() {
    prefs.end();
  }

  // ============================================================
  // SETUP STATUS
  // ============================================================

  bool isSetupDone() {
    return prefs.getBool(NVS_SETUP_DONE, false);
  }

  void setSetupDone(bool val) {
    prefs.putBool(NVS_SETUP_DONE, val);
  }

  // ============================================================
  // MODE OPERASI
  // ============================================================

  String getMode() {
    return prefs.getString(NVS_MODE, MODE_WIFI);
  }

  void setMode(String mode) {
    prefs.putString(NVS_MODE, mode);
  }

  // ============================================================
  // DEVICE NAME
  // ============================================================

  String getDeviceName() {
    return prefs.getString(NVS_DEVICE_NAME, DEVICE_NAME_DEFAULT);
  }

  void setDeviceName(String name) {
    prefs.putString(NVS_DEVICE_NAME, name);
  }

  // ============================================================
  // WIFI CREDENTIALS
  // ============================================================

  String getWifiSSID() {
    return prefs.getString(NVS_WIFI_SSID, "");
  }

  String getWifiPass() {
    return prefs.getString(NVS_WIFI_PASS, "");
  }

  void setWifiCredentials(String ssid, String pass) {
    prefs.putString(NVS_WIFI_SSID, ssid);
    prefs.putString(NVS_WIFI_PASS, pass);
  }

  // ============================================================
  // IP STATIC SETTINGS
  // ============================================================

  String getIPMode()   { return prefs.getString("ip_mode", "dhcp"); }
  String getStaticIP() { return prefs.getString("ip_addr", ""); }
  String getGateway()  { return prefs.getString("ip_gw",   ""); }
  String getSubnet()   { return prefs.getString("ip_sn",   "255.255.255.0"); }
  String getDNS()      { return prefs.getString("ip_dns",  "8.8.8.8"); }

  void setIPMode(String mode) { prefs.putString("ip_mode", mode); }

  void setStaticIP(String ip, String gw, String sn, String dns) {
    prefs.putString("ip_addr", ip);
    prefs.putString("ip_gw",   gw);
    prefs.putString("ip_sn",   sn);
    prefs.putString("ip_dns",  dns);
  }

  bool hasWifiCredentials() {
    return prefs.getString(NVS_WIFI_SSID, "").length() > 0;
  }

  // ============================================================
  // NAMA CHANNEL
  // ============================================================

  String getCh1Name() { return prefs.getString(NVS_CH1_NAME, CH1_NAME_DEFAULT); }
  String getCh2Name() { return prefs.getString(NVS_CH2_NAME, CH2_NAME_DEFAULT); }
  String getCh3Name() { return prefs.getString(NVS_CH3_NAME, CH3_NAME_DEFAULT); }

  void setCh1Name(String n) { prefs.putString(NVS_CH1_NAME, n); }
  void setCh2Name(String n) { prefs.putString(NVS_CH2_NAME, n); }
  void setCh3Name(String n) { prefs.putString(NVS_CH3_NAME, n); }

  // ============================================================
  // AKUMULASI kWh — Simpan per jam
  // ============================================================

  float getKwhCh1()    { return prefs.getFloat(NVS_KWH_CH1,     0.0f); }
  float getKwhCh2In()  { return prefs.getFloat(NVS_KWH_CH2_IN,  0.0f); }
  float getKwhCh2Out() { return prefs.getFloat(NVS_KWH_CH2_OUT, 0.0f); }
  float getKwhCh3()    { return prefs.getFloat(NVS_KWH_CH3,     0.0f); }

  void saveKwh(float ch1, float ch2in, float ch2out, float ch3) {
    prefs.putFloat(NVS_KWH_CH1,     ch1);
    prefs.putFloat(NVS_KWH_CH2_IN,  ch2in);
    prefs.putFloat(NVS_KWH_CH2_OUT, ch2out);
    prefs.putFloat(NVS_KWH_CH3,     ch3);
  }

  void resetKwh() {
    prefs.putFloat(NVS_KWH_CH1,     0.0f);
    prefs.putFloat(NVS_KWH_CH2_IN,  0.0f);
    prefs.putFloat(NVS_KWH_CH2_OUT, 0.0f);
    prefs.putFloat(NVS_KWH_CH3,     0.0f);
  }

  // ============================================================
  // UPTIME KUMULATIF
  // ============================================================

  unsigned long getUptime() {
    return prefs.getULong(NVS_UPTIME, 0);
  }

  void saveUptime(unsigned long seconds) {
    prefs.putULong(NVS_UPTIME, seconds);
  }

  // ============================================================
  // ON/OFF SENSOR
  // ============================================================

  bool isBmpEnabled()  { return prefs.getBool("sen_bmp",  SENSOR_BMP_DEFAULT);  }
  bool isMpuEnabled()  { return prefs.getBool("sen_mpu",  SENSOR_MPU_DEFAULT);  }
  bool isBuzzEnabled() { return prefs.getBool("sen_buzz", SENSOR_BUZZ_DEFAULT); }

  void setBmpEnabled(bool v)  { prefs.putBool("sen_bmp",  v); }
  void setMpuEnabled(bool v)  { prefs.putBool("sen_mpu",  v); }
  void setBuzzEnabled(bool v) { prefs.putBool("sen_buzz", v); }

  // ============================================================
  // KALIBRASI MPU6050
  // ============================================================

  void saveMpuCalib(float ax, float ay, float az,
                    float gx, float gy, float gz) {
    prefs.putFloat("mpu_ax", ax); prefs.putFloat("mpu_ay", ay);
    prefs.putFloat("mpu_az", az); prefs.putFloat("mpu_gx", gx);
    prefs.putFloat("mpu_gy", gy); prefs.putFloat("mpu_gz", gz);
    prefs.putBool("mpu_cal", true);
  }

  bool isMpuCalibrated() { return prefs.getBool("mpu_cal", false); }

  float getMpuCalAX() { return prefs.getFloat("mpu_ax", 0); }
  float getMpuCalAY() { return prefs.getFloat("mpu_ay", 0); }
  float getMpuCalAZ() { return prefs.getFloat("mpu_az", 0); }
  float getMpuCalGX() { return prefs.getFloat("mpu_gx", 0); }
  float getMpuCalGY() { return prefs.getFloat("mpu_gy", 0); }
  float getMpuCalGZ() { return prefs.getFloat("mpu_gz", 0); }

  void resetMpuCalib() {
    prefs.putBool("mpu_cal", false);
    prefs.putFloat("mpu_ax", 0); prefs.putFloat("mpu_ay", 0);
    prefs.putFloat("mpu_az", 0); prefs.putFloat("mpu_gx", 0);
    prefs.putFloat("mpu_gy", 0); prefs.putFloat("mpu_gz", 0);
  }

  // ============================================================
  // FACTORY RESET
  // ============================================================

  void factoryReset() {
    prefs.clear();
  }
};

// Instance global
Storage storage;