#pragma once

const char LOG_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Solar Monitor - Log</title>
<style>
:root{
  --bg:#0f1923;--card:#1a2535;--border:#2d3f55;--text:#e0e0e0;
  --dim:#94a3b8;--accent:#f59e0b;--accent2:#38bdf8;
  --green:#22c55e;--red:#ef4444;--yellow:#facc15;
  --sans:'Segoe UI',Arial,sans-serif;--mono:'Courier New',monospace
}
*{box-sizing:border-box;margin:0;padding:0}
body{background:var(--bg);color:var(--text);font-family:var(--sans);min-height:100vh;padding-top:0}
nav{background:#111d2b;padding:12px 20px;display:flex;align-items:center;
  justify-content:space-between;border-bottom:1px solid var(--border);
  position:sticky;top:0;z-index:100}
.nav-left{display:flex;align-items:center;gap:12px}
.logo{font-size:16px;font-weight:bold;color:var(--accent)}
.badge{font-family:var(--mono);font-size:10px;padding:2px 8px;border-radius:4px;
  background:#1a2e42;color:var(--accent2);letter-spacing:1px}
.nav-tabs{display:flex;gap:4px}
.tab{padding:6px 14px;border-radius:6px;font-size:13px;cursor:pointer;border:none;
  background:transparent;color:#94a3b8;transition:all .2s}
.tab.active{background:var(--accent);color:#0f1923;font-weight:bold}
.tab:hover:not(.active){background:#1e3a5f;color:var(--text)}
.conn-dot{width:8px;height:8px;border-radius:50%;background:var(--green);
  box-shadow:0 0 6px var(--green);animation:pulse 2s infinite;display:inline-block}
.conn-dot.off{background:var(--red);box-shadow:0 0 6px var(--red);animation:none}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}
.uptime{font-family:var(--mono);font-size:11px;color:var(--dim)}
/* CONTENT */
.content{padding:16px;max-width:960px;margin:0 auto}
/* TOOLBAR */
.toolbar{display:flex;align-items:center;gap:8px;margin-bottom:12px;flex-wrap:wrap}
.filter-btn{padding:5px 12px;border-radius:16px;font-size:12px;font-weight:bold;
  cursor:pointer;border:1px solid var(--border);background:var(--card);
  color:var(--dim);transition:all .2s}
.filter-btn.active{border-color:var(--accent);color:var(--accent);background:#1a2e1a}
.filter-btn.f-kwh.active   {border-color:#22c55e;color:#22c55e;background:#052e16}
.filter-btn.f-mqtt.active  {border-color:#38bdf8;color:#38bdf8;background:#0c1a2e}
.filter-btn.f-sensor.active{border-color:#a855f7;color:#a855f7;background:#1a0a2e}
.filter-btn.f-wifi.active  {border-color:#facc15;color:#facc15;background:#2e2a06}
.filter-btn.f-sys.active   {border-color:#f59e0b;color:#f59e0b;background:#2e1a06}
.spacer{flex:1}
.btn-clear{padding:5px 14px;border-radius:7px;font-size:12px;background:#450a0a;
  border:1px solid var(--red);color:var(--red);cursor:pointer;transition:all .2s}
.btn-clear:hover{background:var(--red);color:#fff}
.btn-refresh{padding:5px 14px;border-radius:7px;font-size:12px;background:var(--card);
  border:1px solid var(--border);color:var(--dim);cursor:pointer;transition:all .2s}
.btn-refresh:hover{border-color:var(--accent);color:var(--accent)}
/* LOG TABLE */
.log-wrap{background:var(--card);border-radius:10px;overflow:hidden}
.log-header{display:grid;grid-template-columns:160px 70px 1fr;gap:0;
  padding:8px 12px;background:#0f1923;border-bottom:1px solid var(--border);
  font-size:11px;color:var(--dim);font-weight:bold;letter-spacing:1px}
.log-body{max-height:65vh;overflow-y:auto}
.log-row{display:grid;grid-template-columns:160px 70px 1fr;gap:0;
  padding:7px 12px;border-bottom:1px solid #1a2535;font-size:12px;
  transition:background .1s}
.log-row:hover{background:#1e2d3d}
.log-row:last-child{border-bottom:none}
.log-time{font-family:var(--mono);color:var(--dim);font-size:11px;padding-right:8px}
.log-cat{font-weight:bold;font-size:11px;padding-right:8px}
.log-msg{color:var(--text);font-family:var(--mono);font-size:12px;word-break:break-word}
/* Warna kategori */
.cat-kwh   {color:#22c55e}
.cat-mqtt  {color:#38bdf8}
.cat-sensor{color:#a855f7}
.cat-wifi  {color:#facc15}
.cat-sys   {color:#f59e0b}
/* Empty state */
.empty{text-align:center;padding:40px;color:var(--dim);font-size:13px}
/* Status bar */
.status-bar{display:flex;justify-content:space-between;align-items:center;
  padding:6px 12px;background:#0f1923;border-top:1px solid var(--border);
  font-size:11px;color:var(--dim)}
/* Auto refresh toggle */
.auto-wrap{display:flex;align-items:center;gap:6px;font-size:12px;color:var(--dim)}
.toggle-mini{width:32px;height:18px;background:#2d3f55;border-radius:9px;
  position:relative;cursor:pointer;transition:background .2s}
.toggle-mini.on{background:var(--accent)}
.toggle-mini::after{content:'';position:absolute;width:14px;height:14px;
  background:#fff;border-radius:50%;top:2px;left:2px;transition:left .2s}
.toggle-mini.on::after{left:16px}
@media(max-width:500px){
  .log-header,.log-row{grid-template-columns:1fr}
  .log-time,.log-cat{display:inline;margin-right:6px}
}
</style>
</head>
<body>
<nav>
  <div class="nav-left">
    <div style="display:flex;align-items:center;gap:8px">
      <div class="logo">&#9728; SolarMonitor</div>
      <div class="badge">LOG</div>
    </div>
    <div class="nav-tabs">
      <button class="tab" onclick="location.href='/'">Dashboard</button>
      <button class="tab" onclick="location.href='/setting'">Setting</button>
      <button class="tab" onclick="location.href='/system'">System</button>
      <button class="tab active">Log</button>
    </div>
  </div>
  <div style="display:flex;align-items:center;gap:6px">
    <span class="uptime" id="uptime">--:--:--</span>
    <span class="conn-dot" id="conn-dot"></span>
  </div>
</nav>

<div class="content">
  <!-- TOOLBAR -->
  <div class="toolbar">
    <button class="filter-btn active" id="f-all" onclick="setFilter('all')">Semua</button>
    <button class="filter-btn f-kwh"    id="f-kwh"    onclick="setFilter('kWh')">&#9889; kWh</button>
    <button class="filter-btn f-mqtt"   id="f-mqtt"   onclick="setFilter('MQTT')">&#128225; MQTT</button>
    <button class="filter-btn f-sensor" id="f-sensor" onclick="setFilter('SENSOR')">&#128268; Sensor</button>
    <button class="filter-btn f-wifi"   id="f-wifi"   onclick="setFilter('WiFi')">&#128246; WiFi</button>
    <button class="filter-btn f-sys"    id="f-sys"    onclick="setFilter('SYS')">&#9881; SYS</button>
    <div class="spacer"></div>
    <div class="auto-wrap">
      <span>Auto</span>
      <div class="toggle-mini on" id="auto-toggle" onclick="toggleAuto()"></div>
    </div>
    <button class="btn-refresh" onclick="loadLog()">&#8635; Refresh</button>
    <button class="btn-clear" onclick="clearLog()">&#128465; Clear</button>
  </div>

  <!-- LOG TABLE -->
  <div class="log-wrap">
    <div class="log-header">
      <div>WAKTU</div>
      <div>KATEGORI</div>
      <div>PESAN</div>
    </div>
    <div class="log-body" id="log-body">
      <div class="empty">Memuat log...</div>
    </div>
    <div class="status-bar">
      <span id="log-count">0 entri</span>
      <span id="log-ntp">NTP: --</span>
    </div>
  </div>
</div>

<script>
let _filter  = 'all';
let _autoOn  = true;
let _autoTimer = null;

function setFilter(cat) {
  _filter = cat;
  // Update button styles
  ['all','kwh','mqtt','sensor','wifi','sys'].forEach(k => {
    const btn = document.getElementById('f-'+k);
    if (btn) btn.classList.remove('active');
  });
  const key = cat === 'all' ? 'all' : cat.toLowerCase();
  const btn = document.getElementById('f-'+key);
  if (btn) btn.classList.add('active');
  loadLog();
}

function toggleAuto() {
  _autoOn = !_autoOn;
  document.getElementById('auto-toggle').className = 'toggle-mini' + (_autoOn ? ' on' : '');
  if (_autoOn) startAuto(); else stopAuto();
}

function startAuto() {
  stopAuto();
  _autoTimer = setInterval(loadLog, 3000);
}

function stopAuto() {
  if (_autoTimer) { clearInterval(_autoTimer); _autoTimer = null; }
}

function catClass(c) {
  const map = {kWh:'kwh',MQTT:'mqtt',SENSOR:'sensor',WiFi:'wifi',SYS:'sys'};
  return 'cat-' + (map[c] || 'sys');
}

function loadLog() {
  const url = '/api/log?cat=' + encodeURIComponent(_filter);
  fetch(url)
    .then(r => r.json())
    .then(data => {
      const body = document.getElementById('log-body');
      if (!data || data.length === 0) {
        body.innerHTML = '<div class="empty">Belum ada log' +
          (_filter !== 'all' ? ' untuk kategori ' + _filter : '') + '</div>';
        document.getElementById('log-count').textContent = '0 entri';
        return;
      }
      let html = '';
      data.forEach(e => {
        html += `<div class="log-row">
          <div class="log-time">${e.t}</div>
          <div class="log-cat ${catClass(e.c)}">[${e.c}]</div>
          <div class="log-msg">${escHtml(e.m)}</div>
        </div>`;
      });
      body.innerHTML = html;
      document.getElementById('log-count').textContent = data.length + ' entri';
      document.getElementById('conn-dot').className = 'conn-dot';
    })
    .catch(() => {
      document.getElementById('conn-dot').className = 'conn-dot off';
    });

  // Update uptime
  fetch('/api/data')
    .then(r=>r.json())
    .then(d=>{
      const s=parseInt(d.uptime)||0;
      const h=Math.floor(s/3600),m=Math.floor((s%3600)/60),ss=s%60;
      document.getElementById('uptime').textContent=
        String(h).padStart(2,'0')+':'+String(m).padStart(2,'0')+':'+String(ss).padStart(2,'0');
      document.getElementById('log-ntp').textContent = 'NTP: ' + (d.ntp_time || '--');
    }).catch(()=>{});
}

function clearLog() {
  if (!confirm('Hapus semua log?')) return;
  fetch('/api/log/clear', {method:'POST'})
    .then(() => loadLog());
}

function escHtml(s) {
  return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
}

// Init
loadLog();
startAuto();
</script>
</body>
</html>
)rawliteral";
