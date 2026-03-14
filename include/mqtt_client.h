#pragma once
// ============================================================
// SOLAR MONITOR — mqtt_client.h
// Auto-mode: TCP (1883) / SSL (8883) / WebSocket (9001/443)
// MQTT Discovery untuk Home Assistant
// ============================================================
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <functional>
#include "config.h"
#include "storage.h"
#include "ina3221.h"
#include "bmp280.h"
#include "mpu6050.h"

#define MQTT_DATA_INA    0b00001
#define MQTT_DATA_BMP    0b00010
#define MQTT_DATA_MPU    0b00100
#define MQTT_DATA_ALARM  0b01000
#define MQTT_DATA_LOC    0b10000
#define MQTT_MODE_TCP  0
#define MQTT_MODE_SSL  1
#define MQTT_MODE_WS   2

static int detectMqttMode(int port) {
  if (port == 8883) return MQTT_MODE_SSL;
  if (port == 9001 || port == 443 || port == 80) return MQTT_MODE_WS;
  return MQTT_MODE_TCP;
}

// ============================================================
// Shared publish data builder
// ============================================================
static String buildPayload(const String& devName, int dataMask, bool bmpActive, bool mpuActive) {
  JsonDocument doc;
  doc["device"] = devName;
  doc["ts"]     = millis();
  if (dataMask & MQTT_DATA_INA) {
    JsonObject io = doc["ina"].to<JsonObject>();
    JsonArray  ch = io["ch"].to<JsonArray>();
    for (int i = 0; i < 3; i++) {
      JsonObject c = ch.add<JsonObject>();
      c["v"] = round(ina.ch[i].voltage * 100) / 100.0;
      c["i"] = round(ina.ch[i].current * 1000) / 1000.0;
      c["p"] = round(ina.ch[i].power   * 100)  / 100.0;
    }
    io["kwh_panel"]   = round(ina.kwh[0]    * 1000) / 1000.0;
    io["kwh_bat_in"]  = round(ina.kwhCh2In  * 1000) / 1000.0;
    io["kwh_bat_out"] = round(ina.kwhCh2Out * 1000) / 1000.0;
    io["kwh_load"]    = round(ina.kwh[2]    * 1000) / 1000.0;
    io["soc"]         = ina.getSOC();
  }
  if ((dataMask & MQTT_DATA_BMP) && bmpActive) {
    JsonObject b = doc["bmp"].to<JsonObject>();
    b["temp"]     = round(bmp.temperature * 10) / 10.0;
    b["pressure"] = round(bmp.pressure    * 10) / 10.0;
    b["altitude"] = round(bmp.altitude    * 10) / 10.0;
  }
  if ((dataMask & MQTT_DATA_MPU) && mpuActive) {
    JsonObject m = doc["mpu"].to<JsonObject>();
    m["vib"]    = round(mpu.vibration * 1000) / 1000.0;
    m["status"] = mpu.vibrationStatus();
    m["quake"]  = mpu.quakeDetected;
    m["ax"]     = round(mpu.accX * 100) / 100.0;
    m["ay"]     = round(mpu.accY * 100) / 100.0;
  }
  if ((dataMask & MQTT_DATA_LOC) && storage.isLocSet()) {
    JsonObject loc = doc["loc"].to<JsonObject>();
    loc["name"] = storage.getLocName();
    loc["lat"]  = storage.getLocLat();
    loc["lng"]  = storage.getLocLng();
    loc["alt"]  = storage.getLocAlt();
  }
  String out; serializeJson(doc, out); return out;
}

// ============================================================
// MQTT Discovery — publish config ke HA sekali saat connect
// Format: homeassistant/sensor/{devId}/{sensorId}/config
// ============================================================
static void publishDiscovery(
    std::function<void(const String&, const String&)> publishFn,
    const String& devName, int dataMask, bool bmpActive, bool mpuActive)
{
  String devId   = devName;
  devId.replace(" ", "_");
  devId.toLowerCase();

  String stateTopic = "solar/" + devName + "/data";
  String avtyTopic  = "solar/" + devName + "/status";

  // Device info — semua entity di-group jadi satu device di HA
  auto makeDevice = [&](JsonDocument& doc) {
    JsonObject dev = doc["device"].to<JsonObject>();
    dev["name"]         = devName;
    dev["identifiers"]  = devId;
    dev["model"]        = "ESP32 Solar Monitor";
    dev["manufacturer"] = "SolarMonitor";
    dev["configuration_url"] = "http://" + WiFi.localIP().toString();
  };

  // Helper tambah availability ke setiap sensor
  auto addAvty = [&](JsonDocument& doc) {
    doc["availability_topic"]     = avtyTopic;
    doc["payload_available"]      = "online";
    doc["payload_not_available"]  = "offline";
  };

  auto pub = [&](const String& sensorId, JsonDocument& doc) {
    String topic = "homeassistant/sensor/" + devId + "/" + sensorId + "/config";
    String payload;
    serializeJson(doc, payload);
    publishFn(topic, payload);
    delay(50);  // Jeda kecil agar HA tidak overwhelmed
  };

  String ch0Name = storage.getCh1Name();
  String ch1Name = storage.getCh2Name();
  String ch2Name = storage.getCh3Name();

  if (dataMask & MQTT_DATA_INA) {
    // --- CH0 Panel ---
    { JsonDocument d; makeDevice(d);
      d["name"]             = ch0Name + " Voltage";
      d["unique_id"]        = devId + "_ch0_v";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.ch[0].v }}";
      d["unit_of_measurement"] = "V";
      d["device_class"]     = "voltage";
      addAvty(d);
      pub("ch0_voltage", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch0Name + " Current";
      d["unique_id"]        = devId + "_ch0_i";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.ch[0].i }}";
      d["unit_of_measurement"] = "A";
      d["device_class"]     = "current";
      addAvty(d);
      pub("ch0_current", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch0Name + " Power";
      d["unique_id"]        = devId + "_ch0_p";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.ch[0].p }}";
      d["unit_of_measurement"] = "W";
      d["device_class"]     = "power";
      addAvty(d);
      pub("ch0_power", d); }

    // --- CH1 Baterai ---
    { JsonDocument d; makeDevice(d);
      d["name"]             = ch1Name + " Voltage";
      d["unique_id"]        = devId + "_ch1_v";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.ch[1].v }}";
      d["unit_of_measurement"] = "V";
      d["device_class"]     = "voltage";
      addAvty(d);
      pub("ch1_voltage", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch1Name + " Current";
      d["unique_id"]        = devId + "_ch1_i";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.ch[1].i }}";
      d["unit_of_measurement"] = "A";
      d["device_class"]     = "current";
      addAvty(d);
      pub("ch1_current", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch1Name + " SOC";
      d["unique_id"]        = devId + "_soc";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.soc }}";
      d["unit_of_measurement"] = "%";
      d["device_class"]     = "battery";
      addAvty(d);
      pub("soc", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch1Name + " kWh In";
      d["unique_id"]        = devId + "_kwh_bat_in";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.kwh_bat_in }}";
      d["unit_of_measurement"] = "kWh";
      d["device_class"]     = "energy";
      d["state_class"]      = "total_increasing";
      addAvty(d);
      pub("kwh_bat_in", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch1Name + " kWh Out";
      d["unique_id"]        = devId + "_kwh_bat_out";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.kwh_bat_out }}";
      d["unit_of_measurement"] = "kWh";
      d["device_class"]     = "energy";
      d["state_class"]      = "total_increasing";
      addAvty(d);
      pub("kwh_bat_out", d); }

    // --- CH2 Beban ---
    { JsonDocument d; makeDevice(d);
      d["name"]             = ch2Name + " Voltage";
      d["unique_id"]        = devId + "_ch2_v";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.ch[2].v }}";
      d["unit_of_measurement"] = "V";
      d["device_class"]     = "voltage";
      addAvty(d);
      pub("ch2_voltage", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch2Name + " Power";
      d["unique_id"]        = devId + "_ch2_p";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.ch[2].p }}";
      d["unit_of_measurement"] = "W";
      d["device_class"]     = "power";
      addAvty(d);
      pub("ch2_power", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch2Name + " kWh";
      d["unique_id"]        = devId + "_kwh_load";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.kwh_load }}";
      d["unit_of_measurement"] = "kWh";
      d["device_class"]     = "energy";
      d["state_class"]      = "total_increasing";
      addAvty(d);
      pub("kwh_load", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = ch0Name + " kWh";
      d["unique_id"]        = devId + "_kwh_panel";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.ina.kwh_panel }}";
      d["unit_of_measurement"] = "kWh";
      d["device_class"]     = "energy";
      d["state_class"]      = "total_increasing";
      addAvty(d);
      pub("kwh_panel", d); }
  }

  if ((dataMask & MQTT_DATA_BMP) && bmpActive) {
    { JsonDocument d; makeDevice(d);
      d["name"]             = "Suhu";
      d["unique_id"]        = devId + "_temp";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.bmp.temp }}";
      d["unit_of_measurement"] = "°C";
      d["device_class"]     = "temperature";
      addAvty(d);
      pub("temperature", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = "Tekanan Udara";
      d["unique_id"]        = devId + "_pressure";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.bmp.pressure }}";
      d["unit_of_measurement"] = "hPa";
      d["device_class"]     = "atmospheric_pressure";
      addAvty(d);
      pub("pressure", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = "Ketinggian";
      d["unique_id"]        = devId + "_altitude";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.bmp.altitude }}";
      d["unit_of_measurement"] = "m";
      addAvty(d);
      pub("altitude", d); }
  }

  if ((dataMask & MQTT_DATA_MPU) && mpuActive) {
    { JsonDocument d; makeDevice(d);
      d["name"]             = "Getaran";
      d["unique_id"]        = devId + "_vibration";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.mpu.vib }}";
      d["unit_of_measurement"] = "g";
      addAvty(d);
      pub("vibration", d); }

    { JsonDocument d; makeDevice(d);
      d["name"]             = "Status Gempa";
      d["unique_id"]        = devId + "_quake_status";
      d["state_topic"]      = stateTopic;
      d["value_template"]   = "{{ value_json.mpu.status }}";
      addAvty(d);
      pub("quake_status", d); }
  }

  Serial.printf("[MQTT] Discovery published untuk %d sensor\n",
    ((dataMask & MQTT_DATA_INA) ? 12 : 0) +
    ((dataMask & MQTT_DATA_BMP) && bmpActive ? 3 : 0) +
    ((dataMask & MQTT_DATA_MPU) && mpuActive ? 2 : 0));
}

class WsMqttBroker {
public:
  bool isConnected = false;
  String label;
private:
  WebSocketsClient _ws;
  String _host, _user, _pass, _topic;
  int    _port, _intervalSec, _dataMask;
  bool   _enabled=false, _wsConn=false, _mqttReady=false;
  bool   _discoveryDone=false;
  unsigned long _lastPublish=0, _lastPing=0;

  void _writeMqttStr(uint8_t* buf, int& pos, const String& s) {
    uint16_t len = s.length();
    buf[pos++] = (len>>8)&0xFF; buf[pos++] = len&0xFF;
    memcpy(buf+pos, s.c_str(), len); pos += len;
  }

  void _sendConnect() {
    String clientId = "solar-" + storage.getDeviceName() + "-" + label;
    bool hasAuth = (_user.length() > 0);
    int payLen = 2 + clientId.length();
    uint8_t flags = 0x02;
    if (hasAuth) { flags |= 0xC0; payLen += 2+_user.length()+2+_pass.length(); }
    int remLen = 10 + payLen;
    uint8_t buf[256]; int pos=0;
    buf[pos++]=0x10; buf[pos++]=(uint8_t)remLen;
    buf[pos++]=0x00; buf[pos++]=0x04;
    buf[pos++]='M'; buf[pos++]='Q'; buf[pos++]='T'; buf[pos++]='T';
    buf[pos++]=0x04; buf[pos++]=flags;
    buf[pos++]=0x00; buf[pos++]=0x3C;
    _writeMqttStr(buf, pos, clientId);
    if (hasAuth) { _writeMqttStr(buf, pos, _user); _writeMqttStr(buf, pos, _pass); }
    _ws.sendBIN(buf, pos);
    Serial.printf("[MQTT-%s] WS CONNECT sent (id:%s)\n", label.c_str(), clientId.c_str());
  }

  void _sendPublish(const String& topic, const String& payload, bool retain=false) {
    int tLen=topic.length(), pLen=payload.length();
    int remLen = 2+tLen+pLen;
    uint8_t buf[1024]; int pos=0;
    buf[pos++] = retain ? 0x31 : 0x30;  // 0x31 = PUBLISH + retain flag
    if (remLen<128) { buf[pos++]=remLen; }
    else { buf[pos++]=(remLen&0x7F)|0x80; buf[pos++]=remLen>>7; }
    buf[pos++]=(tLen>>8)&0xFF; buf[pos++]=tLen&0xFF;
    memcpy(buf+pos, topic.c_str(), tLen); pos+=tLen;
    memcpy(buf+pos, payload.c_str(), pLen); pos+=pLen;
    _ws.sendBIN(buf, pos);
  }

  void _handlePacket(uint8_t* data, size_t len) {
    if (len<2) return;
    if ((data[0]&0xF0)==0x20) {
      if (data[3]==0x00) {
        _mqttReady=true; isConnected=true;
        Serial.printf("[MQTT-%s] CONNACK OK!\n", label.c_str());
        // Discovery dulu, baru publish online — agar HA proses config sebelum availability
        if (!_discoveryDone) {
          _discoveryDone = true;
          publishDiscovery(
            [this](const String& t, const String& p){ _sendPublish(t, p, true); },
            storage.getDeviceName(), _dataMask, true, true
          );
        }
        // Online status dengan retain — dikirim setelah discovery
        delay(200);
        _sendPublish(_topic+"/"+storage.getDeviceName()+"/status", "online", true);
      } else {
        _mqttReady=false; isConnected=false;
        Serial.printf("[MQTT-%s] CONNACK ditolak: %d\n", label.c_str(), data[3]);
      }
    }
  }

public:
  WsMqttBroker(String lbl) : label(lbl) {}

  void configure(bool en, String host, int port, String user,
                 String pass, String topic, int interval, int dataMask) {
    _enabled=en; _host=host; _port=port; _user=user; _pass=pass;
    _topic=topic; _intervalSec=interval; _dataMask=dataMask;
    if (!en || host.length()==0) return;
    bool ssl = (port==443||port==8884);
    if (ssl) _ws.beginSSL(host.c_str(), port, "/mqtt");
    else     _ws.begin(host.c_str(), port, "/mqtt");
    _ws.setReconnectInterval(5000);
    _ws.setExtraHeaders("Sec-WebSocket-Protocol: mqtt");
    _ws.onEvent([this](WStype_t t, uint8_t* d, size_t l){
      if (t==WStype_CONNECTED) { _wsConn=true; _sendConnect(); }
      else if (t==WStype_DISCONNECTED) { _wsConn=false; _mqttReady=false; isConnected=false;
        _discoveryDone=false;
        Serial.printf("[MQTT-%s] WS Disconnected\n", label.c_str()); }
      else if (t==WStype_BIN) _handlePacket(d,l);
      else if (t==WStype_ERROR) Serial.printf("[MQTT-%s] WS Error\n", label.c_str());
    });
    Serial.printf("[MQTT-%s] WS configured: %s:%d SSL:%s\n",
      label.c_str(), host.c_str(), port, ssl?"yes":"no");
  }

  void handle(bool bmpActive, bool mpuActive) {
    if (!_enabled||_host.length()==0) return;
    if (WiFi.status()!=WL_CONNECTED) { isConnected=false; return; }
    _ws.loop();
    if (!_mqttReady) return;
    if (millis()-_lastPing>=30000) { uint8_t p[2]={0xC0,0x00}; _ws.sendBIN(p,2); _lastPing=millis(); }
    if (millis()-_lastPublish>=(unsigned long)(_intervalSec*1000)) {
      String payload = buildPayload(storage.getDeviceName(), _dataMask, bmpActive, mpuActive);
      _sendPublish(_topic+"/"+storage.getDeviceName()+"/data", payload, true);
      Serial.printf("[MQTT-%s] WS Published %d bytes\n", label.c_str(), payload.length());
      _lastPublish=millis();
    }
  }

  void publishAlarm(String type, String detail) {
    if (!_enabled||!_mqttReady||!(_dataMask&MQTT_DATA_ALARM)) return;
    String j="{\"device\":\""+storage.getDeviceName()+"\",\"type\":\""+type+"\",\"detail\":\""+detail+"\",\"ts\":"+String(millis())+"}";
    _sendPublish(_topic+"/"+storage.getDeviceName()+"/alarm", j);
  }

  void disconnect() {
    uint8_t d[2]={0xE0,0x00}; if(_wsConn) _ws.sendBIN(d,2);
    _ws.disconnect(); _wsConn=false; _mqttReady=false; isConnected=false;
  }
};

// ============================================================
// TCP/SSL MQTT Broker (PubSubClient)
// ============================================================
class TcpMqttBroker {
public:
  bool isConnected = false;
  String label;
private:
  WiFiClient       _wifiClient;
  WiFiClientSecure _secureClient;
  PubSubClient     _client;
  bool   _useSSL=false, _enabled=false;
  bool   _discoveryDone=false;
  String _host, _user, _pass, _topic;
  int    _port, _intervalSec, _dataMask;
  unsigned long _lastPublish=0, _lastReconnect=0;

public:
  TcpMqttBroker(String lbl) : label(lbl), _client(_wifiClient) {}

  void configure(bool en, String host, int port, String user,
                 String pass, String topic, int interval, int dataMask) {
    _enabled=en; _host=host; _port=port; _user=user; _pass=pass;
    _topic=topic; _intervalSec=interval; _dataMask=dataMask;
    _useSSL=(port==8883);
    if (!en||host.length()==0) return;
    if (_useSSL) { _secureClient.setInsecure(); _client.setClient(_secureClient); }
    else         { _client.setClient(_wifiClient); }
    _client.setServer(host.c_str(), port);
    _client.setBufferSize(2048);
    _client.setKeepAlive(60);
    _client.setSocketTimeout(10);
    Serial.printf("[MQTT-%s] TCP configured: %s:%d SSL:%s\n",
      label.c_str(), host.c_str(), port, _useSSL?"yes":"no");
  }

  void handle(bool bmpActive, bool mpuActive) {
    if (!_enabled||_host.length()==0) return;
    if (WiFi.status()!=WL_CONNECTED) { isConnected=false; return; }
    if (!_client.connected()) {
      isConnected=false;
      unsigned long now=millis();
      if (now-_lastReconnect>=5000) { _lastReconnect=now; _reconnect(); }
      if (!_client.connected()) return;
    }
    _client.loop(); isConnected=true;
    if (millis()-_lastPublish>=(unsigned long)(_intervalSec*1000)) {
      String payload = buildPayload(storage.getDeviceName(), _dataMask, bmpActive, mpuActive);
      String topic   = _topic+"/"+storage.getDeviceName()+"/data";
      _client.publish(topic.c_str(), payload.c_str(), true);  // retain=true
      Serial.printf("[MQTT-%s] TCP Published %d bytes\n", label.c_str(), payload.length());
      _lastPublish=millis();
    }
  }

  void publishAlarm(String type, String detail) {
    if (!_enabled||!(_dataMask&MQTT_DATA_ALARM)||!_client.connected()) return;
    String topic=_topic+"/"+storage.getDeviceName()+"/alarm";
    String json="{\"device\":\""+storage.getDeviceName()+"\",\"type\":\""+type+"\",\"detail\":\""+detail+"\",\"ts\":"+String(millis())+"}";
    _client.publish(topic.c_str(), json.c_str());
  }

  void disconnect() { if(_client.connected()) _client.disconnect(); isConnected=false; _discoveryDone=false; }

private:
  void _reconnect() {
    String clientId="solar-"+storage.getDeviceName()+"-"+label;
    Serial.printf("[MQTT-%s] TCP konek ke %s:%d (user:%s)\n",
      label.c_str(), _host.c_str(), _port, _user.length()>0?_user.c_str():"anonymous");
    bool ok = (_user.length()>0)
      ? _client.connect(clientId.c_str(), _user.c_str(), _pass.c_str())
      : _client.connect(clientId.c_str());
    if (ok) {
      isConnected=true;
      Serial.printf("[MQTT-%s] TCP Terhubung!\n", label.c_str());
      // Discovery dulu, baru online
      if (!_discoveryDone) {
        _discoveryDone = true;
        publishDiscovery(
          [this](const String& t, const String& p){
            _client.publish(t.c_str(), p.c_str(), true);
          },
          storage.getDeviceName(), _dataMask, true, true
        );
      }
      delay(200);
      _client.publish((_topic+"/"+storage.getDeviceName()+"/status").c_str(), "online", true);
    } else {
      int rc=_client.state();
      const char* d="UNKNOWN";
      switch(rc){case -4:d="TIMEOUT";break;case -3:d="NET_FAIL";break;case -2:d="CONN_FAIL";break;
                 case  4:d="BAD_CRED";break;case 5:d="UNAUTH";break;}
      Serial.printf("[MQTT-%s] TCP Gagal rc=%d (%s)\n", label.c_str(), rc, d);
    }
  }
};

// ============================================================
// Universal MqttBroker — auto pilih mode dari port
// ============================================================
class MqttBroker {
public:
  bool isConnected = false;
  String label;
private:
  TcpMqttBroker* _tcp = nullptr;
  WsMqttBroker*  _ws  = nullptr;
  int  _mode    = MQTT_MODE_TCP;
  bool _enabled = false;
public:
  MqttBroker(String lbl) : label(lbl) {}
  ~MqttBroker() { if(_tcp) delete _tcp; if(_ws) delete _ws; }

  void configure(bool en, String host, int port, String user,
                 String pass, String topic, int interval, int dataMask) {
    _enabled=en; _mode=detectMqttMode(port);
    if (_tcp) { delete _tcp; _tcp=nullptr; }
    if (_ws)  { delete _ws;  _ws=nullptr;  }
    isConnected=false;
    if (!en||host.length()==0) { Serial.printf("[MQTT-%s] Disabled\n",label.c_str()); return; }
    const char* ms = (_mode==MQTT_MODE_WS)?"WebSocket":(_mode==MQTT_MODE_SSL)?"SSL/TCP":"TCP";
    Serial.printf("[MQTT-%s] Mode: %s (%s:%d)\n", label.c_str(), ms, host.c_str(), port);
    if (_mode==MQTT_MODE_WS) { _ws=new WsMqttBroker(label);  _ws->configure(en,host,port,user,pass,topic,interval,dataMask); }
    else                     { _tcp=new TcpMqttBroker(label); _tcp->configure(en,host,port,user,pass,topic,interval,dataMask); }
  }

  void handle(bool bA, bool bB) {
    if (!_enabled) return;
    if (_mode==MQTT_MODE_WS&&_ws)  { _ws->handle(bA,bB);  isConnected=_ws->isConnected; }
    else if (_tcp)                  { _tcp->handle(bA,bB); isConnected=_tcp->isConnected; }
  }

  void publishAlarm(String t, String d) {
    if (_mode==MQTT_MODE_WS&&_ws) _ws->publishAlarm(t,d);
    else if (_tcp)                _tcp->publishAlarm(t,d);
  }

  void disconnect() {
    if (_mode==MQTT_MODE_WS&&_ws) _ws->disconnect();
    else if (_tcp)                _tcp->disconnect();
    isConnected=false;
  }
};

MqttBroker mqttA("A");
MqttBroker mqttB("B");

// ============================================================
// MqttManager
// ============================================================
class MqttManager {
public:
  void begin() {
    mqttA.configure(storage.getMqttAEnabled(),storage.getMqttAHost(),storage.getMqttAPort(),
      storage.getMqttAUser(),storage.getMqttAPass(),storage.getMqttATopic(),
      storage.getMqttAInterval(),storage.getMqttAData());
    mqttB.configure(storage.getMqttBEnabled(),storage.getMqttBHost(),storage.getMqttBPort(),
      storage.getMqttBUser(),storage.getMqttBPass(),storage.getMqttBTopic(),
      storage.getMqttBInterval(),storage.getMqttBData());
    Serial.printf("[MQTT] Broker A: %s | Broker B: %s\n",
      storage.getMqttAEnabled()?"ON":"OFF", storage.getMqttBEnabled()?"ON":"OFF");
  }
  void handle(bool bmpActive, bool mpuActive) {
    mqttA.handle(bmpActive, mpuActive);
    mqttB.handle(bmpActive, mpuActive);
  }
  void reload() { mqttA.disconnect(); mqttB.disconnect(); begin(); }
  void publishAlarm(String t, String d) { mqttA.publishAlarm(t,d); mqttB.publishAlarm(t,d); }
  bool isAConnected() { return mqttA.isConnected; }
  bool isBConnected() { return mqttB.isConnected; }
};

MqttManager mqttMgr;
