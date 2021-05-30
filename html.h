const char changelog[] PROGMEM = R"=====(
<b>Device info</b>: Frank's Solar Charge monitor

<li>0.9.9.97  (1) Nachricht 'Trying to open Webclient socket' wird nun nur noch auf die Konsole gelogged; Kommen non-Jsondaten an, werden sie nur noch ins UI gelogged, wenn der Debugschalter gesetzt ist (Webclient)
<li>0.9.9.97  (2) Das Flag debugConfig wurde im config.print() nicht ausgegeben
<li>0.9.9.97  (3) Die mit 96/4 eingef&uuml;hrte Karenzzeit von 60s triggert nun dauert, obwohl der Controller schon mehr als 60s l&auml;uft. 
<li>0.9.9.97  (4) Es SCHEINT, der H&auml;nger kommt aus invert.checkIncome() im Bereich timeClient.update() ( while-Schleife OHNE Exit ). Ein break beendet den Loop nun nach 10 Fehlversuchen. 
<li>0.9.9.97  (5) Eine Zellunterspannung sollte auf jeden Fall auf Kommandozeile UND GUI gelogged werden (inverter.cpp, wenn debugInverter == true)
<li>0.9.9.97  (6) LOW_VOLTAGE_MILLIS wird von 3100 auf 2800mv heruntergenommen. Damit ist im Netzmodus eine Umschaltung auf Batterie bei 2850mV (durch Hysterese) m&ouml;glich.
<li>0.9.9.97  (7) failureCount und errLimit wurden in inverter.h/cpp integriert, das errLimit von 5 auf 10 angehoben 
<li>0.9.9.97  (8) Der Solarcharger sollte nicht 5, sondern 10Minuten laufen
<li>0.9.9.97  (9) Zur Meldung "Aktiviere Solarcharger 2 wegen Zellunterspannung Zelle 7" sollte die gemessene Spannung angegeben werden 
<li>0.9.9.98  (1) DebugTesla-Variable eingerichtet und in Tesla.cpp verarbeitet
<li>0.9.9.98  (2) Loginabschnitt aus html.h entfernen (Formbased-Login, nicht verwendet), der Redirecting-Timeout im u.a. Updatetemplate wurde von 3 auf 6s erh&ouml;ht
<li>0.9.9.98  (3) Tesla.cpp gibt nun eine Erkl&auml;rung f&uuml;r den gefunden rc<0 aus
<li>0.9.9.98  (4) Das neue Flag (momentan nur per Kommandozeile setzbar) deaktiviert den Batteriemodus (nur laden m&ouml;glich); DEFAULT: false ( wegen Belgienurlaub )
<li>0.9.9.98  (5) Batteriemodus in CFG speichern, beim starten wiederherstellen und in GUI &auml;nderbar machen, in CFG nur 'debugConfig' verwenden
<li>0.9.9.98  (6) Ausgabe myWifi-Verbindungsstatus mit myWifi.connected &uuml;ber print() 
<li>0.9.9.98  (7) Statt WifiUdp in sma.read() wird nun AsyncUDP verwendet ( damit wird fast jede Sekunde automatisch ein UDP-Paket empfangen )
<li>0.9.9.98  (8) Wird der Charger s2 in charger.cpp wg. Zellunterspannungen an oder der Inverter abgeschaltet, dann erfolgt dies nun aufgrund von Fehlercountern in battery.h ab count>3.
<li>0.9.9.98  (9) Kommandozeile: Ein "reset sma" und ein sma.reset() nach myWifi.reconnect() wurden verdrahtet.
<li>0.9.9.99  (1) Einf&uuml;hrung eines Connection-Checks der alle 5Min. l&auml;uft und bei myWifi.connected()==false einen Reconnect einleitet (anders als bei udp in SMA.h hier aus loop0)
<li>0.9.9.99  (2) Kommt in SMA.cpp ein neues UDP-Paket an, wird ein Flag gesetzt, welches steuert, dass charger.checkOnIncome() aufgerufen wird. Nach Verarbeitung setzt die Chargermethode das Flag zur&uuml;ck (Schnellere Justierung der Charger)
<li>0.9.9.99  (3) Das Flag debugInverter in Inverter.cpp sollte beim Start- und Stop des Netzmodus keine Nachrichten unterdr&uuml;cken
<li>0.9.9.99  (4) Konfigoption, alle einkommenden Ertragswerte (netto) direkt an alle verbundenen Clients schicken zu k&ouml;nnen (fastResponse)
<li>0.9.9.99  (5) Die mit 0.9.9.93(2) herausgenommene Ber&uuml;cksichtigung von socLimit wird f&uuml;r den automatischen Abendbetrieb wieder aktiviert. Der beginnt nur, wenn soc>socLimit(70)+SOC_HYST(5) ist, also ab 75% StateOfCharge
<li>0.9.9.99  (6) Logs.h/Logs.cpp aus PegelControl &uuml;bernommen, wird zwischen Netz- und Batteriemodus geschaltet, wird dies hier gelogged.
<li>0.9.9.99  (7) Fix Tesla UserAgent Header 
<li>0.9.9.99  (8) M&ouml;glichkeit einen Logeintrag zu schreiben (Test, Kommandozeile)
<li>1.0.0     (0) Release
<li>1.0.1     (1) Wird die inverter.starteBatterie(..) gerufen, wurde meist keine Lognachricht geschrieben (vorzeitiges return)
<li>1.0.1     (2) Logging beim Einschalten der Batterie um den 'soc'-Wert erweitert
<li>1.0.1     (3) Fehler in der loop0-Methode behoben, hier wurde der myWifi.connected()-Check IMMER durchgef&uuml;hrt, da 'lastConnectCheck' nie gesetzt wurde.
<li>1.0.1     (4) Logs.cpp optimiert und Ausgabe verschlankt
<li>1.0.1     (5) Es braucht das access_token, nicht das refresh_token zur Authentifizierung des Tesla-Accounts 
<li>1.0.1     (6) Der Timeclient wird nur noch in MyWifi.cpp, nicht aber in Inverter.cpp initialisiert/abgefragt
<li>1.0.2     (1) Luefterfunktionen in Luefter.h/.cpp ausgelagert
<li>1.0.2     (2) L&auml;uft ein Charger, dann sollte SOC>=99% ODER Temp<TempMax reichen, nicht UND.
<li>1.0.2     (3) Inverter errLimit und failureCount entfernt ( es reicht, wenn battery.checkCellVoltages() erst ab 3 Fehlmessungen ausl&ouml;st ) 
<li>1.0.2     (4) Die doppelte Messung von Cellspannungen in Inverter.cpp (zur Abschaltung) und in battery.checkCellVoltags (zur Steuerung von Charger2 im Unterspannungsfall wurde aufgehoben
<li>1.0.2     (5) Es gibt nur noch eine untere Zellspannung ( LOW_VOLTAGE_CV_MILLIS ). Sobald diese &uuml;berschritten wird, erlischt der Fehler (durch Abschaltung des Inverters steigen Spannungen automatisch).
<li>1.0.2     (6) 5 globale Variablen wurden nach battery.h verlegt
<li>1.0.3     (1) MyWifi.print() gibt eigenen Status aus    
<li>1.0.3     (2) Debugvariable sbmsAnalyzer und loopAnalyzer entfernt
<li>1.0.3     (3) Wird im Inverter beim Check die Nachtzeit geprueft, so erfolgt dies alle paar Sekunden. Ein falscher SoC-Wert (einmal: 4858) kann die Batterie einschalten. Es werden nun alle Werte > 100 ignoriert. 
<li>1.0.3     (4) SBMS.readSoc beruecksichtigt nun nur noch neu SoC-Werte, wenn sie vom alten weniger als 10% abweichen (Und der alte > 0 war)
<li>1.0.3     (5) Da die Batterie regelm&auml;ssig mit Zellunterspannungsfehlern ausgestiegen ist, wurde nun der Error-Threshold von 3 auf 10 hochgesetzt (Serialdebugmeldungen im Fehlerfalle hinzugef&uuml;gt)
<li>1.0.3     (6) Der Fehlerz&auml;hler wird nur hochgestellt, wenn die gemessene Zellspannung > 0 ist (sonst liegt eh eine Fehlmessung vor) 
<li>1.0.3     (7) Die L&uuml;fter werden nur eingeschaltet, wenn die gemessene Temperatur UNTER 80Grad ist, alles andere wird als Fehlmessung verworfen (Luefter.cpp)
<li>1.0.4     (1) Die Limits der Teslasteuerung lassen sich nun mit einer Eingabe stufenlos setzen.
<li>1.0.4     (2) Tritt beim Wakeup ein Fehler auf, so wird dieser im Client angezeigt
<li>1.0.4     (3) Zur Re-Authentifizierung (alle 45d Pflicht) wurde ein Button eingef&uuml;hrt. ( geht noch nicht )
<li>1.0.4     (4) Es wurde eine Load- und Save-Methode fuer die Konfigklasse vorbereitet
<li>1.0.4     (5) L&auml;uft der Inverter im Nachtmodus (wurde er also autom.bei >=75%SoC gestartet, dann ist die minimale Zellspg. 3050mV, sonst (Notmodus) 2800mV (vorher 2700mV), siehe battery.h/.cpp
<li>1.0.5     (1) Der L&uuml;fter wird st&auml;ndig an- und ausgeschaltet, wenn die Temperatur 38°C ist und SOC>=99 angezeigt wird. Der SOC sollte hier nicht ber&uuml;cksichtigt werden.
<li>1.0.6     (1) Die Tesla-API verwendet nun die API des HTTPClients korrekt, indem im .begin(..) der WiFiClient als erstes Argument &uuml;bergibt.  (revert, weil crash; evtl. muss der WifiClient VOR dem HttpClient initialisiert werden)
<li>1.0.6     (2) Bei einem readChargeState des Tesla erfolgt meist ein Fehler, da der Wagen schl&auml;ft. In diesem Fall sollte der Webclient informiert werden. 
<li>1.0.7     (1) Ein neuer Schwellwert wait_excess_power_start_millis regelt nun, dass der statische Charter S1 erst aktiviert wird, wenn f&uuml;r mehr als 1 Minute positive Zahlen f&uuml;r ihn vorliegen
<li>1.0.7     (2) Bisher wurde ab 10Uhr morgens aufs Netz geschaltet, dies wird nun mit der Bedingung verkn&uuml;pft, dass der SOC kleiner als 50% ist.
<li>1.0.8     (1) L&uuml;ftersteuerung aus CTRL.h/cpp nach Luefter.h/cpp verlegt
<li>1.0.8     (2) Versuch, den rc=5 connection error in der Testlasteuerung zu beheben, beginRequest inline gelegt.
<li>1.0.8     (3) Charger.cpp hat nun eigene Ausgabe mit print()
<li>1.0.9     (1) Chargertuning ...
<li>1.0.9     (2) Liefen Charger 1+2 mehr als eine Stunde, wurde bisher der Batteriebetrieb abgeschaltet, das wird nun zur&uuml;ckgenommen (Batterie l&auml;uft durch, unterst&uuml;tzt durch die Charger)
<li>1.0.10    (1) Tesla-API zur&uuml;ckgestellt, wegen rc=5, connection lost
<li>1.0.11    (1) Stringdaten vom Inverter und Temperatur Becken empfangen
<li>1.0.11    (2) Fix: retrieveData wurde VIEL zu h&auml;fig aufgerufen, weil connectionCheck-Timestamp nicht zur&uuml;gesetzt wurde 
<li>1.0.12    (1) Ein 'Automatik'-Modules (automatedCharging; default: true) lösen s1override und s2override ab.
<li>1.0.12    (2) Die globalen Variablen s1_countBeforeOff, enableCountBeforeOff (war sei 0.9.9.64 off) entfallen; s1MinRestMillis, s2MinRestMillis, dutyCycle wurden in Charger verlegt.
<li>1.0.12    (3) Die Variable s2_millisBeforeOff wird umbenannt in s2_lowNettoMillis, eine s1_lowNettoMillis (mind. 30s) wurde hinzugef&uuml;gt
<li>1.0.13    (1) Bei config load von der Kommandozeile wird nun debugConfig gesetzt 
<li>1.0.14    (1) Im WebCom wurde eine neue Methode hinzugef&uuml;gt, um einen Log-Ringpuffer bef&uuml;llen zu k&oouml;nnen, der bei Reconnect an den Client geschickt wird. Jeder neue Client bekommt ein Update der letzten Messages
<li>1.0.15    (1) Im Inverter.cpp wird tags&uuml;ber auf Netzbetrieb gestellt, wenn der battery.soc unter 50% f&auml;llt. Wird in SBMS.cpp aber der battery.soc falsch gelesen, wird auf 0 gestellt. Das wird nun nicht mehr gemacht.
<li>1.0.16    (1) Libupates und IDF 3.3 ( 1.0.5-rc3)
<li>1.0.16    (2) Tesla Re-Authentifizierung ohne Bearer Token (das ist ja abgelaufen) und mit anderer Zugriffsmechanik (Tesla.cpp)
<li>1.0.16    (3) OTA.cpp, max. Groesse erhoeht: 1248576 auf 1348576 
<li>1.0.17    (1) Loglines von 10 auf 20 erhoeht, battery.print gibt nun auch die Fehlercounter für Inverter und Charger aus (moegliche Analyse, warum Batterieumschaltung nach einiger Zeit nicht mehr automatisch erfolgt)
<li>1.0.17    (2) Revert max-Size OTA.cpp (Bad Size Given bei jedem Updateversuch)
<li>1.0.18    (1) Meter-Rendering Chrome gefixed
<li>3.0.0     (1) RELAY_PIN(21) wird von GPIO21 auf GPIO0 gelegt
<li>3.0.0     (2) LiquidCrystal_I2C Unterstützung
<li>3.0.1     (1) Inverter bekommt print-Funktion, um 'nacht'-Flag ausgeben zu koennen
<li>3.0.1     (1) Inverter: Das 'nacht'-Flag muss am Tage IMMER auf false gesetzt werden (es wurde sonst nur umgesetzt wenn der SOC<50%). Wurde dann der Inverter Manuell gestopped, startete er abends nicht mehr automatisch
<li>3.0.1     (2) Funktion voltageSensor.calibrate() in CLI-Hilfe integriert
<li>3.0.1     (3) Logs vereinfacht, nur noch 3 Zeilen, kein CIFS mehr.
<li>3.0.1     (4) Commmandline-Input wird nur bis zum ersten Lineinput gelesen. 
<li>3.0.1     (5) Wenn die Netzspannung fehlt, wird nun auch immer der Lüfter aktiviert (voltageSensor.hasNetzspannung())
<li>3.0.2     (1) Nutzung von LittleFS anstatt SPIFFS
<li>3.0.3     (1) Eine Fehlermeldung, wenn die SBMS-Daten nicht zu dekodieren waren, konnte wegen der enthaltenen Sonderzeichen nicht zum Client geschickt werden. Sie wird nun zusätzlich auf Serial ausgegeben.
<li>3.0.3     (2) Wird keine SBMS-Nachricht gelesen (zu kurz), dann wird die Status-LED wieder auf Blau gestellt, anderfalls auf Grün
<li>3.0.4     (1) Versuch zu verhindern, dass RELAY_PIN (0) nach jedem Restart (Spannung weg) sofort schaltet und den Bootvorgang stört
<li>3.0.5     (1) Die Variable 'wait_excess_power_start_millis' wurde von 0 auf 5000ms gesetzt (Zeit bevor S1 geschaltet wird)
<li>3.0.5     (2) Die Reaktionszeit des Chargers 2 wird von 30s auf 5s heruntergesetzt ( s2MinRestMillis )
<li>3.0.5     (3) Die Reaktionszeit des Chargers 1 wird von 60s auf 20s heruntergesetzt ( s1MinRestMillis )
<li>3.0.5     (4) Charger.cpp (calculateDc) wurde jeder Wert um 10 erhöht (um den Solarstrom besser auszunutzen)
)=====";

#define VERSION "3.0.5"

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
    </select>
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
    vorlauf=json.vl;
  }  
  if(json.hasOwnProperty("rl")) {
    ruecklauf=json.rl;
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
