#pragma once

// ============================================================
// SOLAR MONITOR — web_login.h
// Halaman login dengan session token
// ============================================================

const char LOGIN_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Solar Monitor - Login</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:'Segoe UI',Arial,sans-serif;background:#0f1923;color:#e0e0e0;
  min-height:100vh;display:flex;align-items:center;justify-content:center;padding:16px}
.card{background:#1a2535;border-radius:12px;padding:32px 28px;width:100%;max-width:380px;
  box-shadow:0 8px 32px rgba(0,0,0,.5);border:1px solid #2d3f55}
.logo{text-align:center;margin-bottom:28px}
.logo-icon{font-size:36px;color:#f59e0b;display:block;margin-bottom:8px}
.logo-text{font-size:22px;font-weight:bold;color:#f59e0b;letter-spacing:1px}
.logo-sub{font-size:12px;color:#64748b;margin-top:4px}
label{display:block;font-size:11px;color:#94a3b8;margin-bottom:4px;margin-top:16px}
.input-wrap{position:relative}
input{width:100%;padding:10px 12px;background:#0f1923;border:1px solid #2d3f55;
  border-radius:7px;color:#e0e0e0;font-size:14px;outline:none;transition:border .2s}
input:focus{border-color:#f59e0b}
.show-btn{position:absolute;right:10px;top:50%;transform:translateY(-50%);
  background:none;border:none;color:#64748b;cursor:pointer;font-size:16px;padding:2px}
.show-btn:hover{color:#f59e0b}
.btn-login{width:100%;margin-top:24px;padding:11px;background:#f59e0b;border:none;
  border-radius:7px;color:#0f1923;font-size:14px;font-weight:bold;cursor:pointer;
  transition:all .2s;letter-spacing:.5px}
.btn-login:hover{background:#fbbf24}
.btn-login:disabled{background:#374151;color:#6b7280;cursor:not-allowed}
.error{display:none;margin-top:14px;padding:10px 12px;background:#450a0a;
  border-radius:7px;color:#fca5a5;font-size:13px;text-align:center}
.spinner{display:inline-block;width:14px;height:14px;border:2px solid #0f192380;
  border-top-color:#0f1923;border-radius:50%;animation:spin .6s linear infinite;
  vertical-align:middle;margin-right:6px}
@keyframes spin{to{transform:rotate(360deg)}}
.footer{text-align:center;margin-top:20px;font-size:11px;color:#374151}
</style>
</head>
<body>
<div class="card">
  <div class="logo">
    <span class="logo-icon">&#9728;</span>
    <div class="logo-text">SolarMonitor</div>
    <div class="logo-sub">Masuk untuk melanjutkan</div>
  </div>
  <label for="usr">Username</label>
  <input type="text" id="usr" placeholder="admin" autocomplete="username">
  <label for="pwd">Password</label>
  <div class="input-wrap">
    <input type="password" id="pwd" placeholder="••••••" autocomplete="current-password"
      onkeydown="if(event.key==='Enter')doLogin()">
    <button class="show-btn" type="button" onclick="togglePwd()" id="eye-btn">&#128065;</button>
  </div>
  <button class="btn-login" onclick="doLogin()" id="btn-login">Masuk</button>
  <div class="error" id="err-msg">Username atau password salah</div>
  <div class="footer">Solar Monitor v2.0.0</div>
</div>
<script>
function togglePwd() {
  const i = document.getElementById('pwd');
  i.type = i.type === 'password' ? 'text' : 'password';
}
function doLogin() {
  const btn = document.getElementById('btn-login');
  const err = document.getElementById('err-msg');
  const usr = document.getElementById('usr').value.trim();
  const pwd = document.getElementById('pwd').value;
  if (!usr || !pwd) { showErr('Username dan password wajib diisi'); return; }
  btn.disabled = true;
  btn.innerHTML = '<span class="spinner"></span>Memeriksa...';
  err.style.display = 'none';
  fetch('/api/login', {
    method: 'POST',
    headers: {'Content-Type':'application/json'},
    body: JSON.stringify({usr, pwd})
  }).then(r=>r.json()).then(d=>{
    if (d.success) {
      // Simpan token di localStorage sebagai backup
      localStorage.setItem('sm_token', d.token);
      window.location.href = '/';
    } else {
      showErr('Username atau password salah');
      btn.disabled = false;
      btn.innerHTML = 'Masuk';
    }
  }).catch(()=>{
    showErr('Koneksi terputus, coba lagi');
    btn.disabled = false;
    btn.innerHTML = 'Masuk';
  });
}
function showErr(msg) {
  const e = document.getElementById('err-msg');
  e.textContent = msg;
  e.style.display = 'block';
}
// Auto focus username
window.onload = () => document.getElementById('usr').focus();
</script>
</body>
</html>
)rawliteral";
