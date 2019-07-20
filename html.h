const char changelog[] PROGMEM = R"=====(
<b>Device info</b>: Frank's Solar Charger Monitor
<li> Integrierte Schaltung der beiden Solarlader, WebSocketServer als WebCom gekapselt
<li> Errormodus wird bei fehlenden SBMS-Paketen nun wieder aktiviert.    
<li> Interruptbehandlung (Taster) wieder akiviert.  
<li> Wenn debug sread nur ausgeben, wenn Inhalt vorhanden.  
<li> Fix input Pullup fuer Taster auf GPIO19.
<li> Acebutton und ButtonConfig zur Entprellung einsetzen.
<li> ToggleCharger-Methoden.
<li> S1 / S2 nun manuell zu schalten.     
<li> Initmeldungen, wenn neuer Client connected sollte nicht an alle gehen. 
<li> Initmeldungen fuer debug/debug2 an neue Clients. 
<li> Einbinden von JSON zwischen Server -> Client.
<li> Automatisches Ein-/Ausschalten der Charger.
<li> OTA-Updaterklasse uebernommen, Hauptklasse aufgeraeumt.
<li> Debugausgabe Wirkleistung/Lieferung in Webbrowser (d2).
<li> OTAUpdater mit Debugsettings ueber d2.
<li> ESP32OTA kann nun auf vars (debug2 und udp) zugreifen, update geht (vorher Reset empfehlenswert!).
<li> Werden S1 oder S2 ueber SMA.cpp geschaltet, muss s1 an, s2 aus etc. uebermittelt werden.
<li> Globale Objekte mit global.h.
<li> Variablenupdats nun nur noch mittels JSon
<li> Moeglichkeit, WiFi mittels 'restart wifi' ueber serielle Schnittstelle zu restarten     
<li> Wifi restartet, wenn 10s kein UDP-Paket des SMA-Energymeters empfangen wurde
<li> Umstellung auf AsyncWebserver inkl. Async OTA
<li>OTA.h/OTA.cpp mit ESP32OTA.h/ESP32OTA.cpp verschmolzen
<li>Vars.h in global.h ueberfuehrt, updaterui.h und webpage.h in html.h konsolidiert
<li>Eigene IP in JavaScript mit location.host auslesen und verwenden
<li>Battery.h/.cpp in wechselrichter.h/.cpp ueberfuehrt
<li>Methoden starteBatterie und starteWechselrichter in inverter-Klasse uebernommen
<li>Websocket-Eventhandler ueber Wrappermethode in WebCom verschoben
<li>SOC-Variable und cv (cell voltages) in global.h/.cpp uebernommen
<li>OTA mit Reporting und Abschlusseite (OK/Failed) geht jetzt
<li>Funktion readSBMS nun in SBMS.h/.cpp
<li>Authentifizierung fuer ESPOTA und AsyncWebserver (auskommentiert), Zugriff ueber / statt /sbms
<li>Ein Schaltvorgang auf s1 und s2 sollte maximal alle 60s erlaubt werden.
<li>checkValues mit led-Methoden in Inverterklasse verlegt.
<li>Code zum behandeln des Inverter-Toggle-Tasters in Inverterklasse verlegt
<li>Nun 60s, bis WiFi wieder aufgebaut wird (UDP miss)
<li>Feld fuer Lieferung / Bezug anzeigen
<li>SBMS-Daten werden nun auch per JSon uebermittelt
<li>Webseite aufraeumen. Doppelklick auf Dbg1/Dbg2 leitet auf Updateseite, erfolgr. Update nach 3s zur SBMS-Seite
<li>Low-Meter von 20% auf 30% geaendert, statt 110AH nun 100AH, Lieferung Schriftformatierung
<li>NTPClient zugefuegt, um im inverter.check() ein automatische Schaltung des Verbrauchs zu ermoeglichen
<li>Zwischen 20 Uhr und 5 Uhr morgens Batteriebetrieb versuchen (nur wenn state of charge > limit)
<li>Staendige Datumsanzeige in der Webseite
<li>Fehler in 0.9.9.30 saugte Batterie leer, solange SOC_LIMIT OK, ignorierte aber Zelleinzelspannungen
<li>Aut. WiFi reconnect ging nicht, darum erstmal herausgenommen (SMA.cpp); Update-Site aus Location beruecksichtigt 
<li>Schaltschwelle CV low von 2850mV auf 3000mV erhoeht (spaeter weiter auf 3100 mit 0.9.9.38)
<li>Hysterese fuer SOC_LIMIT (SOC_HYST mit 5%) hinzugefuegt
<li>0.9.9.36: 100mV Hysterese bei Zellspannungen verhindert automatisches Anschalten bei Leerlauf
<li>0.9.9.37: Faellt die Spannung einer Zelle auf <2,7V, dann wird der Lader S2 f&uuml;r 5Min aktiviert     
<li>0.9.9.38: Weitere Modularisierung mit Charger und Batteryklasse, bei niedriger CV nun start S2 fuer jeweils 5Min.
<li>0.9.9.39: Nun statt Wifi-Reset sma.reset (udp reinit); Fixed Fehler, es wurden nur 7 Zellen ueberwacht
<li>0.9.9.40: Relais 3+4 verdrahtet, Umstrukturierung, mit Relais 4 werden nun die Luefter geschaltet
<li>0.9.9.41: Batterie erst wieder moeglich, wenn Zellspannungen LOW_VOLTAGE_MILLIS + CV_HYST (3150mV) betragen
<li>0.9.9.42: SBMS-Temperatur wird ausgelesen und verhindert ab 40 Grad Celsius ein Abschalten der Luefter
<li>0.9.9.43: Immer, wenn seit dem letzten Restart mehr als 5h vergangen sind (Ausnahme: 0Uhr-Uebergang), wird ESP.resta  rt() ausgeloest
<li>0.9.9.44: Da das Relais zum Charger S1 (HLG600, 600W) ausgewechselt werden musste, wird S1 nun seltener geschaltet (10min statt 60s)
<li>0.9.9.45: Zellspannungsdifferenzen Min-Max werden wieder richtig angezeigt
<li>0.9.9.46: Auswertung Webseite ging nur bis Zelle 7, Notladungsautomatik in battery.cpp schaltete S2 nach 5 Minuten immer wieder ab
<li>0.9.9.47: Min-/Maxzellspannung um einen Index versetzt; Seit 0.9.9.45 Fehler bei PV1 und PV2 Poweranzeige (weil eigentlich Zellspannungen 2/3)
<li>0.9.9.48: Ueber das freie Relais RELAY_3 zum Remote ON/OFF von S1
<li>0.9.9.49: Charger S1 isChargerOn wurde nicht korrekt abgefragt. Hier wurde nach erstmaligem einschalten immer isOn gemeldet, obwohl S1 off war. Temp Limit Luefter 40->35
<li>0.9.9.50: (1) Die fuer beide Lader zustaendige Methode charger.isOn hat Relais_3 (HLG600 nicht beruecksichtigt), der Charger S1 (R3) konnte nicht abgeschaltet werden, da R3 auf LOW statt auf HIGH geschaltet wurde.
<li>0.9.9.50: (2) Ist noch ein Lader an, aber der StateOfCharge >=99% und die Temperatur <35 Grad, schalte die Luefter ab
<li>0.9.9.51: Statt wie bei S1 mind. 60s Idle zu bleiben, kann S2 nun schon nach 30s wieder aktiv geschaltet werden
<li>0.9.9.52: Wird Luefter wegen Soc und Temp abgeschaltet, dann sollte dies nur einmal erfolgen und danach das Flag fansRunning beachtet werden
<li>0.9.9.53: (1) Version 51,52 (nach .48) haben Charger nicht mehr geschaltet; in charger.cpp war an 3 Stellen die statt isChargerOn(nr) nur (nr) verblieben; Debug2 ueberarbeitet.
<li>0.9.9.53: (2) Beide Charger wurden erst nach ihrerer minimalen Ruhezeit gestartet (S1==10min, S2==30s), dies wird beim ersten Mal umgangen. S1 Ruhezeit von 10min auf 60s verringert.
<li>0.9.9.54: (1) Moeglichkeit testFixed (Datasimulation SBMS120) zu aktivieren.
<li>0.9.9.54: (2) Moeglichkeit, bei test==on die pwm-Werte von gpio05/25/26 zu setzen.
<li>0.9.9.55: Beruecksichtigen, dass S2 nun auch 600W liefert, Umschaltzeiten etwas dehnen; ArduinoJson6
<li>0.9.9.56: Sind die Lader aktiv, der Ladestand ist aber >=99%, schalte die Luefter nicht ein (sondern ab)
<li>0.9.9.57: LastHourRestart wurde nie verwendet, da immer -1 und damit <0, jetzt wird jeden Morgen gegen 5 neu gestartet
<li>0.9.9.58: Restart um 6,
<li>0.9.9.59: Schalte Luefter NUR ein, wenn Temperatur ueber 35°C (battery.cpp), Git-Upd ESP32 Libs, Kompilat damit 844 statt 841 gross
<li>0.9.9.60: GIIO5 schaltet nun per PWM den HLG600B von 0..546W (charger.cpp, calculateDc)
<li>0.9.9.61: Der mit 0.9.9.58 eingerf&uuml;hrte Restart muss auf die Stunde UND die Minute eingeschraenkt sein, da sonst eine Stunde neu gestartet wird
<li>0.9.9.62: Die Methode charger.checkOnIncome() sollte nur noch maximal alle CHECK_INCOME_MIN_INTERVAL_MILLIS laufen
<li>0.9.9.63: Eine DC-Berchnung sollte auch den aktuellen Wert des Ladestroms mit inkludieren
<li>0.9.9.65: Ein favicon.ico wird nun &uuml;ber SPIFFS geserved
<li>0.9.9.66: Fehler in charger.cpp verhinderte Abschaltung von S1 (HLG600A), wenn netto negativ     
<li>0.9.9.67: Pruefinterval SOC und Zellspannungen von 3 auf 10s erhoeht    
<li>0.9.9.68: Skipmeldungen nur auf Serial, nicht WS, Html-Konsolenlogging mit append, nicht mit innerHtml (Seite steht sonst nach einigen hundert Zeilen)      
<li>0.9.9.69: Ist Netto positiv, aber der Inverter laeuft, sollte dieser gestoppt werden, BEVOR die Charger geschaltet werden  
<li>0.9.9.70: Beim Schalten des Netzvorrangs einen Offset von 100W Lieferung beachten (wenn weniger als 100W, dann wird Batterie nicht abgeschaltet)       
<li>0.9.9.71: S1 vor S2 bewerten und schalten. Damit l&auml;sst muss der Nettoabzug (600W) nicht mehr berechnet werden, S2 ist immer als Tuning der Lieferung vorhanden, nicht nur zwischen 0...600W.
<li>0.9.9.72: Vor der Bewertung (charger.checkOnIncome) f&uuml;r S1 und S2 sollte geprueft werden, ob die Batterie genutzt wird (Upload geht NICHT, SBMS einlesen auch nicht; Was hat sich hier geändert?; => hier war sbms.readSbms();  auskommentiert!)
<li>0.9.9.73: <Wrover-B> Switch auf Wrover-B, wo SBMS-In(RX2) auf GPIO16 nicht belegt und auf Platinenrückseite auf GPIO15 umgelegt werden muss. Wird da eine Brücke zwischen GPIO15 und GPIO16 gelegt, m&uuml;ssten beide Chips arbeiten. (Upload kein Binary)
<li>0.9.9.74: <Wroom> isAlive Url geschaltet: /lbprobe liefert 'online', Rollback 0.9.9.72 (charger.cpp), es darf auch im Batterieberieb geladen werden (Upload kein Binary)
<li>0.9.9.75: Loop0-Taskmethode integriert, um bisher ungenutzten Core zu nutzen (Upload kein Binary)
<li>0.9.9.76: Beginn der Steuerung der Teslaladeregelung, Tesla.h und Tesla.cpp hinzugefuegt, Status, Wakeup, Ladestart und Ladestop, alles implementiert, aber noch nicht angesteuert (Upload geht, SBMS einlesen aber nicht mehr => hier war sbms.readSbms();  auskommentiert!)
<li>0.9.9.77: Verschiedene Optimierungen wg. Stringnutzung, Changelog wurde in html.h verlegt; Anhebung des Uploadmaximums fuer OTA-Sketches von 1048576 auf 1248576. Das zul&auml;ssige Maximum sollte noch bestimmt werden.
<li>0.9.9.78: Der mit 0.9.9.58 eingef&uuml;hrte ESP.restart() um 06:00 morgens wird wieder herausgenommen.
<li>0.9.9.79: In charger.cpp wurde nun die Tesla-Ladesteuerung aktiviert, bei >500 und allen Ladern an wird nun versucht, den Ladevorgang zu starten, bei <-1000W wird gestoppt. Minimale Schaltzeit ist 5Min.
<li>0.9.9.80: Beim startCharge wird nun zwischen Limitsetzung und Chartstart 8s statt 5s gewartet. Eine Schaltung bedingt nun ausserdem mindestens 10 positive Messungen.
<li>0.9.9.81: Tesla Chargestate zeichnet nun die wichtigsten Grössen auf.
<li>0.9.9.82: Anfang eines GUI-Controls (Web) um die Teslasteuerung ein-/bzw. auszuschalten
<li>0.9.9.83: (1) Das Flag teslaCtrlActive wird nun permanent im SPIFFS abgelegt
<li>0.9.9.83: (2) Die Konfiguration wird nun lokal im CFG vorgehalten, Konfig nun &uuml;ber Kommandozeile speicherbar
<li>0.9.9.83: (3) Commandline auf Task0
<li>0.9.9.83: (4) Add Try-Catch in Webseite bei SBMS-Verarbeitung
<li>0.9.9.83: (5) Nach setzen des Testdatenschalters wird UI aktualisiert
<li>0.9.9.83: (6) Debug / Debug2 werden nun auch per JSon aus der Webansicht aktualisiert (aber nicht im SPIFFS abgespeichert)
)=====";

#define VERSION "0.9.9.83"

const char update[] PROGMEM = R"=====(
<!DOCTYPE html><html lang="en" style="height:100%;"><head>
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
color:#545147;}h2{margin-top: 4px;margin-left:10px;}p{margin-top: 2px;margin-bottom: 0px;margin-left: 10px;}
</style>
<script>var redirect = false;
{redirect};
if(redirect) {
  setTimeout('document.location.href="/sbms";', 3000);
}
</script>
</head>
<body>
<div id="header">
<h2>{banner}</h2>
<p>{build}</p><p>{branch}</p><p>{deviceInfo}</p></div> 
<div id="body"><form method='POST' action='' enctype='multipart/form-data'><input type='file' name='update'><br/>
<br/><input type='submit' value='Update'></form><br>{footer}</div><div id="footer"><p>SBMS120 / LiFeOP4 2,7kWh</p>
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
<!--meta http-equiv='refresh' content='3'-->
<style media='screen' type='text/css'>
meter {-moz-appearance:none;appearance:none;width:180px;height:12px;position:absolute;left:10px;box-shadow:1px 2px 3px #ec6;}
.bar{position:absolute;bottom:0px;display:inline-block;}
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
#console{box-shadow: inset 0px 1px 4px #666;overflow-y:scroll;font-size:0.7em}
div{position: relative;float:left;}
div2{position: absolute;color:#fed;line-height: 21px;}
div3{position: relative;height:180px; width:720px;float:left;font-family:Arial,serif;font-weight: bold;font-size:18px;}
div1h{position:absolute;width: 179px;height: 160px;left:360px;color:#300;background: rgba(120,90,0,0.6);}
div1m{position:absolute;width: 180px;height: 160px;left:540px;color:#300;background: rgba(30,70,0,0.5);}
div4{position:absolute;width: 236px;height: 22px;bottom:9px;color:#211;background: #fa5;}
div5{position:absolute;background: rgba(120,90,0,0.4);}
button{color:#b50;background:#D8BFD8;border:2px solid white;width:85px;height:22px;}
</style>
</head>
<body style='background: #000;'>
<div3 style='height: 135px;'>
<canvas id='Lg' width='70' height='120' style='position:relative; top:11px; left:12px; z-index:2; float: left;'></canvas>
<div2 style="top:12px; left:360px;" id="id">SBMS120
<div2 style="top:0px;left:200px;top:-6px;width:138px;font-family:'Arial';font-size:28px;color:lightgreen;background-color:#505050;border:1px solid white;padding:5px;text-align:right;white-space:pre" title="If solar production is positive (green), the power that goes to the grid is shown, else (if red) the power taken from the grid is shown" id="lieferung">0.0 W</div2>
</div2>
<div2 style='width:350px; top:82px; left:90px; color:#be5;float:none;'>
<div2 style='margin-top:-20px' id="datetime">2018-12-27</div2>
<div >www.ElectroDacus.com</div>
<div style='color:transparent; -webkit-transform: rotateX(180deg);transform: rotateX(180deg);-ms-transform:rotateX(180deg); text-shadow: 0px 0px 1px #371;' onClick="updatePage();">www.ElectroDacus.com</div></div2>
<div2 id="demo"></div2>
<div2 style='width:350px; top:82px; left:520px;'>
<button id="bb" onclick="toggleBattery(this.innerHTML);">Netzvorrang</button>
<button id="b1" style="width:47px" onclick="toggleS1(this.innerHTML);">S1off</button>
<button id="b2" style="width:47px" onclick="toggleS2(this.innerHTML);">S2off</button>
<br><input type='checkbox' id='dbg1' onchange='updateServer();'>&nbsp;<span ondblclick='updatePage();'>Dbg1</span></input>
<input type='checkbox' id='dbg2' onchange='updateServer();'>&nbsp;<span ondblclick='updatePage();'>Dbg2</span></input>
</div2>
</div3>
<div3>
<meter id='bat' style='height: 70px; width: 320px; top: 9px;' min='0' low='30' max='100'></meter>
<meter style='height: 50px; left: 332px; top:18px; width: 15px;' min='2' max='8' value='0'></meter>
<div2 style='color:#030;font-size:48px;top:35px;left:120px;text-shadow: -2px -2px 2px #efc;' id='SOC' ></div2>
<div2 style='left:360px;color:#ea8;' id='d2'></div2>
<div2 style='left:430px;' id='d3'></div2>
<div2 style='left:490px;color:#888;' id='d4'></div2>
<div2 style='left:5px;top:92px;color:#a99;line-height: 25px;' id='d12'></div2>
<div2 style='left:510px;' id='mt1'></div2> 
</div3>
<div3>
<div2 style='left:5px;color:#ea8;top:22px;'id='d6'></div2>
<div2 style='right:530px;text-align: right;'id='d7'></div2>
<div2 style='right:430px;text-align: right;'id='d8'></div2>
<div2 style='right:220px;text-align: right;'id='d9'></div2>
<div2 style='right:10px;text-align: right;'id='d10'></div2>
<div5 style='width: 360px;top:0px;height:22px;text-align: right;'id='d11'></div5>

<div2 style="border:1px solid #505050;left:360px;width:355px;height:160px;">
<input type="checkbox" id="teslaactive" onchange='updateServer();'></input>Tesla Steuerung aktiv
</div2>

</div3>

<br>
<div id="console" style="width:707px;height:150px;background-color:#505050;border:1px solid #606060;color:beige;padding:5px;">
</div>
<script id='smain2'>

var debug1 = false;
var debug2 = false;

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
    origin = "http://192.168.178.47";
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
log('Trying to open Webclient socket hihi');

//var timerID=0;
connection.onopen = function () { 
  connection.send('Connect ' + new Date()); 
};
connection.onerror = function (error) { 
  console.log('wsServer Error ', error);
  log('wsServer Error ' + error);
};

connection.onclose = function(){
};

//vom Server empfangen
var server = '';
var json = null;
connection.onmessage = function (e) { 
  if(server == '') {
    server = e.data;
    console.log('Server: ', server);
    log('Server: ' + server);
  } else {
    data = e.data;
    switch(data[0]) {
       case '{':
            if(debug1) {
              log(data);
            }
            try {
              json = JSON.parse(data); 
              updateUi();             
              updateSbmsData();    
            } catch(err) {
              log(err.message);
              console.log(data);
            }
          break;
      default:
            console.log('Nachricht: ', data);
            log(data);      
          break;          
    }
  }
};
console.log('End trying to open Webclient socket');
log('End trying to open webclient socket');

/**
 * Ab 0.8.11 Abloesung der Einzelnachrichten durch JSon
 */
function updateUi() {
  debug1 = json.d1; //debug ist global
  if(null != debug1) {
    document.getElementById("dbg1").checked = debug1;
  }
  var debug2 = json.d2;
  if(null != debug2) {
    document.getElementById("dbg2").checked = debug2;
  }
  var teslaCtlActive = json.ta;
  if(null != teslaCtlActive) {    
    document.getElementById("teslaactive").checked = teslaCtlActive;
  }
  var dt = json.dt;
  if(null != dt) {
    document.getElementById("datetime").innerHTML=dt;
  }
  var s1 = json.s1;
  if(null != s1) {
    b1 = document.getElementById("b1");
    if(s1) {   
      b1.style.background='#f00';
      b1.style.color='#ff0';
      b1.innerHTML='S1on';
    } else {
      b1.style.background='#D8BFD8';
      b1.style.color='#b50';
      b1.innerHTML='S1off';      
    }
  }
  var s2 = json.s2;
  if(null != s2) {
    b2 = document.getElementById("b2");
    if(s2) {   
      b2.style.background='#f00';
      b2.style.color='#ff0';
      b2.innerHTML='S2on';
    } else {
      b2.style.background='#D8BFD8';
      b2.style.color='#b50';
      b2.innerHTML='S2off';
    }
  }
  var batt = json.b;
  if(null != batt) {
    bb = document.getElementById("bb");
    if(batt) {
        bb.style.background='#f00';
        bb.style.color='#ff0';
        bb.innerHTML='Batterie';
      } else {
        bb.style.background='#D8BFD8';
        bb.style.color='#b50';
        bb.innerHTML='Netzvorrang';
      }
  }  
  var lieferung = json.l;
  var bezug = json.z;
  var sum = lieferung - bezug;
  var elem = document.getElementById("lieferung");
  if(sum > 0) {
    elem.style.color='lightgreen';    
  } else {
    elem.style.color='#F1948A';
  }
  elem.innerHTML=''+sum.toFixed(1)+' W';
}

//Keine State-Information hier, die Bestätigung kommt mit Websocket-Datagramm
function toggleBattery(txt) {
    if(txt == 'Netzvorrang') {
      connection.send("@+"); //auf Batterie aendern
    } else {
      connection.send("@-"); //auf Netzvorrang schalten
    }
}
function toggleS1(txt) {
    if(txt == 'S1off') {
      connection.send("@s1+"); //Solarlader S1 an
    } else {
      connection.send("@s1-"); //Solarlader S1 off
    }
}
function toggleS2(txt) {
    if(txt == 'S2off') {
      connection.send("@s2+"); //Solarlader S2 an
    } else {
      connection.send("@s2-"); //Solarlader S2 off
    }
}
function updateServer() {
  var o = {};
  o.ta = document.getElementById("teslaactive").checked;
  o.d1 = debug1 = document.getElementById("dbg1").checked;
  o.d2 = debug2 = document.getElementById("dbg2").checked;
  
  //geht, aber man muss einen String 'bauen'
  //var data = JSON.stringify({ "ta": document.getElementById("teslaactive").checked });
  
  var data = JSON.stringify( o );
  connection.send(data);
}

function htm(id,s){document.getElementById(id).innerHTML =s;};
function pad(n, width, z) {z=z || '0';n=n+'';return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;}
function dcmp(p,s,d){xx=0; for (z=0;z<s;z++){xx = xx + ((d.charCodeAt((p+s-1)-z)-35)*Math.pow(91,z));}return xx;}
function fN(nm){return nm.toString().replace(/(\d)(?=(\d{3})+(?!\d))/g, '$1 , ')}

var lg1="#B33A33B##333333B##B33A33B##B33333B##B''''''##A44A544##B44444B##;75444A##144B444##B33333B##444444B##2331$$A##B8:B:8B#";
var xsbms="01d&u%u$#'G";
var eA="##lh###v1---$v2---empty-%v1&2-#+#y#$1u#y##";
var eW="##T_###$1---#%2---empty-#&1&2-#$#x#$#&#z##";
var sbms1=['','Batt','PV1','PV2','ExtLd','PV1+PV2','Load','ExtLd'];
var sbms2=[0,0,0,0,0,0,0,0,3,7,1,1];

function updateSbmsData(){
   
  var c = document.getElementById('Lg');
  var ctx = c.getContext('2d');
  var r ='</br>'
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
          if(debug1) 
            log("Index " + a + "; cvs["+a+"] " + cvs[a] + "; minInd: " + minInd + "; maxInd: " + maxInd);
        }     
        sbms2[9]=minInd+1;
        sbms2[8]=maxInd+1;   
        for (x1=0;x1<8;x1++) {
          var n=n1='';   
          var cv=cvs[x1];
          if (sbms2[9]==x1+1){min1=cv;n='<mn1>';n1='</mn1>';};
          if (sbms2[8]==x1+1){max1=cv;n='<mx1>';n1='</mx1>';};
          w[0] +=n+'Cell. '+ (x1+1)+n1+r ;
          w[1] +=n+cv.toFixed(3)+n1+r;
          if (sbms2[x1]!=1){w[2] +='<txt>V</txt>'+r;}
          else {w[2] +='<lt><</lt>'+r;};
          bv +=cv;
          var mt = document.querySelector('#mt1');
          var x = document.createElement('meter');
          //Frank: Divisor von 1000 auf 100 geaendert
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
        for (x1=0;x1<8;x1++) {
          col = sbms1[x1+1];
          if(col == 'Load') continue; //Load nicht mehr benoetigt  
          var n2=w[8]=w[9]=w[10]=w[11]='';
          var cv=dcmp((x1*3)+29,3,data)/1000;
          var enW=dcmp(x1*6,6,eW);
          var enA=dcmp(x1*6,6,eA);
          if (x1==0){
            n2=data.charAt(28);
            w[8]='[A]'+r;
            w[9]='[W]'+r;
            w[10]='[MAh][kAh][Ah][mAh]'+r;
            w[11]='[MWh][kWh][Wh]'+r;
          };
          if (x1==1||x1==2){pv3 +=cv;};
          if (x1==3){sv=cv;}
          if (x1==4){cv=pv3;}
          if (x1==5){cv=dcmp(0,3,xsbms)/1000;}
          if (x1==6){cv=sv;}  
          if(x1!=3){
          w[3] +=col+r ;
          w[4] +=w[8]+n2+cv.toFixed(3)+r;
          w[5] +=w[9]+n2+(cv*bv).toFixed(1)+r;
          w[6] +=w[10]+fN(enA)+r;
          w[7] +=w[11]+fN((enW/10).toFixed(1))+r;
          }
            
        }   
        for (i=6;i<9;i++){ //Spalten MAh,kAh,Ah,mAh | MWh, kWh, Wh weg ( i<11 vorher)
          htm('d'+i,w[i-3]);
        }
        htm('d'+12,'Typ: LiIon Kap: 100Ah Status: '+dcmp(56,3,data)+r+'SBMS Temp Int: <val>'+ ((dcmp(24,2,data)/10)-45).toFixed(1)+'</val>&#8451 Ext: <val>'+ ((dcmp(26,2,data)/10)-45).toFixed(1)+'</val>&#8451'+r+'BattVoltage <Val>'+ bv.toFixed(3)+'</Val> V Cell &#916 <Val>'+((max1-min1)*1000).toFixed(0)+'</Val> mV');
      }
  } catch(err) {
    log(err.message);
  }
}

//anfangs sollten die Checkboxen nicht selektiert sein 
document.getElementById("dbg1").checked=false;
document.getElementById("dbg2").checked=false;
</script>

</body>
</html>
)=====";
