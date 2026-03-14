#pragma once

// ============================================================
// SOLAR MONITOR — mpu6050.h
// Driver MPU6050 via register I2C langsung
// Accelerometer, Gyroscope, Suhu internal
// Deteksi gempa dari delta accelerasi mendadak
// ============================================================

#include <Wire.h>
#include "config.h"

// MPU6050 Register
#define MPU_ADDR            0x68    // AD0=GND, gunakan 0x69 jika AD0=VCC
#define MPU_REG_PWR_MGMT    0x6B
#define MPU_REG_ACCEL_XOUT  0x3B   // 6 byte: X,Y,Z high+low
#define MPU_REG_TEMP_OUT    0x41   // 2 byte
#define MPU_REG_GYRO_XOUT   0x43   // 6 byte: X,Y,Z high+low
#define MPU_REG_ACCEL_CFG   0x1C   // ±8g
#define MPU_REG_GYRO_CFG    0x1B   // ±500°/s
#define MPU_REG_DLPF        0x1A   // Digital Low Pass Filter

// Skala
#define ACCEL_SCALE         4096.0f  // LSB/g untuk ±8g
#define GYRO_SCALE          65.5f    // LSB/°/s untuk ±500°/s

class MPU6050Sensor {
public:
  // Data mentah setelah kalibrasi
  float accX, accY, accZ;   // m/s²
  float gyroX, gyroY, gyroZ; // °/s
  float tempC;               // °C

  // Deteksi gempa
  float vibration;           // magnitude delta accelerasi
  bool  quakeDetected;
  float quakeThreshold;      // threshold getaran (default 0.5g)

  // Status kalibrasi
  bool  isCalibrated;
  bool  isOnline;

  // Offset kalibrasi
  float offAccX, offAccY, offAccZ;
  float offGyroX, offGyroY, offGyroZ;

  void begin() {
    isOnline     = false;
    isCalibrated = false;
    quakeThreshold = MPU_QUAKE_THRESHOLD;
    quakeDetected  = false;
    vibration      = 0;

    // Reset offset
    offAccX = offAccY = offAccZ = 0;
    offGyroX = offGyroY = offGyroZ = 0;

    // Cek keberadaan sensor
    Wire.beginTransmission(MPU_ADDR);
    if (Wire.endTransmission() != 0) return;

    // Wake up (keluarkan dari sleep mode)
    _writeByte(MPU_REG_PWR_MGMT, 0x00);
    delay(100);

    // Konfigurasi
    _writeByte(MPU_REG_ACCEL_CFG, 0x10); // ±8g
    _writeByte(MPU_REG_GYRO_CFG,  0x08); // ±500°/s
    _writeByte(MPU_REG_DLPF,      0x03); // DLPF 44Hz

    delay(100);
    isOnline = true;
  }

  void read() {
    if (!isOnline) return;

    // Baca accelerometer (6 byte)
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MPU_REG_ACCEL_XOUT);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)14); // Accel+Temp+Gyro sekaligus

    int16_t rawAX = (Wire.read() << 8) | Wire.read();
    int16_t rawAY = (Wire.read() << 8) | Wire.read();
    int16_t rawAZ = (Wire.read() << 8) | Wire.read();
    int16_t rawT  = (Wire.read() << 8) | Wire.read();
    int16_t rawGX = (Wire.read() << 8) | Wire.read();
    int16_t rawGY = (Wire.read() << 8) | Wire.read();
    int16_t rawGZ = (Wire.read() << 8) | Wire.read();

    // Konversi + kurangi offset kalibrasi
    float aX = (rawAX / ACCEL_SCALE) - offAccX;
    float aY = (rawAY / ACCEL_SCALE) - offAccY;
    float aZ = (rawAZ / ACCEL_SCALE) - offAccZ;

    accX  = aX * 9.81f; // g → m/s²
    accY  = aY * 9.81f;
    accZ  = aZ * 9.81f;

    gyroX = (rawGX / GYRO_SCALE) - offGyroX;
    gyroY = (rawGY / GYRO_SCALE) - offGyroY;
    gyroZ = (rawGZ / GYRO_SCALE) - offGyroZ;

    tempC = (rawT / 340.0f) + 36.53f;

    // Hitung magnitude vibration (delta dari 1g gravitasi)
    float totalG = sqrt(aX*aX + aY*aY + aZ*aZ);
    vibration    = abs(totalG - 1.0f); // delta dari 1g

    // Deteksi gempa
    quakeDetected = (vibration > quakeThreshold);
  }

  // Kalibrasi — panggil saat sensor DIAM
  // Ambil rata-rata N sampel sebagai offset
  void calibrate(int samples = 500) {
    if (!isOnline) return;

    float sumAX=0, sumAY=0, sumAZ=0;
    float sumGX=0, sumGY=0, sumGZ=0;

    for (int i = 0; i < samples; i++) {
      Wire.beginTransmission(MPU_ADDR);
      Wire.write(MPU_REG_ACCEL_XOUT);
      Wire.endTransmission(false);
      Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)14);

      int16_t rAX = (Wire.read() << 8) | Wire.read();
      int16_t rAY = (Wire.read() << 8) | Wire.read();
      int16_t rAZ = (Wire.read() << 8) | Wire.read();
      Wire.read(); Wire.read(); // skip temp
      int16_t rGX = (Wire.read() << 8) | Wire.read();
      int16_t rGY = (Wire.read() << 8) | Wire.read();
      int16_t rGZ = (Wire.read() << 8) | Wire.read();

      sumAX += rAX / ACCEL_SCALE;
      sumAY += rAY / ACCEL_SCALE;
      sumAZ += rAZ / ACCEL_SCALE;
      sumGX += rGX / GYRO_SCALE;
      sumGY += rGY / GYRO_SCALE;
      sumGZ += rGZ / GYRO_SCALE;

      delay(3);
    }

    offAccX  = sumAX / samples;
    offAccY  = sumAY / samples;
    offAccZ  = (sumAZ / samples) - 1.0f; // Z harus 1g saat datar
    offGyroX = sumGX / samples;
    offGyroY = sumGY / samples;
    offGyroZ = sumGZ / samples;

    isCalibrated = true;
  }

  void loadCalibration(float ax, float ay, float az,
                       float gx, float gy, float gz) {
    offAccX  = ax; offAccY  = ay; offAccZ  = az;
    offGyroX = gx; offGyroY = gy; offGyroZ = gz;
    isCalibrated = (ax != 0 || ay != 0 || az != 0);
  }

  void resetCalibration() {
    offAccX = offAccY = offAccZ = 0;
    offGyroX = offGyroY = offGyroZ = 0;
    isCalibrated = false;
  }

  // Status getaran untuk OLED
  const char* vibrationStatus() {
    if      (vibration < 0.05f) return "AMAN";
    else if (vibration < 0.2f)  return "GETAR";
    else if (vibration < 0.5f)  return "SEDANG";
    else if (vibration < 1.0f)  return "KUAT";
    else                         return "GEMPA!";
  }

private:
  void _writeByte(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
  }
};

MPU6050Sensor mpu;
