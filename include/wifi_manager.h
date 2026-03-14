#pragma once

// ============================================================
// SOLAR MONITOR — wifi_manager.h
// Struktur SAMA dengan v2.0.0 yang sudah terbukti jalan
// Tambahan: route /api/data dan / untuk dashboard
// ============================================================

#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "config.h"
#include "storage.h"
#include "session_manager.h"
#include "log_manager.h"
#include "ntp_manager.h"
#include "oled.h"
#include "web_dashboard.h"
#include "web_setting.h"
#include "web_login.h"
#include "web_system.h"
#include "web_compressed.h"

// Helper: kirim response gzip
static void sendGzip(AsyncWebServerRequest* req, const uint8_t* data, size_t len, const char* mime) {
  AsyncWebServerResponse* res = req->beginResponse(200, mime, data, len);
  res->addHeader("Content-Encoding", "gzip");
  res->addHeader("Cache-Control", "no-cache");
  req->send(res);
}

DNSServer dnsServer;
AsyncWebServer server(80);

// ============================================================
// HTML WIZARD SETUP
// ============================================================
const char WIZARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Solar Monitor - Setup</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:Arial,sans-serif;background:#0f1923;color:#e0e0e0;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:16px}
.card{background:#1a2535;border-radius:12px;padding:24px;width:100%;max-width:420px;box-shadow:0 4px 24px rgba(0,0,0,.4)}
.logo{text-align:center;margin-bottom:20px}
.logo h1{font-size:20px;color:#f59e0b}
.logo p{font-size:12px;color:#94a3b8;margin-top:4px}
h2{font-size:14px;color:#f59e0b;margin:16px 0 10px;padding-bottom:6px;border-bottom:1px solid #2d3f55}
label{display:block;font-size:12px;color:#94a3b8;margin-bottom:3px;margin-top:10px}
input[type=text],input[type=password],select{width:100%;padding:9px 11px;background:#0f1923;border:1px solid #2d3f55;border-radius:7px;color:#e0e0e0;font-size:13px;outline:none}
input:focus,select:focus{border-color:#f59e0b}
.row{display:flex;gap:8px}.row>div{flex:1}
.mode-group{display:flex;gap:8px;margin-top:4px}
.mode-btn{flex:1;padding:9px;border:2px solid #2d3f55;border-radius:7px;background:#0f1923;color:#94a3b8;font-size:12px;cursor:pointer;text-align:center;transition:all .2s}
.mode-btn.active{border-color:#f59e0b;color:#f59e0b;background:#1f2d3d}
.scan-list{margin-top:6px;max-height:150px;overflow-y:auto;border:1px solid #2d3f55;border-radius:7px;display:none}
.scan-item{padding:9px 11px;border-bottom:1px solid #2d3f55;cursor:pointer;display:flex;justify-content:space-between;font-size:12px;transition:background .15s}
.scan-item:last-child{border-bottom:none}
.scan-item:hover,.scan-item.selected{background:#1f2d3d;color:#f59e0b}
.signal{font-size:11px;color:#64748b}
.btn{width:100%;padding:11px;border:none;border-radius:7px;font-size:14px;font-weight:bold;cursor:pointer;margin-top:14px;transition:opacity .2s}
.btn:hover{opacity:.85}
.btn-primary{background:#f59e0b;color:#0f1923}
.btn-sm{background:#2d3f55;color:#e0e0e0;font-size:12px;padding:7px;margin-top:6px}
.status-box{margin-top:12px;padding:10px;border-radius:7px;font-size:12px;display:none}
.ok{background:#064e3b;color:#6ee7b7}
.err{background:#450a0a;color:#fca5a5}
.info{background:#1e3a5f;color:#93c5fd}
.spinner{display:inline-block;width:12px;height:12px;border:2px solid #f59e0b;border-top-color:transparent;border-radius:50%;animation:spin .8s linear infinite;margin-right:5px;vertical-align:middle}
@keyframes spin{to{transform:rotate(360deg)}}
.toggle-pass{display:flex;align-items:center;gap:5px;margin-top:5px;font-size:11px;color:#64748b;cursor:pointer}
.ip-section{display:none}
</style>
</head>
<body>
<div class="card">
  <div class="logo">
    <h1>&#9728; Solar Monitor</h1>
    <p>Pengaturan Awal Perangkat</p>
  </div>
  <h2>Nama Perangkat</h2>
  <input type="text" id="devname" placeholder="Monitoring-Daya" maxlength="24" value="Monitoring-Daya">
  <h2>Mode Koneksi</h2>
  <div class="mode-group">
    <div class="mode-btn active" id="btn-wifi" onclick="setMode('wifi')">&#128246; WiFi Router</div>
    <div class="mode-btn" id="btn-ap" onclick="setMode('ap')">&#128225; Local AP</div>
  </div>
  <div id="section-wifi">
    <label>Jaringan WiFi</label>
    <button class="btn btn-sm" onclick="scanWifi()" id="btn-scan">&#128268; Scan Jaringan</button>
    <div class="scan-list" id="scan-list"></div>
    <label>Nama WiFi (SSID)</label>
    <input type="text" id="ssid" placeholder="Nama jaringan">
    <label>Kata Sandi</label>
    <input type="password" id="pass" placeholder="Kata sandi WiFi">
    <label class="toggle-pass"><input type="checkbox" onchange="this.closest('label').previousElementSibling.type=this.checked?'text':'password'"> Tampilkan kata sandi</label>
    <h2>Pengaturan IP</h2>
    <div class="mode-group">
      <div class="mode-btn active" id="btn-dhcp" onclick="setIP('dhcp')">DHCP (Otomatis)</div>
      <div class="mode-btn" id="btn-static" onclick="setIP('static')">IP Statis</div>
    </div>
    <div class="ip-section" id="ip-section">
      <div class="row">
        <div><label>IP Address</label><input type="text" id="ip" placeholder="192.168.1.100"></div>
        <div><label>Gateway</label><input type="text" id="gw" placeholder="192.168.1.1"></div>
      </div>
      <div class="row">
        <div><label>Subnet</label><input type="text" id="sn" placeholder="255.255.255.0"></div>
        <div><label>DNS</label><input type="text" id="dns" placeholder="8.8.8.8"></div>
      </div>
    </div>
  </div>
  <div id="section-ap" style="display:none">
    <div class="status-box info" style="display:block;margin-top:10px">
      <b>Mode Local AP</b><br>ESP32 menjadi hotspot sendiri.<br>
      Konek ke <b>WifiMonitor-Setup</b> lalu buka <b>192.168.4.1</b>
    </div>
  </div>
  <div id="status-box" class="status-box"></div>
  <button class="btn btn-primary" onclick="simpan()" id="btn-simpan">Simpan &amp; Mulai</button>
</div>
<script>
let selMode='wifi',selIP='dhcp';
function setMode(m){
  selMode=m;
  ['wifi','ap'].forEach(x=>document.getElementById('btn-'+x).className='mode-btn'+(m===x?' active':''));
  document.getElementById('section-wifi').style.display=m==='wifi'?'block':'none';
  document.getElementById('section-ap').style.display=m==='ap'?'block':'none';
}
function setIP(m){
  selIP=m;
  ['dhcp','static'].forEach(x=>document.getElementById('btn-'+x).className='mode-btn'+(m===x?' active':''));
  document.getElementById('ip-section').style.display=m==='static'?'block':'none';
}
function scanWifi(){
  const btn=document.getElementById('btn-scan');
  btn.innerHTML='<span class="spinner"></span>Memindai...';btn.disabled=true;
  fetch('/api/scan').then(r=>r.json()).then(d=>{
    const list=document.getElementById('scan-list');
    list.style.display='block';list.innerHTML='';
    if(!d.networks.length){list.innerHTML='<div class="scan-item">Tidak ada jaringan</div>';return;}
    d.networks.forEach(n=>{
      const div=document.createElement('div');div.className='scan-item';
      div.innerHTML='<span>'+n.ssid+'</span><span class="signal">'+n.rssi+'dBm</span>';
      div.onclick=()=>{document.getElementById('ssid').value=n.ssid;
        list.querySelectorAll('.scan-item').forEach(i=>i.classList.remove('selected'));div.classList.add('selected');};
      list.appendChild(div);
    });
    btn.innerHTML='&#128268; Scan Ulang';btn.disabled=false;
  }).catch(()=>{btn.innerHTML='&#128268; Scan Jaringan';btn.disabled=false;});
}
function showSt(msg,cls){const b=document.getElementById('status-box');b.className='status-box '+cls;b.innerHTML=msg;b.style.display='block';}
function simpan(){
  const btn=document.getElementById('btn-simpan');
  const devname=document.getElementById('devname').value.trim()||'Monitoring-Daya';
  if(selMode==='ap'){
    btn.disabled=true;btn.innerHTML='<span class="spinner"></span>Menyimpan...';
    fetch('/api/save',{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({mode:'ap',devname,ssid:'',password:'',ip_mode:'dhcp'})})
    .then(r=>r.json()).then(()=>{showSt('&#10003; Tersimpan! Restart dalam 3 detik.','ok');
      btn.innerHTML='Simpan & Mulai';btn.disabled=false;});return;
  }
  const ssid=document.getElementById('ssid').value.trim();
  const pass=document.getElementById('pass').value;
  if(!ssid){showSt('&#9888; Nama WiFi tidak boleh kosong.','err');return;}
  const payload={mode:'wifi',devname,ssid,password:pass,ip_mode:selIP};
  if(selIP==='static'){
    payload.ip=document.getElementById('ip').value.trim();
    payload.gw=document.getElementById('gw').value.trim();
    payload.sn=document.getElementById('sn').value.trim();
    payload.dns=document.getElementById('dns').value.trim();
    if(!payload.ip||!payload.gw){showSt('&#9888; IP dan Gateway wajib diisi.','err');return;}
  }
  btn.disabled=true;btn.innerHTML='<span class="spinner"></span>Menyimpan...';
  fetch('/api/save',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(payload)})
  .then(r=>r.json()).then(d=>{
    if(d.success)showSt('&#10003; Tersimpan! Konek ke <b>'+ssid+'</b> dan restart.','ok');
    else showSt('&#10007; Gagal menyimpan.','err');
    btn.innerHTML='Simpan & Mulai';btn.disabled=false;
  }).catch(()=>{showSt('&#10007; Koneksi terputus.','err');btn.innerHTML='Simpan & Mulai';btn.disabled=false;});
}
</script>
</body>
</html>
)rawliteral";

// ============================================================
// WIFI MANAGER CLASS
// ============================================================
class WifiManager {
public:
  bool   isConnected;
  bool   isAPMode;
  bool   isSetupMode;
  String ipAddress;

  // ============================================================
  // BEGIN — struktur SAMA dengan v2.0.0 yang jalan
  // ============================================================
  void begin() {
    isConnected = false;
    isAPMode    = false;
    isSetupMode = false;

    String mode = storage.getMode();

    // Mode AP permanen
    if (mode == MODE_AP) {
      _startLocalAP();
      _startServer();
      return;
    }

    // Belum ada credentials → Setup AP
    if (!storage.hasWifiCredentials()) {
      _startSetupAP();
      return;
    }

    // Ada credentials → coba konek WiFi
    _connectWifi(storage.getWifiSSID(), storage.getWifiPass());

    if (isConnected) {
      _startServer();
    } else {
      // Gagal konek → Setup AP
      _startSetupAP();
    }
  }

  void handle() {
    if (isAPMode) dnsServer.processNextRequest();
  }

private:

  // ============================================================
  // KONEKSI WIFI
  // ============================================================
  void _connectWifi(String ssid, String pass) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(storage.getDeviceName().c_str());

    // IP Static jika dikonfigurasi
    if (storage.getIPMode() == "static") {
      String sip = storage.getStaticIP();
      String sgw = storage.getGateway();
      String ssn = storage.getSubnet();
      String sdns = storage.getDNS();
      if (sip.length() > 6 && sgw.length() > 6) {
        IPAddress ip, gw, sn, dns;
        if (ip.fromString(sip) && gw.fromString(sgw)) {
          sn.fromString(ssn.length() > 6 ? ssn : "255.255.255.0");
          dns.fromString(sdns.length() > 6 ? sdns : "8.8.8.8");
          WiFi.config(ip, gw, sn, dns);
          Serial.println("[WiFi] IP Static: " + sip);
        } else {
          Serial.println("[WiFi] IP Static invalid, pakai DHCP");
        }
      }
    }

    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.print("[WiFi] Konek ke: " + ssid);

    unsigned long start = millis();
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED) {
      if (millis() - start >= WIFI_TIMEOUT_MS) {
        Serial.println("\n[WiFi] Timeout!");
        return;
      }
      oledDisp.showWifiConnecting(ssid, attempt++);
      delay(WIFI_RETRY_DELAY);
      Serial.print(".");
    }

    isConnected = true;
    ipAddress   = WiFi.localIP().toString();
    Serial.println("\n[WiFi] Terhubung! IP: " + ipAddress);
    oledDisp.showWifiConnected(ipAddress);
  }

  // ============================================================
  // AP SETUP MODE — wizard WiFi
  // ============================================================
  void _startSetupAP() {
    isAPMode    = true;
    isSetupMode = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.softAPsetHostname(storage.getDeviceName().c_str());
    dnsServer.start(53, "*", WiFi.softAPIP());
    ipAddress = WiFi.softAPIP().toString();

    oledDisp.setWifiInfo("Setup", AP_SSID, ipAddress, true);
    oledDisp.showAPMode();
    Serial.println("[AP] Setup Mode: " + String(AP_SSID));

    _registerRoutes();
    server.begin();
    Serial.println("[Web] Server aktif di " + ipAddress);
  }

  // ============================================================
  // LOCAL AP MODE — hotspot permanen
  // ============================================================
  void _startLocalAP() {
    isAPMode    = true;
    isSetupMode = false;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.softAPsetHostname(storage.getDeviceName().c_str());
    dnsServer.start(53, "*", WiFi.softAPIP());
    ipAddress = WiFi.softAPIP().toString();

    oledDisp.setWifiInfo("AP", AP_SSID, ipAddress, true);
    Serial.println("[AP] Local AP Mode: " + ipAddress);

    _registerRoutes();
    server.begin();
    Serial.println("[Web] Server aktif di " + ipAddress);
  }

  // ============================================================
  // START SERVER — WiFi mode normal
  // ============================================================
  void _startServer() {
    oledDisp.setWifiInfo("WiFi", storage.getWifiSSID(), ipAddress, true);
    _registerRoutes();
    server.begin();
    Serial.println("[Web] Server aktif di " + ipAddress);
  }

  // ============================================================
  // REGISTER ROUTES — dipanggil sekali per mode
  // ============================================================
  void _registerRoutes() {

    // --- Halaman Login (tidak perlu auth) ---
    server.on("/login", HTTP_GET, [](AsyncWebServerRequest* req) {
      if (sessionMgr.isValid(req)) {
        req->redirect("/");
        return;
      }
      sendGzip(req, LOGIN_HTML_GZ, LOGIN_HTML_GZ_LEN, "text/html");
    });

    // --- API: Login POST ---
    AsyncCallbackJsonWebHandler* loginHandler =
      new AsyncCallbackJsonWebHandler("/api/login",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
          String usr = json["usr"].as<String>();
          String pwd = json["pwd"].as<String>();
          if (usr == storage.getAuthUser() && pwd == storage.getAuthPass()) {
            String token = sessionMgr.createSession();
            AsyncWebServerResponse* res = req->beginResponse(200, "application/json",
              "{\"success\":true,\"token\":\"" + token + "\"}");
            res->addHeader("Set-Cookie", SessionManager::makeCookieHeader(token));
            req->send(res);
          } else {
            req->send(200, "application/json", "{\"success\":false}");
          }
        });
    server.addHandler(loginHandler);

    // --- API: Logout ---
    server.on("/api/logout", HTTP_POST, [](AsyncWebServerRequest* req) {
      String token = sessionMgr.extractToken(req);
      sessionMgr.invalidateToken(token);
      AsyncWebServerResponse* res = req->beginResponse(200, "application/json", "{\"ok\":true}");
      res->addHeader("Set-Cookie", SessionManager::clearCookieHeader());
      req->send(res);
    });

    // Helper macro auth check
    #define AUTH_CHECK(req) if (!sessionMgr.isValid(req)) { req->redirect("/login"); return; }

    // --- Halaman Utama ---
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      if (!storage.isSetupDone()) {
        req->redirect("/setup");
        return;
      }
      sendGzip(req, DASHBOARD_HTML_GZ, DASHBOARD_HTML_GZ_LEN, "text/html");
    });

    // --- Setup Wizard (bebas, tidak perlu auth) ---
    server.on("/setup", HTTP_GET, [](AsyncWebServerRequest* req) {
      req->send(200, "text/html", WIZARD_HTML);
    });

    // --- Halaman Setting ---
    server.on("/setting", HTTP_GET, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      sendGzip(req, SETTING_HTML_GZ, SETTING_HTML_GZ_LEN, "text/html");
    });

    // --- Halaman System ---
    server.on("/system", HTTP_GET, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      sendGzip(req, SYSTEM_HTML_GZ, SYSTEM_HTML_GZ_LEN, "text/html");
    });

    // --- API: Data sensor realtime ---
    server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      extern bool bmpActive, mpuActive;
      extern unsigned long uptimeSeconds;
      String ip = WiFi.localIP().toString();
      if (ip == "0.0.0.0") ip = WiFi.softAPIP().toString();
      String json = buildDataJson(ip, uptimeSeconds, bmpActive, mpuActive);
      // Inject ntp_time — hapus } terakhir, tambah field
      json.remove(json.lastIndexOf('}'));
      json += ",\"ntp_time\":\"" + ntpMgr.getTimeStr() + "\""
              ",\"ntp_synced\":" + (ntpMgr.synced ? "true" : "false") + "}";
      req->send(200, "application/json", json);
    });

    // --- API: System info ---
    server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      extern unsigned long uptimeSeconds;
      extern bool mqttAOk, mqttBOk;
      String ip  = WiFi.localIP().toString();
      String gw  = WiFi.gatewayIP().toString();
      String mac = WiFi.macAddress();
      JsonDocument doc;
      doc["ram_free"]    = ESP.getFreeHeap();
      doc["ram_total"]   = ESP.getHeapSize();
      doc["chip_temp"]   = temperatureRead();
      doc["rssi"]        = WiFi.RSSI();
      doc["cpu_mhz"]     = ESP.getCpuFreqMHz();
      doc["flash_free"]  = ESP.getFreeSketchSpace();
      doc["flash_total"] = ESP.getFlashChipSize();
      doc["uptime"]      = uptimeSeconds;
      doc["ip"]          = ip;
      doc["gw"]          = gw;
      doc["mac"]         = mac;
      doc["host"]        = storage.getDeviceName();
      doc["ssid"]        = WiFi.SSID();
      doc["version"]     = FIRMWARE_VERSION;
      doc["build"]       = FIRMWARE_BUILD;
      doc["chip"]        = "ESP32";
      doc["mqtt_a"]      = mqttAOk;
      doc["mqtt_b"]      = mqttBOk;
      doc["ntp_synced"]  = ntpMgr.synced;
      doc["ntp_time"]    = ntpMgr.getTimeStr();
      doc["ntp_last_sync"] = ntpMgr.lastSync;
      String out;
      serializeJson(doc, out);
      req->send(200, "application/json", out);
    });

    // --- API: Scan WiFi ---
    server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest* req) {
      int n = WiFi.scanNetworks();
      String json = "{\"networks\":[";
      for (int i = 0; i < n; i++) {
        if (i > 0) json += ",";
        String ssid = WiFi.SSID(i);
        ssid.replace("\"", "\\\"");
        json += "{\"ssid\":\"" + ssid + "\",\"rssi\":" + WiFi.RSSI(i) + "}";
      }
      json += "]}";
      req->send(200, "application/json", json);
    });

    // --- API: Simpan setting WiFi wizard ---
    AsyncCallbackJsonWebHandler* saveHandler =
      new AsyncCallbackJsonWebHandler("/api/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
          String mode   = json["mode"].as<String>();
          String devname= json["devname"].as<String>();
          String ssid   = json["ssid"].as<String>();
          String pass   = json["password"].as<String>();
          String ipMode = json["ip_mode"].as<String>();

          if (devname.length() > 0) storage.setDeviceName(devname);
          storage.setMode(mode);
          if (mode == MODE_WIFI) {
            storage.setWifiCredentials(ssid, pass);
            storage.setIPMode(ipMode);
            if (ipMode == "static") {
              storage.setStaticIP(
                json["ip"].as<String>(),
                json["gw"].as<String>(),
                json["sn"].as<String>(),
                json["dns"].as<String>()
              );
            }
          }
          storage.setSetupDone(true);
          req->send(200, "application/json", "{\"success\":true}");
          delay(2000);
          ESP.restart();
        });
    server.addHandler(saveHandler);

    // --- API: Status ---
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      extern bool mqttAOk, mqttBOk;
      String json = "{\"ok\":true";
      json += ",\"mqtt_a\":" + String(mqttAOk ? "true" : "false");
      json += ",\"mqtt_b\":" + String(mqttBOk ? "true" : "false");
      json += "}";
      req->send(200, "application/json", json);
    });

    // --- Halaman Log ---
    extern void handleLogPage(AsyncWebServerRequest*);
    server.on("/log", HTTP_GET, handleLogPage);

    // --- API: Ambil log ---
    server.on("/api/log", HTTP_GET, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      String cat = req->hasParam("cat") ? req->getParam("cat")->value() : "all";
      String json = logger.toJson(cat.c_str());
      req->send(200, "application/json", json);
    });

    // --- API: Clear log ---
    server.on("/api/log/clear", HTTP_POST, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      logger.clear();
      req->send(200, "application/json", "{\"ok\":true}");
    });
    server.on("/api/ota", HTTP_POST,
      // onRequest — dipanggil setelah upload selesai
      [](AsyncWebServerRequest* req) {
        if (!sessionMgr.isValid(req)) {
          req->send(401, "text/plain", "Unauthorized");
          return;
        }
        bool ok = !Update.hasError();
        if (ok) {
          // Simpan kWh sebelum restart
          storage.saveKwh(ina.kwh[0], ina.kwhCh2In, ina.kwhCh2Out, ina.kwh[2]);
          oledDisp.showStatus("OTA Selesai!", "Restart...", "");
          req->send(200, "text/plain", "OK");
          delay(1000);
          ESP.restart();
        } else {
          String err = "Gagal: " + String(Update.errorString());
          req->send(500, "text/plain", err);
          Serial.printf("[OTA] Error: %s\n", Update.errorString());
        }
      },
      // onUpload — dipanggil saat data masuk
      [](AsyncWebServerRequest* req, String filename, size_t index, uint8_t* data, size_t len, bool final) {
        if (!sessionMgr.isValid(req)) return;
        if (index == 0) {
          Serial.printf("[OTA] Upload mulai: %s\n", filename.c_str());
          oledDisp.showStatus("OTA Update", "Uploading...", "");
          if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
            Serial.printf("[OTA] begin error: %s\n", Update.errorString());
          }
        }
        if (Update.write(data, len) != len) {
          Serial.printf("[OTA] write error: %s\n", Update.errorString());
        }
        // Progress ke OLED tiap ~10%
        if (req->contentLength() > 0) {
          int pct = (index + len) * 100 / req->contentLength();
          if (pct % 10 == 0) {
            char buf[16]; snprintf(buf, sizeof(buf), "%d%%", pct);
            oledDisp.showStatus("OTA Update", buf, "Jangan matikan!");
          }
        }
        if (final) {
          if (Update.end(true)) {
            Serial.printf("[OTA] Sukses! %u bytes\n", index + len);
          } else {
            Serial.printf("[OTA] end error: %s\n", Update.errorString());
          }
        }
      }
    );

    // --- API: OTA Rollback ---
    server.on("/api/ota/rollback", HTTP_POST, [](AsyncWebServerRequest* req) {
      AUTH_CHECK(req)
      if (Update.canRollBack()) {
        req->send(200, "application/json", "{\"success\":true}");
        Serial.println("[OTA] Rollback...");
        oledDisp.showStatus("OTA Rollback", "Restart...", "");
        delay(1000);
        Update.rollBack();
        ESP.restart();
      } else {
        req->send(200, "application/json", "{\"success\":false,\"error\":\"Tidak ada firmware lama\"}");
        Serial.println("[OTA] Rollback tidak tersedia");
      }
    });
    server.on("/api/setting", HTTP_GET, [](AsyncWebServerRequest* req) {
      String json = "{";
      json += "\"oled_on\":"     + String(storage.getOledEnabled()    ? "true" : "false") + ",";
      json += "\"oled_scroll\":" + String(storage.getOledAutoScroll() ? "true" : "false") + ",";
      json += "\"oled_page\":"   + String(storage.getOledPage())    + ",";
      json += "\"oled_dur\":"    + String(storage.getOledDuration() / 1000) + ",";
      json += "\"oled_bright\":" + String(storage.getOledBrightness()) + ",";
      json += "\"bq_on\":"  + String(storage.getBuzzQuakeOnMs())   + ",";
      json += "\"bq_off\":" + String(storage.getBuzzQuakeOffMs())  + ",";
      json += "\"bq_dur\":" + String(storage.getBuzzQuakeDurSec()) + ",";
      json += "\"bv_on\":"  + String(storage.getBuzzVoltOnMs())    + ",";
      json += "\"bv_off\":" + String(storage.getBuzzVoltOffMs())   + ",";
      json += "\"bv_dur\":" + String(storage.getBuzzVoltDurSec())  + ",";
      json += "\"bc_on\":"  + String(storage.getBuzzCurrOnMs())    + ",";
      json += "\"bc_off\":" + String(storage.getBuzzCurrOffMs())   + ",";
      json += "\"bc_dur\":" + String(storage.getBuzzCurrDurSec())  + ",";
      json += "\"mpu_cal\":"     + String(storage.isMpuCalibrated() ? "true" : "false") + ",";
      json += "\"shunt_ohms\":"  + String(storage.getShuntOhms(), 6) + ",";
      json += "\"bmp_t_off\":"   + String(storage.getBmpTempOffset(), 1) + ",";
      json += "\"ch1\":\""   + storage.getCh1Name()  + "\",";
      json += "\"ch2\":\""   + storage.getCh2Name()  + "\",";
      json += "\"ch3\":\""   + storage.getCh3Name()  + "\",";
      json += "\"bmp\":"     + String(storage.isBmpEnabled()  ? "true" : "false") + ",";
      json += "\"mpu\":"     + String(storage.isMpuEnabled()  ? "true" : "false") + ",";
      json += "\"buzz\":"    + String(storage.isBuzzEnabled() ? "true" : "false") + ",";
      json += "\"vmin\":"    + String(storage.getAlarmVMin(),  1) + ",";
      json += "\"vmax\":"    + String(storage.getAlarmVMax(),  1) + ",";
      json += "\"imax\":"    + String(storage.getAlarmIMax(),  1) + ",";
      json += "\"quake\":"   + String(storage.getAlarmQuake(), 1) + ",";
      json += "\"devname\":\"" + storage.getDeviceName()  + "\",";
      json += "\"ssid\":\""    + storage.getWifiSSID()    + "\",";
      json += "\"ip_mode\":\"" + storage.getIPMode()      + "\",";
      json += "\"ip\":\""      + storage.getStaticIP()    + "\",";
      json += "\"gw\":\""      + storage.getGateway()     + "\",";
      json += "\"sn\":\""      + storage.getSubnet()      + "\",";
      json += "\"dns\":\""     + storage.getDNS()         + "\",";
      // MQTT Broker A
      json += "\"ma_en\":"    + String(storage.getMqttAEnabled() ? "true" : "false") + ",";
      json += "\"ma_host\":\"" + storage.getMqttAHost()   + "\",";
      json += "\"ma_port\":"   + String(storage.getMqttAPort())     + ",";
      json += "\"ma_user\":\"" + storage.getMqttAUser()   + "\",";
      json += "\"ma_topic\":\"" + storage.getMqttATopic() + "\",";
      json += "\"ma_int\":"    + String(storage.getMqttAInterval()) + ",";
      json += "\"ma_data\":"   + String(storage.getMqttAData())     + ",";
      // MQTT Broker B
      json += "\"mb_en\":"    + String(storage.getMqttBEnabled() ? "true" : "false") + ",";
      json += "\"mb_host\":\"" + storage.getMqttBHost()   + "\",";
      json += "\"mb_port\":"   + String(storage.getMqttBPort())     + ",";
      json += "\"mb_user\":\"" + storage.getMqttBUser()   + "\",";
      json += "\"mb_topic\":\"" + storage.getMqttBTopic() + "\",";
      json += "\"mb_int\":"    + String(storage.getMqttBInterval()) + ",";
      json += "\"mb_data\":"   + String(storage.getMqttBData())     + ",";
      // Lokasi
      json += "\"loc_set\":"   + String(storage.isLocSet() ? "true" : "false") + ",";
      json += "\"loc_name\":\"" + storage.getLocName()    + "\",";
      json += "\"loc_lat\":"   + String(storage.getLocLat(), 7)     + ",";
      json += "\"loc_lng\":"   + String(storage.getLocLng(), 7)     + ",";
      json += "\"loc_alt\":"   + String(storage.getLocAlt(), 1)     + ",";
      json += "\"ntp_en\":"    + String(storage.getNtpEnabled() ? "true" : "false") + ",";
      json += "\"ntp_server\":\"" + storage.getNtpServer() + "\",";
      json += "\"ntp_offset\":" + String(storage.getNtpOffset());
      json += "}";
      req->send(200, "application/json", json);
    });

    // --- API: Simpan setting ---
    AsyncCallbackJsonWebHandler* settingHandler =
      new AsyncCallbackJsonWebHandler("/api/setting",
        [](AsyncWebServerRequest* req, JsonVariant& body) {
          String type = body["type"].as<String>();

          if (type == "buzzer") {
            storage.setBuzzQuakeOnMs(body["bq_on"].as<int>());
            storage.setBuzzQuakeOffMs(body["bq_off"].as<int>());
            storage.setBuzzQuakeDurSec(body["bq_dur"].as<int>());
            storage.setBuzzVoltOnMs(body["bv_on"].as<int>());
            storage.setBuzzVoltOffMs(body["bv_off"].as<int>());
            storage.setBuzzVoltDurSec(body["bv_dur"].as<int>());
            storage.setBuzzCurrOnMs(body["bc_on"].as<int>());
            storage.setBuzzCurrOffMs(body["bc_off"].as<int>());
            storage.setBuzzCurrDurSec(body["bc_dur"].as<int>());
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "buzz-test") {
            String ev = body["event"].as<String>();
            if (ev == "volt")
              buzzer.playAlarm(storage.getBuzzVoltOnMs(), storage.getBuzzVoltOffMs(), 3);
            else if (ev == "curr")
              buzzer.playAlarm(storage.getBuzzCurrOnMs(), storage.getBuzzCurrOffMs(), 3);
            else
              buzzer.playAlarm(storage.getBuzzQuakeOnMs(), storage.getBuzzQuakeOffMs(), 3);
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "calib-start") {
            extern volatile bool calibRequested, calibDone, calibSuccess;
            calibDone      = false;
            calibSuccess   = false;
            calibRequested = true;  // loop() akan proses
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "calib-poll") {
            extern volatile bool calibDone, calibSuccess;
            if (!calibDone) {
              req->send(200, "application/json", "{\"done\":false}");
            } else {
              String r = calibSuccess ? "{\"done\":true,\"success\":true}" : "{\"done\":true,\"success\":false}";
              req->send(200, "application/json", r);
            }

          } else if (type == "calib-reset") {
            mpu.resetCalibration();
            storage.resetMpuCalib();
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "oled") {
            bool on     = body["oled_on"].as<bool>();
            bool scroll = body["oled_scroll"].as<bool>();
            int  page   = body["oled_page"].as<int>();
            int  dur    = body["oled_dur"].as<int>();
            int  bright = body["oled_bright"].as<int>();
            storage.setOledEnabled(on);
            storage.setOledAutoScroll(scroll);
            storage.setOledPage(page);
            storage.setOledDuration(dur);
            storage.setOledBrightness(bright);
            // Terapkan langsung ke OLED tanpa restart
            oledDisp.setEnabled(on);
            oledDisp.setBrightness(bright);
            oledDisp.setAutoScroll(scroll, dur);
            if (!scroll) oledDisp.setPage(page);
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "channel") {
            if (body["ch1"].as<String>().length()) storage.setCh1Name(body["ch1"].as<String>());
            if (body["ch2"].as<String>().length()) storage.setCh2Name(body["ch2"].as<String>());
            if (body["ch3"].as<String>().length()) storage.setCh3Name(body["ch3"].as<String>());
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "sensor") {
            storage.setBmpEnabled(body["bmp"].as<bool>());
            storage.setMpuEnabled(body["mpu"].as<bool>());
            storage.setBuzzEnabled(body["buzz"].as<bool>());
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "alarm") {
            storage.setAlarmVMin(body["vmin"].as<float>());
            storage.setAlarmVMax(body["vmax"].as<float>());
            storage.setAlarmIMax(body["imax"].as<float>());
            storage.setAlarmQuake(body["quake"].as<float>());
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "wifi") {
            storage.setDeviceName(body["devname"].as<String>());
            storage.setWifiCredentials(body["ssid"].as<String>(), body["password"].as<String>());
            storage.setIPMode(body["ip_mode"].as<String>());
            if (body["ip_mode"].as<String>() == "static") {
              storage.setStaticIP(body["ip"].as<String>(), body["gw"].as<String>(),
                                  body["sn"].as<String>(), body["dns"].as<String>());
            }
            req->send(200, "application/json", "{\"success\":true}");
            delay(3000);
            ESP.restart();

          } else if (type == "reset-wifi") {
            storage.clearWifiSettings();
            req->send(200, "application/json", "{\"success\":true}");
            delay(2000);
            ESP.restart();

          } else if (type == "reset-kwh") {
            storage.resetKwh();   // reset NVS
            ina.resetKwh();       // reset RAM — bug fix: sebelumnya nilai lama masih jalan
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "bmp-offset") {
            storage.setBmpTempOffset(body["bmp_t_off"].as<float>());
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "shunt") {
            float ohms = body["shunt_ohms"].as<float>();
            if (ohms > 0.0f) {
              storage.setShuntOhms(ohms);
              ina.setShunt(ohms);   // terapkan langsung tanpa restart
              req->send(200, "application/json", "{\"success\":true}");
            } else {
              req->send(200, "application/json", "{\"success\":false,\"msg\":\"Nilai tidak valid\"}");
            }

          } else if (type == "mqtt") {
            // Simpan Broker A
            storage.setMqttA(
              body["ma_en"].as<bool>(),
              body["ma_host"].as<String>(),
              body["ma_port"].as<int>(),
              body["ma_user"].as<String>(),
              body["ma_pass"].as<String>(),
              body["ma_topic"].as<String>(),
              body["ma_int"].as<int>(),
              body["ma_data"].as<int>()
            );
            // Simpan Broker B
            storage.setMqttB(
              body["mb_en"].as<bool>(),
              body["mb_host"].as<String>(),
              body["mb_port"].as<int>(),
              body["mb_user"].as<String>(),
              body["mb_pass"].as<String>(),
              body["mb_topic"].as<String>(),
              body["mb_int"].as<int>(),
              body["mb_data"].as<int>()
            );
            // Reload MQTT — mqttManager didefinisikan di mqtt_client.h yang di-include SETELAH wifi_manager.h
            // Gunakan flag, reload dilakukan di loop() main.cpp
            extern bool mqttReloadRequested;
            mqttReloadRequested = true;
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "location") {
            storage.setLocation(
              body["name"].as<String>(),
              body["lat"].as<double>(),
              body["lng"].as<double>(),
              body["alt"].as<float>()
            );
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "auth") {
            String usr = body["usr"].as<String>();
            String pwd = body["pwd"].as<String>();
            if (usr.length() > 0 && pwd.length() >= 4) {
              storage.setAuthUser(usr);
              storage.setAuthPass(pwd);
              sessionMgr.invalidateAll();
              req->send(200, "application/json", "{\"success\":true}");
            } else {
              req->send(200, "application/json", "{\"success\":false}");
            }

          } else if (type == "ntp") {
            storage.setNtpEnabled(body["ntp_en"].as<bool>());
            storage.setNtpServer(body["ntp_server"].as<String>());
            storage.setNtpOffset(body["ntp_offset"].as<int>());
            ntpMgr.applyTimezone();
            String ts = ntpMgr.synced ? ntpMgr.getTimeStr() : String("");
            String msg = "NTP setting disimpan: " + body["ntp_server"].as<String>()
              + " UTC" + String(body["ntp_offset"].as<int>());
            logger.add(LOG_SYS, msg.c_str(), ts.length() ? ts.c_str() : nullptr);
            req->send(200, "application/json", "{\"success\":true}");

          } else if (type == "ntp_sync") {
            ntpMgr.sync();
            if (ntpMgr.synced) {
              String t = ntpMgr.getTimeStr();
              logger.add(LOG_SYS, ("NTP manual sync OK: " + t).c_str(), t.c_str());
              req->send(200, "application/json",
                "{\"success\":true,\"time\":\"" + t + "\"}");
            } else {
              logger.add(LOG_SYS, "NTP manual sync GAGAL", nullptr);
              req->send(200, "application/json", "{\"success\":false}");
            }

          } else if (type == "restart") {
            // Simpan kWh sebelum restart agar tidak hilang
            storage.saveKwh(ina.kwh[0], ina.kwhCh2In, ina.kwhCh2Out, ina.kwh[2]);
            req->send(200, "application/json", "{\"success\":true}");
            delay(3000);
            ESP.restart();

          } else {
            req->send(400, "application/json", "{\"success\":false}");
          }
        });
    server.addHandler(settingHandler);

    // --- Not Found ---
    server.onNotFound([](AsyncWebServerRequest* req) {
      // Captive portal — hanya redirect saat AP/Setup mode
      if (req->host() == "192.168.4.1") {
        req->redirect("http://192.168.4.1/");
        return;
      }
      req->send(404, "text/plain", "Not found");
    });
  }
};

WifiManager wifiManager;