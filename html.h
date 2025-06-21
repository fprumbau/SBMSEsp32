const char changelog[] PROGMEM = R"=====(
<b>Device info</b>: Frank's Solar Charge monitor

<li>3.0.18    (1) 04.11.23 :: Update von 2.0.11 auf 2.0.13 der ESP32 Libs
<li>3.1.0     (1) 16.11.24 :: Update aller Libs und der Boardlibs (3.0.1 auf 3.0.7)
              (2)             aus 0.9.9.88: (6) Watchdogtimer abstellen (999s, no panic handler), Syntaxupdate (diese Version wurde nie eingespielt, die 3.0.18 blieb die letzte lauffaehige Version)
<li>4.0.0     (1) 22.04.25 :: Update des Boards und alle Libs, Boardlib 3.2.0, Installation am 01.06.2025 per USB (Controller ausgebaut); LittleFS 2MB, Programmcode 2MB
              (2)             Die Anbindung des AC-Sensors ZMPT101B erfordert keine Bindung des Pins im Hauptsketch mit adcAttachPin(34); die Zeile entfaellt
<li>4.0.1     (1)             Neue Libs
<li>4.0.8     (1) 21.06.25 :: Version laeuft wieder, ledcAttach in SBMSEsp32::116 auskommentiert (3Zeilen).
             
<p>
<h3>Offen:</h3>
- Untersuchen, was der Wegfall von ledcAttach zu bedeuten hat
- Neuen Debugmodus umsetzen, wie er in der Absaugsteuerung implementiert wurde.
- Variablen aus der Global.cpp in die dafür zugehörigen Modul-Klassen verbringen.
- Neue Teslasteuerung implementieren (Bluetoothanbindung über ESP)
- Websockets über Apache reparieren, aktuell ist die Steuerung von außen nicht aufrufbar.
- Markanten kurzen User+Pass in config.json ablegen.
- Debugmeldungen erreichen den Webclient nur (z.B. bei debugRelais==1), wenn auch der Debug Web(Client) eingeschaltet ist
- Nach einem Wifi-Restart wird die Reconnectzeit korrekt mit '6.6.2023, 11:16:43' angegeben. In der UI (über Websockets) kommt jedoch "dt": "2023-06-05T22:33:14Z". Was gilt hier und was wird für
  die Kalkulation der Nacht-/Tagzeit übernommen? Wird die Anwendung neu gestartet, wird in der UI die aktuelle Zeit -1h angezeigt. Hier wird also das konfigurierte Offset +1h nicht beruecksichtigt.
  Hier wird die datetime-Variable genutzt, die auch aktualisiert wird.
<br>- Teslaintegration: https://tesla-api.timdorr.com/api-basics/authentication
)=====";

#define VERSION "4.0.9"

const char update[] PROGMEM = R"=====(
<!DOCTYPE html><html lang="de" style="height:100%;"><head>
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
<title>{title}</title>
<style>
.c{text-align: center;} 
div,input{padding:5px;font-size:1em;} 
input{width:95%;margin-bottom:5px;border-radius: 4px;} 
body{
  height:100%;
  text-align: center;
  font-family:verdana;
  background-image:url("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHCAYAAADEUlfTAAAEfnpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjarVdttuMsCP7PKmYJAcSP5WiM57w7eJc/j8YmbW9vpp078SRYQEQeREvb//81+oVH2C3kLESfvF/wuOSSZHTisj875cWN7/6ESfmRT4dAwFJQ3X/6bepn8O0cENzkl0c+hXXaidPQbeZpUPvMgs7Ui9OQys7n+ZvSHJfd3XLmu27DxMLT6PNvFxCMamCqkGwKPr5jFoUHGjWDGr6sviuBZnVonZ9ex46O7lPwjt5T7JY8+foYClr8VPBPMZp8ttexGxG694jPmR8Ezg48v8SutRpb2/bVZecRKU9zUbeljB4UC0zpGObRAl5DP4yW0CKWuAKxCjQL2kqcWBDtxo4rZ268DbryChedbBJARVZEvPOiBkmyDlB6+B03CZq0EjASXYGagi2HLzzmTWO+lSNmrgxNYRhjjPjS6BXzb9phqLWeusxLPGIFv6QnINzoyPUvtAAItxlTG/Edje7yZrkDVoGgjTBHLDAvZTdRjM/c0oGzQs8WR8u+NTjUaQAhwtwGZ1iBwOJZjT0vQSQwI44R+GR4LuqkAAE2k8rUgI1iJwSJ0ufGmMBDV0x2NkoLgDD1GgBN0gywnDPkT3AROZRNzZGZeQsWLVn26p03733wvUbloMEFCz6EEEMKOWp00aKPIcaYYk6SFCXMkk+BUkwp5YxJM0xnjM7QyLlI0eKKFV9CiSWVvCJ9Vrfa6tewxjWtuUrViu1ffQ1UY001b7whlTa32ea3sMUtbbkh15o216z5FlpsqeUDtYnqI2r8hNw1ajxR64i5oRdO1MAO4WaCezmxjhkQE8dAPHQEkNDSMVsiOycduY7ZkgSbwgSosXVwKnfEgKDbWKzxgd2J3CVuZO4j3OQ75KhD9y+Qow7dRO4rbi9Qq3mcKDoA6ruwx3TRhsLWHCIRs5S41qV3YtdF2ssC95aDdU3pXUXQtGAdwq9V6IHxsR8npZ/5cVJCx3AGAjufModcNaO0leIq48WP3O8BVZ3Hu2X5TkiX0g+EdCEFuFFdy8iiHjVf2Vm/M9grSuggdQrG+9wsB+sXjC76kNKlQvX7FG+4RPwnlwoUkutl+bJD38nEcA6MjMdhv/sUTLnlCrxhP/WZuv7s0Jg1HjkRZHilNhPhtGVjQ3+bTXSRiYLKICmeoRgO6ZZiq74j+xxsqf/ALfrTBnnXLboh9FO36K368YZb3SM5ZmP/ZON9t+j9snbtFt3C9VO36KMyeOEWPaD4A7fo8+r82i36mlwXblkvE6h0MsvE/abFQYmzeR/kbqM/6CTpXQ/4T6N73HBW1w/8u5WxxwpZYLN8V5WvqBh9V8Ks9BKmuxduedqSp41cqk+4RWCLlNTdzLie4BgZ0QIoYGrop2KAeq2rj/1duDrTAMEXIV1KXwn7rXlTl0qseielKR5LYQhx5Uu6F+7rUD/dEyj+xYXhVWrSLezcU12PKJ4Fzg4cJu3g4lLp77fJXmofGO9RN3aYv8sc6ndO3Fxu2XQGRXHNw19z+g0EzfGFJDrJyAAAAAZiS0dEAP8A/wD/oL2nkwAAAAlwSFlzAAAOwwAADsMBx2+oZAAAAAd0SU1FB+IFEAwsCt9fsxIAAABDSURBVAjXfc67DYBADATRMUf/hVAGCU1ZHgInp+Oz0kZPtjau8zAiWKOyA4wxHpiZjQDztXY3PuMf0m/VZcyEVfWKN854GDbTdSsxAAAAAElFTkSuQmCC");} button{border:0;background-color:#b2ad98;color:#545147;line-height:2.4rem;font-size:1.2rem;width:100%;} #header{height:120px;text-align: left;color:#cbc6af;background-image:url("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAA7DAAAOwwHHb6hkAAAAB3RJTUUH4gUQDDYwqX6QewAAAA1JREFUCNdjMDXWOwMAApoBY44cvvsAAAAASUVORK5CYII=");}#footer{height:24px;color:#292723;background-image:url("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAA7DAAAOwwHHb6hkAAAAB3RJTUUH4gUQDDg61yhU6wAAAA1JREFUCNdjMDXWSwMAAjQA/b8+kbgAAAAASUVORK5CYII=");
}
#body{text-align:left;display:inline-block;min-width:260px;min-height: calc(100% - 186px);
color:#545147;}
h2{margin-top: 4px;margin-left:10px;}p{margin-top: 2px;margin-bottom: 0px;margin-left: 10px;}
</style>
<script>var redirect = false;
{redirect};
if(redirect) {
  setTimeout('document.location.href="/";', 8000);
}
</script>
</head>
<body>
<div id="header">
<h2>{banner}</h2>
<p>{build}</p><p>{branch}</p><p>{deviceInfo}</p></div> 
<div id="body"><form method='POST' action='' enctype='multipart/form-data'><input type='file' name='update'><br/>
<br/><input type='submit' value='Update'></form><br>{footer}</div><div id="footer"><p>SBMS120 / LiFeOP4 6kWh</p>
</div> </body></html>
)=====";

const char html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<title>ElectroDacus</title>
<meta charset="utf-8"/>
<meta http-equiv='cache-control' content='no-cache'>
<meta http-equiv='expires' content='0'>
<meta http-equiv='pragma' content='no-cache'>
<style media='screen' type='text/css'>
meter {width:180px;height:18px;position:absolute;left:10px;}
//meter {-moz-appearance:none;appearance:none;width:180px;height:12px;position:absolute;left:10px;box-shadow:1px 2px 3px #ec6;border-radius:3px;}
#bat {height: 87px; width: 320px; top: 0px;}         
mx1{border:1px solid #fff;color:#fff;padding:0px 3px}
mn1{border:1px solid #fe9;color:#fe9;padding:0px 3px}
Val{color:#fed;}
txt{color:#a99;}
V5{color:#000;white-space:pre;}
v0{background:#fc4;}
v1{background:#e6a;}
v3{background:#0f0;}
v4{background:#f00;}
v6{background:#01f;}
v7{background:#e9f;}
lt{background: #fa6;}
.ch0{background:#f00;}
.date{font-weight:bold;color:#fff;}
.dbg{font-size:0.7em;margin-top:2px;}
div{position: relative;float:left;}
div2{position: absolute;color:#fed;line-height: 21px;}
div3{position: relative;height:180px; width:720px;float:left;font-family:Arial,serif;font-weight: bold;font-size:18px;}
div1h{position:absolute;width: 179px;height: 160px;left:360px;color:#300;background: rgba(120,90,0,0.6);}
div1m{position:absolute;width: 180px;height: 160px;left:540px;color:#300;background: rgba(30,70,0,0.5);}
div4{position:absolute;width: 236px;height: 22px;bottom:9px;color:#211;background: #fa5;}
div5{position:absolute;background: rgba(120,90,0,0.4);}
button{background:#505050;color:beige;width:92px;height:25px;border-width:1px;border-radius:13px;font-weight:bold;}
.bt{position:absolute;top:0;right:0;border-left:1px solid #505050;background-color:rgba(120,90,0,0.4);color:#ea8;width:130px;height:160px;font-size:10px;line-height:13px;}
.bs{background:#505050;color:beige;width:50px;border:1px solid white;height:38px;border-radius:5px;font-weight:bold;padding:0px;}
.onc{color:#ff0;background:#f00}
.off{color:beige;background:#505050;}
input[type=checkbox] {
     background: #505050;
     color:beige;
    -webkit-appearance: none;
    -moz-appearance: none;
    border:1px solid beige;
    margin-bottom:0px;
    height:11px;
    width: 11px;
}
input[type=checkbox]:checked {
    background-image: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHCAQAAABuW59YAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAIGNIUk0AAHolAACAgwAA+f8AAIDpAAB1MAAA6mAAADqYAAAXb5JfxUYAAAB2SURBVHjaAGkAlv8A3QDyAP0A/QD+Dam3W+kCAAD8APYAAgTVZaZCGwwA5wr0AvcA+Dh+7UX/x24AqK3Wg/8nt6w4/5q71wAAVP9g/7rTXf9n/+9N+AAAtpJa/zf/S//DhP8H/wAA4gzWj2P4lsf0JP0A/wADAHB0Ngka6UmKAAAAAElFTkSuQmCC'), linear-gradient(135deg, #B1B6BE 0%, #FFF 100%);
    background-position: center center;
    background-size: 80% 80%;
    background-repeat: no-repeat;
}
#console{
  box-shadow: inset 0px 1px 4px #666;overflow-y:scroll;
  font-size:0.6em;
  width:707px;
  height:150px;
  background-color:#505050;
  color:beige;
  padding:5px;
  border-radius:5px;
  border:1px solid #918263;
}
div::-webkit-scrollbar {
  width: 12px;
}
div::-webkit-scrollbar-track {
  -webkit-box-shadow: inset 0 0 6px rgba(0,0,0,0.3);
  border-radius: 10px;
}
</style>
</head>
<body style='background: #000;'>
<div3 style='height: 135px;'>
<canvas id='Lg' width='70' height='120' style='position:relative; top:11px; left:12px; z-index:2; float: left;'></canvas>
<div2 style="top:12px; left:360px;" id="id"><span ondblclick='updatePage();' style="font-size:32px;font-weight:Bold">SBMS120</span>
<div2 style="top:0px;left:205px;top:-6px;width:138px;font-family:'Arial';font-size:28px;color:lightgreen;background-color:#505050;border:1px solid white;padding:5px;text-align:right;white-space:pre;border-radius:5px;" title="If solar production is positive (green), the power that goes to the grid is shown, else (if red) the power taken from the grid is shown" id="lieferung">0.0 W</div2>
</div2>
<div2 style='width:350px; top:82px; left:90px; color:#be5;float:none;'>
<span style='margin-top:-20px;color:beige;' id="datetime">2019-09-17T13:29:37Z</span>
<div >www.ElectroDacus.com</div>
<div style='color:transparent; -webkit-transform: rotateX(180deg);transform: rotateX(180deg);-ms-transform:rotateX(180deg); text-shadow: 0px 0px 1px #371;' onClick="updatePage();">www.ElectroDacus.com</div></div2>
<div2 style='width:350px; top:45px; left:520px;'>
<button id="bb" onclick="updateServer(this.innerHTML);">Netzvorrang</button>
<button id="b1" style="width:47px" onclick="updateServer(this.innerHTML);">S1off</button>
<button id="b2" style="width:47px" onclick="updateServer(this.innerHTML);">S2off</button>
<br><input type='checkbox' id='dbg' onchange='updateBitset();' style="background-color:#505050;">
    <select id="dbgsel" onchange='updateFromBitset()' style="width:172px;background-color:#505050;color:beige;font-weight:bold;border-radius:5px;height:22px;margin-top:5px;">
      <option name="0">Debug Web (Client)</option>
      <option name="1">Debug Websckts (Srv)</option>
      <option name="2">Debug Json</option>
      <option name="3">Debug Relais</option>
      <option name="4">Debug SMA</option>
      <option name="5">Debug SBMS</option>
      <option name="6">Debug Charger</option>
      <option name="7">Debug Battery</option>
      <option name="8">Debug Inverter</option>
      <option name="9">Debug Konfig</option>
      <option name="10">Debug Tesla</option>
      <option name="11">Batterie Aktiv</option>
      <option name="12">Fast Response</option>
      <option name="13">Automatisches Laden</option>
      <option name="14">S1 aktiviert</option>
      <option name="15">Batteriedauerbetrieb</option>
    </select>
    <br>
    Von <input type="text" class="bs" id="sett1" value="18" onblur="updateServer(this.id);" style="width:26px;padding-left:5px;height:13pt;margin-top:5px"> pm
    bis <input type="text" class="bs" id="sett2" value="9" onblur="updateServer(this.id);" style="width:26px;padding-left:5px;height:13pt"> am
</input>
</div2>
</div3>
<div3>
<meter id='bat' min='0' low='30' max='100'></meter>
<meter style='height: 50px; left: 332px; top:18px; width: 15px;' min='2' max='8' value='0'></meter>
<div2 style='color:#030;font-size:48px;top:35px;left:120px;text-shadow: -2px -2px 2px #efc;' id='SOC' ></div2>
<div2 style='left:360px;color:#ea8;' id='d2'></div2>
<div2 style='left:430px;' id='d3'></div2>
<div2 style='left:490px;color:#888;' id='d4'></div2>
<div2 style='left:5px;top:92px;color:#a99;line-height: 25px;' id='d12'></div2>
<div2 style='left:510px;' id='mt1'></div2> 
</div3>
<div3>
<div2 style='left:5px;color:#ea8;line-height:19px' id='d6'>[Source]</div2>
<div2 style='right:580px;text-align:right;line-height:19px' id='d7'>[A]</div2>
<div2 style='right:490px;text-align:right;line-height:19px' id='d8'>[W]</div2>
<div5 style='width:250px;top:0px;height:23px;' id='d11'></div5>
<div2 style="border:1px solid #918263;left:360px;width:355px;height:160px;border-radius:5px;">
<meter style='height:10px; left: 202px; top:15px; width: 6px;' min='2' max='8' value='0'></meter>
<meter id='bat2' style='height: 30px; width: 190px; top: 2px;margin-top:2px;' min='0' low='20' max='100'></meter>
<div2 style='color:#030;font-size:24px;top:10px;left:87px;text-shadow: -2px -2px 2px #efc;' id='SOC2' >0%</div2>
<input type="checkbox" id="teslaactive" onchange='updateServer();' style="margin-top:45px;"></input>Tesla Ladestrg.

<br>
<table>
<tr>
<td colspan="2"><input type="button" class="bs" id="state" value="Status" onclick="setOn(this);updateServer(this.id);" style="width:107px"></td>
<td><input type="text" class="bs" id="setv" value="75" onclick="updateServer(this.id);" style="width:43px;padding-left:5px;"></td>
<td><input type="button" class="bs" id="set" value="Setze" onclick="setOn(this);updateServer(this.id);" style="width:50px;"></td>
</tr><tr>
<td>
<input type="button" class="bs" id="wakeup" value="Wake" onclick="setOn(this);updateServer(this.id);" style="height:18px">
<br><input type="button" class="bs" id="w2" value="Frei" onclick="setOn(this);updateServer(this.id);" style="height:18px">
</td>
<td>
<input type="button" class="bs" id="charge" value="Lade" onclick="updateServer(this.id);" style="width:52px;">
</td>
<td>
<input type="button" class="bs" id="auth" value="Auth" onclick="setOn(this);updateServer(this.id);">
</td>
<td>
<input type="button" class="bs" id="lim50" value="Lim50" onclick="setOn(this);updateServer(this.id);" style="height:18px">
<br><input type="button" class="bs" id="lim90" value="Lim90" onclick="setOn(this);updateServer(this.id);" style="height:18px">
</td>
</tr>
</table>


<div2 id="teslaout" class="bt">
...
</div2>
</div2>
</div3>
<br>
<div id="console">
</div>
<script id='smain2'>
var debug = false;
var debugJson = false;
var queryTeslaStateAfter = false;
var bitset = "0000000000000";
String.prototype.replaceAt=function(index, char) {
    var a = this.split("");
    a[index] = char;
    return a.join("");
}
//Aktualisierung vom Server    
function updateFromBitset() {
  var selIndex = document.getElementById("dbgsel").selectedIndex;
  if(debug) {
    log("SelectedIndex: " + selIndex);
  }
  if(-1 != selIndex)  { 
        if(bitset.charCodeAt(selIndex) == 48) {
            if(debug) log("Setting selIndex["+selIndex+"] to false; " + bitset.charCodeAt(selIndex));
            document.getElementById("dbg").checked = false;
        } else {
            if(debug) log("Setting selIndex["+selIndex+"] to true; " + bitset.charCodeAt(selIndex));
            document.getElementById("dbg").checked = true;
        }
  }
  for (var i = 0; i < bitset.length; i++) {
     switch(i) {
        case 0:
          if(bitset.charCodeAt(i) == 48) {
            debug = false;
          } else {
            debug = true;
          }
          if(debug) {
            console.log("Setting debug to " + debug);
          }
          break;
        case 2:
          if(bitset.charCodeAt(i) == 48) {
            debugJson = false;
          } else {
            debugJson = true;
          }
          if(debug) {
            console.log("Setting debugJson to " + debugJson);
          }
          break;
     }
  }
  if(debugJson) {
    log("Vom Server: " + bitset);
  }
}
//Aktualisierung des Bitsets auf dem Weg zum Server
function updateBitset() {
  var selIndex = document.getElementById("dbgsel").selectedIndex;
  if(-1 != selIndex)  { 
    if(document.getElementById("dbg").checked) {
        bitset = bitset.replaceAt(selIndex, '1');    
    } else {
        bitset = bitset.replaceAt(selIndex, '0');   
    }
  }
  if(debugJson) {
    log("Zum Server: " + bitset);
  }
  updateServer();
}
function log(msg) {
  cs = document.getElementById('console');
  cs.appendChild(document.createElement('br'));
  var el = document.createElement('b');  
  el.className = 'date';
  el.innerText = new Date().toLocaleString() + ':   ';
  cs.appendChild(el);
  var t = document.createTextNode(msg);
  cs.appendChild(t);
  cs.scrollTop = cs.scrollHeight;
}
function updatePage() {
  var origin = document.location.origin;
  if(origin.indexOf("prumbaum") !== -1) {
    origin = "http://sbms";
  }
  var url = origin + '/update';
  document.location.href=url;
}
//Reconnecting-websocket
!function(a,b){"function"==typeof define&&define.amd?define([],b):"undefined"!=typeof module&&module.exports?module.exports=b():a.ReconnectingWebSocket=b()}(this,function(){function a(b,c,d){function l(a,b){var c=document.createEvent("CustomEvent");
return c.initCustomEvent(a,!1,!1,b),c}var e={debug:!1,automaticOpen:!0,reconnectInterval:1e3,maxReconnectInterval:3e4,reconnectDecay:1.5,timeoutInterval:2e3};d||(d={});for(var f in e)this[f]="undefined"!=typeof d[f]?d[f]:e[f];
this.url=b,this.reconnectAttempts=0,this.readyState=WebSocket.CONNECTING,this.protocol=null;var h,g=this,i=!1,j=!1,k=document.createElement("div");k.addEventListener("open",function(a){g.onopen(a)}),k.addEventListener("close",function(a){g.onclose(a)}),
k.addEventListener("connecting",function(a){g.onconnecting(a)}),k.addEventListener("message",function(a){g.onmessage(a)}),k.addEventListener("error",function(a){g.onerror(a)}),this.addEventListener=k.addEventListener.bind(k),this.removeEventListener=k.removeEventListener.bind(k),
this.dispatchEvent=k.dispatchEvent.bind(k),this.open=function(b){h=new WebSocket(g.url,c||[]),b||k.dispatchEvent(l("connecting")),(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","attempt-connect",g.url);var d=h,
e=setTimeout(function(){(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","connection-timeout",g.url),j=!0,d.close(),j=!1},g.timeoutInterval);h.onopen=function(){clearTimeout(e),(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","onopen",g.url),
g.protocol=h.protocol,g.readyState=WebSocket.OPEN,g.reconnectAttempts=0;var d=l("open");d.isReconnect=b,b=!1,k.dispatchEvent(d)},h.onclose=function(c){if(clearTimeout(e),h=null,i)g.readyState=WebSocket.CLOSED,k.dispatchEvent(l("close"));
else{g.readyState=WebSocket.CONNECTING;var d=l("connecting");d.code=c.code,d.reason=c.reason,d.wasClean=c.wasClean,k.dispatchEvent(d),b||j||((g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","onclose",g.url),k.dispatchEvent(l("close")));
var e=g.reconnectInterval*Math.pow(g.reconnectDecay,g.reconnectAttempts);setTimeout(function(){g.reconnectAttempts++,g.open(!0)},e>g.maxReconnectInterval?g.maxReconnectInterval:e)}},h.onmessage=function(b){(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","onmessage",g.url,b.data);
var c=l("message");c.data=b.data,k.dispatchEvent(c)},h.onerror=function(b){(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","onerror",g.url,b),k.dispatchEvent(l("error"))}},1==this.automaticOpen&&this.open(!1),this.send=function(b){if(h)
return(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","send",g.url,b),h.send(b);throw"INVALID_STATE_ERR : Pausing to reconnect websocket"},this.close=function(a,b){"undefined"==typeof a&&(a=1e3),i=!0,h&&h.close(a,b)},this.refresh=function(){h&&h.close()}}
return a.prototype.onopen=function(){},a.prototype.onclose=function(){},a.prototype.onconnecting=function(){},a.prototype.onmessage=function(){},
a.prototype.onerror=function(){},a.debugAll=!1,a.CONNECTING=WebSocket.CONNECTING,a.OPEN=WebSocket.OPEN,a.CLOSING=WebSocket.CLOSING,a.CLOSED=WebSocket.CLOSED,a});
var ip = location.host;
var connection = new ReconnectingWebSocket('ws://' + ip + '/ws', null, { debug:true, reconnectInterval: 6000, reconnectDecay: 1.1, maxReconnectInterval: 10000 });
console.log('Trying to open Webclient socket');
connection.onopen = function () { 
  log('Connect: ' + new Date()); 
};
connection.onerror = function (error) { 
  console.log('wsServer Error ', error);
  log('wsServer Error ' + error);
};
//vom Server empfangen
var json = null;
connection.onmessage = function (e) { 
  data = e.data;
  switch(data[0]) {
     case '{':
          try {
            if(debugJson) {
              log(data);
            }
            json = JSON.parse(data); 
            updateUi();      
            if(json.hasOwnProperty('d')) {       
              updateSbmsData();    
            }
          } catch(error) {
            console.log( "Fehler in connection.onmessage: ", error.message );
            console.log( error.stack );
            console.log( data );
            log( error.stack );
            log( data );
          }
        break;
    default:
          if(null != data && data.length >= 3 && data.indexOf('408') !== -1) {
              setOff(document.getElementById('state'));
              setOff(document.getElementById('wakeup'));
              setOff(document.getElementById('lim50'));
              setOff(document.getElementById('lim90'));
          }
          console.log('Nachricht: ', data);
          if(debug) log(data);      
        break;          
  }
};
console.log('End trying to open Webclient socket');
var rts_reset = false;
/**
 * Akualisierung nach Empfang von Serverdaten
 * Ab 0.8.11 Abloesung der Einzelnachrichten durch JSon
 */
var string1='';
var string2='';
var pegel=0;
var temp=0;
var vorlauf='';
var ruecklauf='';
var oldT1='18';
var oldT2='9';
function updateUi() {
  if(json.hasOwnProperty("str1")) {
    string1=json.str1;
  }
  if(json.hasOwnProperty("str2")) {
    string2=json.str2;
  } 
  if(json.hasOwnProperty("pgl")) {
    pegel=json.pgl;
  }  
  if(json.hasOwnProperty("vl")) {
    vorlauf = parseFloat(json.vl).toFixed(1)
  }  
  if(json.hasOwnProperty("rl")) {
    ruecklauf = parseFloat(json.rl).toFixed(1)
  }   
  if(json.hasOwnProperty("temp")) {
    temp=json.temp;
  }   
  if(json.hasOwnProperty("dbg")) {
    var dbgBitset = json.dbg;
    bitset = dbgBitset;
    updateFromBitset();
  }
  if(json.hasOwnProperty("ta")) {    
    var teslaCtlActive = json.ta;
    document.getElementById("teslaactive").checked = teslaCtlActive;
  }
  if(json.hasOwnProperty("t1")) {    
    var t1 = json.t1;
    document.getElementById("sett1").value = t1;
    oldT1 = t1;
  }  
  if(json.hasOwnProperty("t2")) {    
    var t2 = json.t2;
    document.getElementById("sett2").value = t2;
    oldT2 = t2;
  }   
  if(json.hasOwnProperty("dt")) {
    var dt = json.dt;
    document.getElementById("datetime").innerHTML=dt;
  }
  if(json.hasOwnProperty("s1")) {
    var s1 = json.s1;
    var b1 = document.getElementById("b1");
    if(s1) {   
      b1.style.background='#f00';
      b1.style.color='#ff0';
      b1.innerHTML='S1on';
    } else {
      b1.style.background='#505050';
      b1.style.color='beige';
      b1.innerHTML='S1off';      
    }
  }
  if(json.hasOwnProperty("s2")) {
    var s2 = json.s2;
    var b2 = document.getElementById("b2");
    if(s2) {   
      b2.style.background='#f00';
      b2.style.color='#ff0';
      b2.innerHTML='S2on';
    } else {
      b2.style.background='#505050';
      b2.style.color='beige';
      b2.innerHTML='S2off';
    }
  }
  if(json.hasOwnProperty("b")) {
    var batt = json.b;
    var bb = document.getElementById("bb");
    if(batt) {
        bb.style.background='#f00';
        bb.style.color='#ff0';
        bb.innerHTML='Batterie';
      } else {
        bb.style.background='#505050';
        bb.style.color='beige';
        bb.innerHTML='Netzvorrang';
      }
  }  
  var elem = document.getElementById("lieferung");
  var sum = 0;
  if(json.hasOwnProperty("n")) {
    sum = json.n;
    elem.innerHTML=''+sum.toFixed(1)+' W';
  } else {
    elem.innerHTML='-';
  }  
  if(sum > 0) {
    elem.style.color='lightgreen';    
  } else {
    elem.style.color='#F1948A';
  }
  if(json.hasOwnProperty('fh')) {
    log("ESP32 free heap: " + json.fh);
  }
  //Antwort auf Statusabfrage vom Server darstellen
  if(json.hasOwnProperty('rts')) {
    var rts = json.rts;
    if(debug) log("json.rts="+rts);    
    setOff(document.getElementById('state'));
    setOff(document.getElementById('wakeup'));
    setOff(document.getElementById('lim50'));
    setOff(document.getElementById('lim90'));
    var to = document.getElementById("teslaout");
    if(typeof to !== 'undefined') {
      to.innerHTML=rts;
    } else {
      console.log("Element 'teslaout' not found");
    }
    console.log("Setting rts_reset to true"); //for debug only
    rts_reset = true; //Setze Resetflag
    updateServer();
  } else {
    if(rts_reset) {
      console.log("Setting rts_reset to false"); //for debug only
      rts_reset = false; //es kommen keine Teslastatusdaten mehr, also kann Resetflag zur&uuml;ckgesetzt werden
    }
  }
  if(json.hasOwnProperty('cs')) {
     if(json.cs) { //laedt
        console.log("Client: Just received from server: Charging: " + json.cs);
        setOn(document.getElementById('charge'));
     } else { //idle
        console.log("Client: Just received from server: Charging:" + json.cs);
        setOff(document.getElementById('charge'));
     }
  }
  if(json.hasOwnProperty('lm')) {
      setOff(document.getElementById('lim50'));
      setOff(document.getElementById('lim50'));
      log("Set charge limit, rc: " + json.lm);
      if(json.lm==200 && queryTeslaStateAfter) {
        setTimeout("updateServer('state');",10000);
      }
  }  
  if(json.hasOwnProperty('sc')) {
    htm('SOC2','<b>'+json.sc+'%</b>');
  }  
  if(json.hasOwnProperty('bl')) { 
    document.getElementById('bat2').value=json.bl;
  }
  if(json.hasOwnProperty('wt')) {
     setOff(document.getElementById('wakeup')); //Request ist durch
     log("Tesla wakeup StatusCode: " + json.wt);
  }  
}
function setOn(elem) {
    if(typeof elem !== 'undefined') {
        var cl = elem.classList;
        cl.remove("off");
        cl.add("onc");
    }
}
function setOff(elem) {
    if(typeof elem !== 'undefined') {
        var cl = elem.classList;
        cl.remove("onc");
        cl.add("off");
    }
}
function isOn(elem) {
    if(typeof elem !== 'undefined') {
        var cl = elem.classList;
        return cl.contains("onc");
    }  
    return false;
}
//Sende Daten zum Server
function updateServer(txt) {
  var o = {};
  
  //Charger S1,S2, Netzstatus
  if(typeof txt !== undefined) {
     switch(txt) {
        case "S1on":
          o.s1 = false;
        break;      
        case "S1off":
          o.s1 = true;
        break;
        case "S2on":
          o.s2 = false;
        break;      
        case "S2off":
          o.s2 = true;
        break;        
        case "Netzvorrang":
          o.vr = 'batt';
        break;
        case "Batterie":
          o.vr = 'netz';
        break;        
        case "state":
          o.rts = true; //request tesla state
        break;
        case "wakeup":
          o.wt = true; //request tesla wakeup
        break;
        case "auth":
          o.auth = true; //request tesla re-authentication
        break;        
        case "lim50":
          o.lm = 50; //charge limit 50%
          queryTeslaStateAfter = true;
        break;    
        case "lim90":
          o.lm = 90; //charge limit 90%
          queryTeslaStateAfter = true;
        break;
        case "set":
          o.lm = document.getElementById("setv").value;  //charge limit
          queryTeslaStateAfter = true;
          setOff(document.getElementById("set"));
        break;             
        case "sett1":
          t1 = document.getElementById("sett1").value; 
          if(t1 != '16' && t1 != '17' && t1 != '18' && t1 != '19' && t1 != '20' && t1 != '21') {
            alert('Der Wert darf nur 16-21 betragen');
            document.getElementById("sett1").value = oldT1
            return
          } 
          o.t1 = t1
        break;       
        case "sett2":
          t2 = document.getElementById("sett2").value; 
          if(t2 != '5' && t2 != '6' && t2 != '7' && t2 != '8' && t2 != '9' && t2 != '10') {
            alert('Der Wert darf nur 5-10 betragen');
            document.getElementById("sett2").value = oldT2
            return
          } 
          o.t2 = t2          
        break;             
        case "charge":    
          var charge = document.getElementById("charge");      
          if(isOn(charge)) {
            setOff(charge);
            console.log("Client: About to issue charge stop req");
            o.ch = false;    
          } else {
            setOn(charge);
            console.log("Client: About to issue charge start req");
            o.ch = true;
          }
        break;        
     }
  }
  
  o.ta = document.getElementById("teslaactive").checked;
  o.dbg=bitset;
  //Wurden Teslastatusdaten verarbeitet, sende Signal an Server, damit dieser keine Wiederholung mehr sendet
  if(rts_reset) {
    console.log("Sending rts_reset=true to server"); //for debug only
    o.rts_reset = rts_reset;
  }
  
  //geht, aber man muss einen String 'bauen'
  //var data = JSON.stringify({ "ta": document.getElementById("teslaactive").checked });
  
  var data = JSON.stringify( o );
  if(debugJson) {
    log("Sending to Server: " + data);
  }
  console.log("Sending to Server: " + data)
  connection.send(data);
}
function htm(id,s){document.getElementById(id).innerHTML =s;};
function pad(n, width, z) {z=z || '0';n=n+'';return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;}
function dcmp(p,s,d){xx=0; for (z=0;z<s;z++){xx = xx + ((d.charCodeAt((p+s-1)-z)-35)*Math.pow(91,z));}return xx;}
function fN(nm){
  return nm.toString().replace(/(\d)(?=(\d{3})+(?!\d))/g, '$1.');
}
var lg1="#B33A33B##333333B##B33A33B##B33333B##B''''''##A44A544##B44444B##;75444A##144B444##B33333B##444444B##2331$$A##B8:B:8B#";
var xsbms="01d&u%u$#'G";
var sbms2=[0,0,0,0,0,0,0,0,3,7,1,1];
function updateSbmsData(){
   
  var c = document.getElementById('Lg');
  var ctx = c.getContext('2d');
  var data = json.d;
  try {
      var SOC = dcmp(6,2,data);
      htm('SOC','<b>'+SOC+'%</b>');
      if(!isNaN(SOC)) {
        document.getElementById('bat').value=SOC;
      } else {
        document.getElementById('bat').value=0;
      }
    
      if (sbms2[10]!=1){mos(0);};if (sbms2[11]!=1){mos(1);};
      function mos(rr){document.getElementById('mo'+rr).style.background='rgba(120,90,0,0.7)';}
    
      //Logo animieren
      for (i=0;i<15;i++){setInterval(function(){lg(lg1);},500*i);};
      function lg(d){
        var k = new Date();
        var n = k.getSeconds();
        if (n>=40){n=n-40};
        if (n>=20){n=n-20};
        if (n>12){n=12};
        ctx.clearRect(0, 0, 70, 120);
        for (y=0;y<9;y++){
        for (x=0;x<7;x++){
        var pix2=((pad(((d.charCodeAt(y+(12*9))-35).toString(2)),6)).charCodeAt(x))-48;
        var pix=((pad(((d.charCodeAt(y+(n*9))-35).toString(2)),6)).charCodeAt(x))-48;
        if (pix==1){col1='#be9';col2='rgba(142,204,104,0.'}
        else if (pix2==1){col1='#694';col2='rgba(66,104,44,0.'}
        else {col1='#361';col2='rgba(42,84,36,0.'}
        ctx.fillStyle =col1;
        ctx.fillRect(x*10, y*10, 8, 8);
        if(y>=6){ctx.fillStyle =col2+(((y*2)-2)-8)+')';ctx.fillRect(x*10, (17.3-y)*10, 8, 8);}
      }}}
      
      mt8();

      function mt8() {
        var w = new Array();
        for (i=0;i<20;i++){w[i]='';}
        var bv=pv3=sv=max1=min1=0;
        var cvs = new Array(8)
        var minInd = 0;
        var maxInd = 0;
        for (a=0;a<8;a++) {
          cvs[a]=dcmp((a*2)+8,2,data)/1000;       
          if (cvs[a] < cvs[minInd]) minInd = a;
          if (cvs[a] > cvs[maxInd]) maxInd = a;
          if(debug) 
            log("Index " + a + "; cvs["+a+"] " + cvs[a] + "; minInd: " + minInd + "; maxInd: " + maxInd);
        }     
        sbms2[9]=minInd+1;
        sbms2[8]=maxInd+1;   
        var mt = document.querySelector('#mt1');
        while (mt.firstChild) {
          mt.removeChild(mt.lastChild);
        }
        for (x1=0;x1<8;x1++) {
          var n=n1='';   
          var cv=cvs[x1];
          if (sbms2[9]==x1+1){min1=cv;n='<mn1>';n1='</mn1>';};
          if (sbms2[8]==x1+1){max1=cv;n='<mx1>';n1='</mx1>';};
          w[0] +=n+'Cell. '+ (x1+1)+n1+'</br>';
          w[1] +=n+cv.toFixed(3)+n1+'</br>';
          if (sbms2[x1]!=1){w[2] +='<txt>V</txt></br>';}
          else {w[2] +='<lt><</lt></br>';};
          bv +=cv;         
          var x = document.createElement('meter');
          x.setAttribute('min',dcmp(5,2,xsbms)/100);
          x.setAttribute('max',dcmp(3,2,xsbms)/100);
          if(!isNaN(cv)) {
            x.setAttribute('value',cv);      
          } else {
            x.setAttribute('value',cv);   
          }
          x.style.top=((x1*21)+3)+'px'
          mt.appendChild(x);
        } 
        for (i=2;i<5;i++){htm('d'+i,w[i-2]);}   

        //Batt
        var n2=w[8]=w[9]='';
        var cv=dcmp(29,3,data)/1000;
        n2=data.charAt(28);
        w[8]='[A]<br style="line-height:22px;">';
        w[9]='[W]<br style="line-height:22px;">';;
        w[3] +='&nbsp;<br style="line-height:22px;">Batt</br>';
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';       
        
        //PV1
        var n2=w[8]=w[9]='';
        var cv=dcmp(32,3,data)/1000;
        pv3 +=cv;
        w[3] +='PV1</br>' ;
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';
  
        
        //PV2
        var n2=w[8]=w[9]='';
        var cv=dcmp(35,3,data)/1000;     
        pv3 +=cv;
        w[3] +='PV2</br>' ;
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';

        //Mmmmh....
        var cv=dcmp(38,3,data)/1000;
        sv=cv;

        //PV1+PV2
        var n2=w[8]=w[9]='';
        var cv=dcmp(41,3,data)/1000;
        cv=pv3;
        w[3] +='PV1+2</br>' ;
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';

        //Ext.Load
        var n2=w[8]=w[9]='';
        var cv=dcmp(47,3,data)/1000;
        cv=sv;
        w[3] +='ExtLd</br>';
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';

        //String 1/2
        w[3] +='Strg 1/2</br>';
        w[4] += string1 + 'W</br>';
        w[5] += string2 + 'W</br>';

        //Vorl./Rueckl.    
        w[3] +='VL/RL</br>';
        w[4] += vorlauf + '°C</br>';
        w[5] += ruecklauf + '°C</br>';

        //Temp./Pegel   
        w[3] +='Pgl/Tp</br>';
        w[4] += pegel + 'cm</br>';
        w[5] += temp + '°C</br>';

        //div2#d6 Spalte mit 'Batt', 'PV1' etc....
        htm('d6',w[3]);

        //div2#d7 Spalte mit Stromwerten
        htm('d7',w[4]);

        //div2#d8 Spalte mit Wattwerten
        htm('d8',w[5]);

        //Ext: <val>'+ ((dcmp(26,2,data)/10)-45).toFixed(1)+'</val>&#8451
        htm('d'+12,'Typ: LiIon <val>260</val>Ah Status: <val>'+dcmp(56,3,data)+'</val></br>'+'SBMS Temp Int: <val>'+ ((dcmp(24,2,data)/10)-45).toFixed(1)+'</val>&#8451</br>'+'BattVoltage <Val>'+ bv.toFixed(3)+'</Val>V Cell &#916 <Val>'+((max1-min1)*1000).toFixed(0)+'</Val>mV');
      }
  } catch(err) {
    log("ERROR_595: " + err.message);
    console.log("ERROR_596: " + err);
    console.log(data);
  }
}
//anfangs sollten die Checkbox nicht selektiert sein 
document.getElementById("dbg").checked=false;
</script>
</body>
</html>
)=====";
