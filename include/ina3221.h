#pragma once

// ============================================================
// SOLAR MONITOR — ina3221.h
// Baca data INA3221 via register I2C langsung
// Shunt 5mΩ, 3 channel
// Moving average filter untuk smoothing data
// ============================================================

#include <Wire.h>
#include "config.h"

// Register INA3221
#define INA_REG_CONFIG      0x00
#define INA_REG_CH1_SHUNT   0x01
#define INA_REG_CH1_BUS     0x02
#define INA_REG_MANUF_ID    0xFE

// Jumlah sampel moving average — lebih besar = lebih smooth tapi lebih lambat respon
#define INA_MA_SAMPLES  8

struct ChannelData {
  float voltage;        // Volt
  float current;        // Ampere (+ atau -)
  float power;          // Watt
};

class INA3221 {
public:
  ChannelData ch[3];
  float efficiency;
  bool  isOnline;

  float kwh[3];
  float kwhCh2In;
  float kwhCh2Out;

  void begin() {
    Wire.beginTransmission(INA_ADDR);
    isOnline = (Wire.endTransmission() == 0);
    if (isOnline) {
      _writeReg(INA_REG_CONFIG, INA_CONFIG_VAL);
      delay(200);
    }
    // Init moving average buffer
    for (int c = 0; c < 3; c++) {
      for (int s = 0; s < INA_MA_SAMPLES; s++) {
        _voltBuf[c][s] = 0.0f;
        _currBuf[c][s] = 0.0f;
      }
    }
  }

  void setShunt(float ohms) {
    if (ohms <= 0.0f) return;
    _shuntOhms = ohms;
    if (isOnline) {
      _writeReg(INA_REG_CONFIG, INA_CONFIG_VAL);
      delay(100);
    }
    Serial.printf("[INA3221] Shunt diperbarui: %.6f Ω (%.3f mΩ)\n", ohms, ohms * 1000.0f);
  }

  float getShunt() { return _shuntOhms; }

  void read() {
    if (!isOnline) return;
    for (int c = 0; c < 3; c++) {
      int16_t rawV = (int16_t)_readReg(INA_REG_CH1_BUS   + c * 2);
      int16_t rawI = (int16_t)_readReg(INA_REG_CH1_SHUNT + c * 2);

      float v = (rawV >> 3) * INA_BUS_LSB;
      float i = ((rawI >> 3) * INA_SHUNT_LSB) / _shuntOhms;

      // Simpan ke moving average buffer
      _voltBuf[c][_maIdx] = v;
      _currBuf[c][_maIdx] = i;

      // Hitung rata-rata
      float sumV = 0, sumI = 0;
      for (int s = 0; s < INA_MA_SAMPLES; s++) {
        sumV += _voltBuf[c][s];
        sumI += _currBuf[c][s];
      }
      ch[c].voltage = sumV / INA_MA_SAMPLES;
      ch[c].current = sumI / INA_MA_SAMPLES;
      ch[c].power   = ch[c].voltage * ch[c].current;
    }

    // Maju index buffer (circular)
    _maIdx = (_maIdx + 1) % INA_MA_SAMPLES;
    _maSamples = min(_maSamples + 1, INA_MA_SAMPLES);

    float useful = (ch[1].current > 0 ? ch[1].power : 0) + ch[2].power;
    efficiency   = (ch[0].power > 0.5f)
                   ? min(useful / ch[0].power * 100.0f, 100.0f)
                   : 0.0f;
  }

  // Akumulasi kWh — panggil tiap interval
  // Menggunakan rata-rata power dari semua sampel sejak interval terakhir
  // Akumulasi Wh — dipanggil tiap 10 detik dari main.cpp
  void accumulateKwh(float intervalSec) {
    // Guard: maksimal 15 detik per interval
    if (intervalSec <= 0.0f || intervalSec > 15.0f) return;

    // Faktor konversi: Detik ke Jam (dibagi 3600).
    // Karena power dalam Watt, maka: Watt * Jam = Watt-hour (Wh)
    float factorWh = intervalSec / 3600.0f;

    // Batas Spike disesuaikan untuk sistem 20Wp. 
    // Jika sensor tiba-tiba membaca di atas 50W, abaikan (dianggap error/spike).
    float maxPowerLimitW = 5000.0f; 
    float maxDeltaWh = maxPowerLimitW * factorWh;

    // CH1 Panel — hanya akumulasi kalau ada daya masuk
    if (ch[0].power > 0.01f) {
      kwh[0] += ch[0].power * factorWh;
    }

    // CH2 Baterai — pisah masuk & keluar
    if (ch[1].current > 0.05f) {
      kwhCh2In += ch[1].power * factorWh;
    }
    
    if (ch[1].current < -0.05f) {
      float delta = fabsf(ch[1].power) * factorWh;
      if (delta > maxDeltaWh) {
        Serial.printf("[Wh] SPIKE CH2OUT! delta=%.6fWh P=%.3fW I=%.3fA t=%.1fs\n",
          delta, ch[1].power, ch[1].current, intervalSec);
      } else {
        kwhCh2Out += delta;
      }
    }

    // CH3 Beban
    if (ch[2].power > 0.01f) {
      float delta3 = ch[2].power * factorWh;
      if (delta3 > maxDeltaWh) {
        Serial.printf("[Wh] SPIKE CH3! delta=%.6fWh P=%.3fW t=%.1fs\n",
          delta3, ch[2].power, intervalSec);
      } else {
        kwh[2] += delta3;
      }

    //String ts = ntpMgr.synced ? ntpMgr.getTimeStr() : String("");
    //logger.addf(LOG_KWH, ts.length() ? ts.c_str() : nullptr,
    //  "Save — CH1:%.3fwh CH2in:%.3fwh CH2out:%.3fwh CH3:%.3fwh",
    //  kwh[0], kwhCh2In, kwhCh2Out, kwh[2]);

      
    }
  }

  void loadKwh(float ch1, float ch2in, float ch2out, float ch3) {
    kwh[0]    = ch1;
    kwhCh2In  = ch2in;
    kwhCh2Out = ch2out;
    kwh[2]    = ch3;
  }

  void resetKwh() {
    kwh[0] = kwh[2] = kwhCh2In = kwhCh2Out = 0.0f;
  }

  // Status baterai CH2
  const char* battStatus() {
    if      (ch[1].current >  0.05f) return "CHARGING";
    else if (ch[1].current < -0.05f) return "DISCHARG";
    else                              return "IDLE";
  }

  // State of Charge estimasi dari tegangan
  int getSOC() {
    float v = ch[1].voltage;
    if      (v >= 4.20f) return 100;
    else if (v >= 4.10f) return 90;
    else if (v >= 4.00f) return 75;
    else if (v >= 3.90f) return 60;
    else if (v >= 3.80f) return 45;
    else if (v >= 3.75f) return 30;
    else if (v >= 3.70f) return 20;
    else if (v >= 3.65f) return 10;
    else                  return 5;
  }

  String socBar() {
    int filled = map(getSOC(), 0, 100, 0, 12);
    String bar = "[";
    for (int x = 0; x < 12; x++) bar += (x < filled) ? "#" : "-";
    bar += "]";
    return bar;
  }

private:
  float _shuntOhms = INA_SHUNT_OHMS_DEFAULT;

  // Moving average buffer
  float _voltBuf[3][INA_MA_SAMPLES] = {};
  float _currBuf[3][INA_MA_SAMPLES] = {};
  int   _maIdx     = 0;
  int   _maSamples = 0;

  uint16_t _readReg(uint8_t reg) {
    Wire.beginTransmission(INA_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)INA_ADDR, (uint8_t)2);
    return ((uint16_t)Wire.read() << 8) | Wire.read();
  }

  void _writeReg(uint8_t reg, uint16_t val) {
    Wire.beginTransmission(INA_ADDR);
    Wire.write(reg);
    Wire.write((val >> 8) & 0xFF);
    Wire.write(val & 0xFF);
    Wire.endTransmission();
  }
};

INA3221 ina;