@echo off
chcp 65001 >nul
title Solar Monitor - Upload ke GitHub

echo ============================================
echo   Solar Monitor - Upload ke GitHub
echo   Repository: britamax/esp32-solar-monitor
echo ============================================
echo.

:: Cek Git terinstall
git --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git belum terinstall!
    echo.
    echo Silakan download dan install Git dulu:
    echo https://git-scm.com/download/win
    echo.
    echo Setelah install Git, jalankan script ini lagi.
    pause
    exit /b 1
)

echo [OK] Git ditemukan
echo.

:: Cek apakah sudah ada repo git di folder ini
if exist ".git" (
    echo [INFO] Folder ini sudah ada Git repository
    echo        Melanjutkan proses push...
    goto :push
)

:: Inisialisasi git repo baru
echo [1/5] Inisialisasi Git repository...
git init
if errorlevel 1 (
    echo [ERROR] Gagal init git!
    pause
    exit /b 1
)
echo [OK] Git repo dibuat
echo.

:: Buat file .gitignore
echo [2/5] Membuat .gitignore...
(
echo # PlatformIO
echo .pio/
echo .pioenvs/
echo .piolibdeps/
echo .clang_complete
echo .gcc-flags.json
echo.
echo # VSCode
echo .vscode/
echo *.code-workspace
echo.
echo # Build output
echo build/
echo dist/
echo.
echo # Windows
echo Thumbs.db
echo desktop.ini
echo.
echo # Credentials - JANGAN di-upload
echo secrets.h
echo credentials.h
) > .gitignore
echo [OK] .gitignore dibuat
echo.

:: Buat README.md
echo [3/5] Membuat README.md...
(
echo # ESP32 Solar Monitor
echo.
echo Sistem monitoring panel surya berbasis ESP32 LOLIN32 Lite.
echo.
echo ## Hardware
echo - ESP32 LOLIN32 Lite ^(USB-C, 4MB^)
echo - INA3221 3-Channel Current Sensor ^(Shunt 5mΩ^)
echo - MPU6050 Gyroscope/Accelerometer
echo - BMP280 Pressure/Temperature
echo - OLED 128x32 I2C
echo - Active Buzzer
echo.
echo ## Pin Config
echo ^| Fungsi ^| GPIO ^|
echo ^|-----^|-----^|
echo ^| SDA ^| 13 ^|
echo ^| SCL ^| 12 ^|
echo ^| Buzzer ^| 27 ^|
echo.
echo ## Channel INA3221
echo - CH1 = Panel Surya → SCC
echo - CH2 = SCC → Baterai ^(+charge / -discharge^)
echo - CH3 = Output → Beban
echo.
echo ## Fitur
echo - Monitoring tegangan, arus, daya realtime
echo - Akumulasi kWh per channel
echo - Estimasi State of Charge baterai
echo - Tampilan OLED 4 halaman auto-scroll
echo - WiFi Manager dengan AP Setup Mode
echo - Web dashboard lokal
echo - MQTT support
echo - Data tersimpan saat restart ^(NVS^)
echo.
echo ## Setup
echo 1. Upload firmware via PlatformIO
echo 2. ESP32 akan membuat hotspot **WifiMonitor-Setup**
echo 3. Konek HP ke hotspot tersebut
echo 4. Buka browser ke **192.168.4.1**
echo 5. Pilih jaringan WiFi dan simpan
echo 6. ESP32 restart dan konek otomatis
echo.
echo ## Lisensi
echo MIT License
) > README.md
echo [OK] README.md dibuat
echo.

:: Set git config jika belum ada
echo [4/5] Konfigurasi Git...
git config user.name >nul 2>&1
if errorlevel 1 (
    set /p GIT_NAME="Masukkan nama Anda untuk Git: "
    git config user.name "%GIT_NAME%"
)
git config user.email >nul 2>&1
if errorlevel 1 (
    set /p GIT_EMAIL="Masukkan email GitHub Anda: "
    git config user.email "%GIT_EMAIL%"
)
echo [OK] Konfigurasi selesai
echo.

:: Add semua file
echo [5/5] Menambahkan semua file...
git add .
git status
echo.

:: Commit pertama
git commit -m "Initial commit - Solar Monitor Tahap 3"
if errorlevel 1 (
    echo [ERROR] Gagal commit!
    pause
    exit /b 1
)
echo [OK] Commit berhasil
echo.

:push
:: Set branch ke main
git branch -M main

:: Tambah remote origin
git remote remove origin >nul 2>&1
git remote add origin https://github.com/britamax/esp32-solar-monitor.git
echo [OK] Remote origin ditambahkan

echo.
echo ============================================
echo   LANGKAH TERAKHIR - Buat Repository Dulu!
echo ============================================
echo.
echo Sebelum push, buat repository baru di GitHub:
echo.
echo 1. Buka: https://github.com/new
echo 2. Repository name : esp32-solar-monitor
echo 3. Pilih            : Public
echo 4. JANGAN centang   : Add README / .gitignore
echo 5. Klik             : Create repository
echo.
echo Setelah repository dibuat, tekan Enter untuk lanjut push...
pause

echo.
echo [PUSH] Mengupload ke GitHub...
echo Jika muncul popup login GitHub, masukkan credentials Anda.
echo.
git push -u origin main
if errorlevel 1 (
    echo.
    echo [ERROR] Push gagal!
    echo.
    echo Kemungkinan penyebab:
    echo 1. Repository belum dibuat di GitHub
    echo    Buat di: https://github.com/new
    echo.
    echo 2. Belum login GitHub di Git
    echo    Jalankan: git credential-manager configure
    echo    Atau install GitHub CLI: https://cli.github.com
    echo.
    echo 3. Token GitHub expired
    echo    Settings - Developer settings - Personal access tokens
    echo.
    pause
    exit /b 1
)

echo.
echo ============================================
echo   BERHASIL! Repository tersedia di:
echo   https://github.com/britamax/esp32-solar-monitor
echo ============================================
echo.

:: Buka browser ke repository
start https://github.com/britamax/esp32-solar-monitor

pause
