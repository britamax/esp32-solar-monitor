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
  // BUZZER SETTING — per event
  // ============================================================

  // Gempa
  int  getBuzzQuakeOnMs()   { return prefs.getInt("bq_on",  100); }
  int  getBuzzQuakeOffMs()  { return prefs.getInt("bq_off", 200); }
  int  getBuzzQuakeDurSec() { return prefs.getInt("bq_dur", 10);  }
  void setBuzzQuakeOnMs(int v)   { prefs.putInt("bq_on",  v); }
  void setBuzzQuakeOffMs(int v)  { prefs.putInt("bq_off", v); }
  void setBuzzQuakeDurSec(int v) { prefs.putInt("bq_dur", v); }

  // Tegangan rendah
  int  getBuzzVoltOnMs()    { return prefs.getInt("bv_on",  200); }
  int  getBuzzVoltOffMs()   { return prefs.getInt("bv_off", 500); }
  int  getBuzzVoltDurSec()  { return prefs.getInt("bv_dur", 10);  }
  void setBuzzVoltOnMs(int v)    { prefs.putInt("bv_on",  v); }
  void setBuzzVoltOffMs(int v)   { prefs.putInt("bv_off", v); }
  void setBuzzVoltDurSec(int v)  { prefs.putInt("bv_dur", v); }

  // Arus berlebih
  int  getBuzzCurrOnMs()    { return prefs.getInt("bc_on",  100); }
  int  getBuzzCurrOffMs()   { return prefs.getInt("bc_off", 100); }
  int  getBuzzCurrDurSec()  { return prefs.getInt("bc_dur", 10);  }
  void setBuzzCurrOnMs(int v)    { prefs.putInt("bc_on",  v); }
  void setBuzzCurrOffMs(int v)   { prefs.putInt("bc_off", v); }
  void setBuzzCurrDurSec(int v)  { prefs.putInt("bc_dur", v); }

  // ============================================================
  // OLED SETTING
  // ============================================================

  int  getOledPage()       { return prefs.getInt("oled_page", 0); }      // halaman aktif
  bool getOledAutoScroll() { return prefs.getBool("oled_scroll", false); } // auto-scroll
  int  getOledDuration()   { return prefs.getInt("oled_dur", 3000); }    // ms per halaman
  int  getOledBrightness() { return prefs.getInt("oled_bright", 128); }  // 0-255
  bool getOledEnabled()    { return prefs.getBool("oled_on", true); }     // on/off

  void setOledPage(int v)         { prefs.putInt("oled_page",   v); }
  void setOledAutoScroll(bool v)  { prefs.putBool("oled_scroll", v); }
  void setOledDuration(int v)     { prefs.putInt("oled_dur",    v); }
  void setOledBrightness(int v)   { prefs.putInt("oled_bright", v); }
  void setOledEnabled(bool v)     { prefs.putBool("oled_on",    v); }

  // ============================================================
  // THRESHOLD ALARM
  // ============================================================

  float getAlarmVMin()   { return prefs.getFloat("alm_vmin", ALARM_V_MIN_DEFAULT); }
  float getAlarmVMax()   { return prefs.getFloat("alm_vmax", ALARM_V_MAX_DEFAULT); }
  float getAlarmIMax()   { return prefs.getFloat("alm_imax", ALARM_I_MAX_DEFAULT); }
  float getAlarmQuake()  { return prefs.getFloat("alm_quake", ALARM_QUAKE_DEFAULT); }

  void setAlarmVMin(float v)  { prefs.putFloat("alm_vmin",  v); }
  void setAlarmVMax(float v)  { prefs.putFloat("alm_vmax",  v); }
  void setAlarmIMax(float v)  { prefs.putFloat("alm_imax",  v); }
  void setAlarmQuake(float v) { prefs.putFloat("alm_quake", v); }

  float getShuntOhms()       { return prefs.getFloat("shunt_ohms", INA_SHUNT_OHMS_DEFAULT); }
  void  setShuntOhms(float v){ prefs.putFloat("shunt_ohms", v); }

  float getBmpTempOffset()        { return prefs.getFloat("bmp_t_off", 0.0f); }
  void  setBmpTempOffset(float v) { prefs.putFloat("bmp_t_off", v); }

  // ============================================================
  // LOKASI PERANGKAT
  // ============================================================

  String getLocName() { return prefs.getString("loc_name", ""); }
  double getLocLat()  { return prefs.getDouble("loc_lat",  0.0); }
  double getLocLng()  { return prefs.getDouble("loc_lng",  0.0); }
  float  getLocAlt()  { return prefs.getFloat("loc_alt",   0.0f); }
  bool   isLocSet()   { return prefs.getBool("loc_set",    false); }

  void setLocation(String name, double lat, double lng, float alt) {
    prefs.putString("loc_name", name);
    prefs.putDouble("loc_lat",  lat);
    prefs.putDouble("loc_lng",  lng);
    prefs.putFloat("loc_alt",   alt);
    prefs.putBool("loc_set",    true);
  }

  // ============================================================
  // MQTT BROKER A
  // ============================================================

  bool   getMqttAEnabled()  { return prefs.getBool("ma_en",    false); }
  String getMqttAHost()     { return prefs.getString("ma_host", ""); }
  int    getMqttAPort()     { return prefs.getInt("ma_port",    1883); }
  String getMqttAUser()     { return prefs.getString("ma_user", ""); }
  String getMqttAPass()     { return prefs.getString("ma_pass", ""); }
  String getMqttATopic()    { return prefs.getString("ma_topic","solar"); }
  int    getMqttAInterval() { return prefs.getInt("ma_int",    10); }   // detik
  // Seleksi data: bit field — bit0=INA, bit1=BMP, bit2=MPU, bit3=alarm, bit4=lokasi
  int    getMqttAData()     { return prefs.getInt("ma_data",   0b11111); }

  void setMqttA(bool en, String host, int port, String user, String pass,
                String topic, int interval, int data) {
    prefs.putBool("ma_en",     en);
    prefs.putString("ma_host", host);
    prefs.putInt("ma_port",    port);
    prefs.putString("ma_user", user);
    prefs.putString("ma_pass", pass);
    prefs.putString("ma_topic",topic);
    prefs.putInt("ma_int",     interval);
    prefs.putInt("ma_data",    data);
  }

  // ============================================================
  // MQTT BROKER B
  // ============================================================

  bool   getMqttBEnabled()  { return prefs.getBool("mb_en",    false); }
  String getMqttBHost()     { return prefs.getString("mb_host", ""); }
  int    getMqttBPort()     { return prefs.getInt("mb_port",    1883); }
  String getMqttBUser()     { return prefs.getString("mb_user", ""); }
  String getMqttBPass()     { return prefs.getString("mb_pass", ""); }
  String getMqttBTopic()    { return prefs.getString("mb_topic","solar"); }
  int    getMqttBInterval() { return prefs.getInt("mb_int",    10); }
  int    getMqttBData()     { return prefs.getInt("mb_data",   0b11111); }

  void setMqttB(bool en, String host, int port, String user, String pass,
                String topic, int interval, int data) {
    prefs.putBool("mb_en",     en);
    prefs.putString("mb_host", host);
    prefs.putInt("mb_port",    port);
    prefs.putString("mb_user", user);
    prefs.putString("mb_pass", pass);
    prefs.putString("mb_topic",topic);
    prefs.putInt("mb_int",     interval);
    prefs.putInt("mb_data",    data);
  }

  // ============================================================
  // AUTH — Username & Password
  // Default: admin / 123123
  // ============================================================

  String getAuthUser() { return prefs.getString("auth_user", "admin"); }
  String getAuthPass() { return prefs.getString("auth_pass", "123123"); }

  void setAuthUser(const String& u) { prefs.putString("auth_user", u); }
  void setAuthPass(const String& p) { prefs.putString("auth_pass", p); }

  void resetAuthToDefault() {
    prefs.remove("auth_user");
    prefs.remove("auth_pass");
    Serial.println("[NVS] Auth reset to default (admin/123123)");
  }

  // ============================================================
  // NTP & TIMEZONE
  // ============================================================
  int    getNtpOffset()     { return prefs.getInt("ntp_offset", 7); }      // default WIB +7
  String getNtpServer()     { return prefs.getString("ntp_server", "pool.ntp.org"); }
  bool   getNtpEnabled()    { return prefs.getBool("ntp_en", true); }

  void   setNtpOffset(int v)          { prefs.putInt("ntp_offset", v); }
  void   setNtpServer(const String& s){ prefs.putString("ntp_server", s); }
  void   setNtpEnabled(bool v)        { prefs.putBool("ntp_en", v); }

  // ============================================================
  // FACTORY RESET
  // ============================================================

  void factoryReset() {
    prefs.clear();
  }

  // Reset WiFi saja — kWh, uptime, nama channel tetap
  void clearWifiSettings() {
    prefs.remove(NVS_WIFI_SSID);
    prefs.remove(NVS_WIFI_PASS);
    prefs.remove(NVS_MODE);
    prefs.remove(NVS_SETUP_DONE);
    prefs.remove(NVS_DEVICE_NAME);
    prefs.remove("ip_mode");
    prefs.remove("ip_addr");
    prefs.remove("ip_gw");
    prefs.remove("ip_sn");
    prefs.remove("ip_dns");
    Serial.println("[NVS] WiFi settings cleared");
  }
};

// Instance global
Storage storage;
