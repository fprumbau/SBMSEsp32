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
<li>0.9.9.72: Vor der Bewertung (charger.checkOnIncome) f&uuml;r S1 und S2 sollte geprueft werden, ob die Batterie genutzt wird (Upload geht NICHT, SBMS einlesen auch nicht; Was hat sich hier ge&auml;ndert?; => hier war sbms.readSbms();  auskommentiert!)
<li>0.9.9.73: <Wrover-B> Switch auf Wrover-B, wo SBMS-In(RX2) auf GPIO16 nicht belegt und auf Platinenr&uuml;ckseite auf GPIO15 umgelegt werden muss. Wird da eine Br&uuml;cke zwischen GPIO15 und GPIO16 gelegt, m&uuml;ssten beide Chips arbeiten. (Upload kein Binary)
<li>0.9.9.74: <Wroom> isAlive Url geschaltet: /lbprobe liefert 'online', Rollback 0.9.9.72 (charger.cpp), es darf auch im Batterieberieb geladen werden (Upload kein Binary)
<li>0.9.9.75: Loop0-Taskmethode integriert, um bisher ungenutzten Core zu nutzen (Upload kein Binary)
<li>0.9.9.76: Beginn der Steuerung der Teslaladeregelung, Tesla.h und Tesla.cpp hinzugefuegt, Status, Wakeup, Ladestart und Ladestop, alles implementiert, aber noch nicht angesteuert (Upload geht, SBMS einlesen aber nicht mehr => hier war sbms.readSbms();  auskommentiert!)
<li>0.9.9.77: Verschiedene Optimierungen wg. Stringnutzung, Changelog wurde in html.h verlegt; Anhebung des Uploadmaximums fuer OTA-Sketches von 1048576 auf 1248576. Das zul&auml;ssige Maximum sollte noch bestimmt werden.
<li>0.9.9.78: Der mit 0.9.9.58 eingef&uuml;hrte ESP.restart() um 06:00 morgens wird wieder herausgenommen.
<li>0.9.9.79: In charger.cpp wurde nun die Tesla-Ladesteuerung aktiviert, bei >500 und allen Ladern an wird nun versucht, den Ladevorgang zu starten, bei <-1000W wird gestoppt. Minimale Schaltzeit ist 5Min.
<li>0.9.9.80: Beim startCharge wird nun zwischen Limitsetzung und Chartstart 8s statt 5s gewartet. Eine Schaltung bedingt nun ausserdem mindestens 10 positive Messungen.
<li>0.9.9.81: Tesla Chargestate zeichnet nun die wichtigsten Gr&ouml;ssen auf.
<li>0.9.9.82: Anfang eines GUI-Controls (Web) um die Teslasteuerung ein-/bzw. auszuschalten
<li>0.9.9.83: (1) Das Flag teslaCtrlActive wird nun permanent im SPIFFS abgelegt
<li>0.9.9.83: (2) Die Konfiguration wird nun lokal im CFG vorgehalten, Konfig nun &uuml;ber Kommandozeile speicherbar
<li>0.9.9.83: (3) Commandline auf Task0
<li>0.9.9.83: (4) Add Try-Catch in Webseite bei SBMS-Verarbeitung
<li>0.9.9.83: (5) Nach setzen des Testdatenschalters wird UI aktualisiert
<li>0.9.9.83: (6) Debug / Debug2 werden nun auch per JSon aus der Webansicht aktualisiert (aber nicht im SPIFFS abgespeichert)
<li>0.9.9.83: (7) S1,S2 und Netzvorrang werden nun ueber Json aktualisiert, alter Benachrichtigungscode bereinigt, S1 und S2 sind nun als symbolische Links integriert
<li>0.9.9.83: (8) Der mit 0.9.9.78 ausgeplante Restart ging einher mit dem Abschalten des Batterievorrangs ab 06 Uhr (wechselrichter.cpp); &Auml;nderung: 6Uhr->9Uhr
<li>0.9.9.83: (9) Das RELAY_S1 wird nun im Setup geschaltet. Nur das RELAY_3 bestimmt noch (ueber Remote) den Zustand des Chargers S1 (HLG600A)
<li>0.9.9.84: (1) Der Aufruf der Upgradeseite erfolgt - beim Aufruf von der Homepage - nun mit dem internen DNS-Namen http://sbms/update
<li>0.9.9.84: (2) Hintergrundfarben der deaktivierten Buttons S1,S2 etc angepasst.
<li>0.9.9.84: (3) Auskunftsfenstern div:id='teslaout' hinzugef&uuml;gt.
<li>0.9.9.84: (4) Die L&uuml;fter sollten laufen, solange die Grenztemperatur nicht um 2 Grand unterschritten wurde (TEMP_THRESHOLD_HIGH/_LOW) (verhindert Relayklickmarathon) 
<li>0.9.9.84: (5) Fehlermeldung fixed: 'Cannot process data: Connect Wed Jul 24 2019 10:17:46 GMT+0200 (Mitteleurop&auml;ische Sommerzeit)' (aus onopen-Event in Webseite; jetzt nur noch lokal).
<li>0.9.9.84: (6) Moeglichkeit geschaffen, &uuml;ber die Kommandozeile Konfigwerte zu setzen (um sie dann mit einem config.save) sp&auml;ter speichern zu k&ouml;nnen. 
<li>0.9.9.84: (7) Wird ein Tesla-Statusupdate empfangen, dann wurde die maximale Nachrichtengroesse von 300Byte ueberschritten (392), Max wurde nun auf 512 erh&uuml;ht
<li>0.9.9.84: (8) Fehler bei Namen WakeUp-Button behoben, Styling, 'teslaout' wird nun bef&uuml;llt.
<li>0.9.9.85: (1) Free heap Anzeige, nur bei Debug2 (online), Hinzu kommmt heap fragmentation und max available block size (command line und online)
<li>0.9.9.85: (2) Implementierung eines Mechanismus' der verhindert, dass der Server immer dieselben Teslastatusdaten sendet
<li>0.9.9.85: (3) Einige yield()s hinzugefuegt, viele String-Optimierungen
<li>0.9.9.85: (4) Bisher wurde beim Laden die L&uuml;fter bei SOC<99 angeschaltet, nun wird dies nur noch bei hoher Temperatur gemacht.
<li>0.9.9.85: (5) Statt Dbg1+Dbg2 als Einzelflags zu regeln, gibt es nun eine Bitleiste auf Server und Client.
<li>0.9.9.85: (6) Steueroption Tesla start/stop charge hinzugef&uuml;gt 
<li>0.9.9.85: (7) Der UDP-Reinitialisierungthreshold wurde von 60s auf 120s angehoben, alle 60s wird nun ein Ping-Paket versendet.
<li>0.9.9.85: (8) Eine Authentifizierungsmethode ist zum Teslaobjekt hinzugekommen um das Bearer-Token erneuern zu k&ouml;nnen.
<li>0.9.9.85: (9) Tesla charge start/stop versuchen nun nicht mehr, das Chargelimit zu setzen. Daf&uuml;r gibt es eine eigene Methode
<li>0.9.9.86: (1) Nach einer Teslastatusabfrage wird 'cs' (charge state) &uuml;bermittelt
<li>0.9.9.86: (2) Weitere yields im Berech des Datenempfangs und der Verarbeitung in WebCom.cpp
<li>0.9.9.86: (3) WebCom.sendJson(key,value) zugef&uuml;gt, auch eine neue Methode WebCom.sendClients(String* msg) gebaut, die das Kopieren des Strings verhindert
<li>0.9.9.86: (4) UpdateUi in html.h sollte nun auch mit Teilantworten zurecht kommen. Das 'cs' (chargeState) kommt nun allein, wenn start/stop charge mit rc==200 erfolgreich
<li>0.9.9.86: (5) Wakeup repariert, der RC wird nun &uuml;bermittelt und gelogged
<li>0.9.9.86: (6) Ein drittes Debugflag 'jsonDebug' erlaubt die Analyse der Server-/Clientkommunikation
<li>0.9.9.87: (1) Es wurden zuviele Debugmessages ohne Debugschalter geschickt (Bitset vom/zum Server)
<li>0.9.9.87: (2) Wird bei aktivem Laden auf dem Ladenbutten geklickt, dann wird das Laden gestoppt. Hier muss bei rc==200 ein Resetz des Schalters erfolgen
<li>0.9.9.87: (3) Schriftgroesse des Konsolenoutputs (Ausgabe GUI) von 0.7em auf 0.6em herabgesetzt, deakt.Buttons hellbraun statt grau
<li>0.9.9.87: (4) Der Statusbutton ist nun rechts neben der Checkbox, zwei neue Buttons (Lim50 und Lim90) wurden vorbereitet, die Statusausgabe 'teslaout' ist 10px schm&auml;ler
<li>0.9.9.87: (5) Bei webcom.updateUi() wird nun immer der letzte Chargingstatus ausgeliefert, nicht nur wenn der Gesamtstatus abgerufen wurde.
<li>0.9.9.87: (6) Aufnahme einer neuen print()-Methode in der Hauptklasse SBMSEsp32, die erst einmal Temperatur und Ladezustand der Batterie ausgibt.
<li>0.9.9.87: (7) Ladestandsanzeiger f&uuml;r den Teslabereich
<li>0.9.9.87: (8) Ein Update des Ladestandes wird nur noch gemacht, wenn rc==200 in perry.readChargeState()
<li>0.9.9.87: (9) Die Buttons Limit50 und Limit90 setzen nun ein Ladelimit von 50 oder 90%. Endet das Kommando mit Rc==200, wird nach 5s noch eine Statusabfrage gemacht (wenn queryTeslaStateAfter = true)
<li>0.9.9.88: (1) Die Wartezeit bis zur Statusabfrage nach Setzen eines Limits wird von 5s auf 10s erhoeht
<li>0.9.9.88: (2) Neues Debugflag 'debugRelais' einbauen, um alle Schaltaktionen von Relais zu dokumentieren; das Ergebnis sollte immer mit log(...) in der Konsole erscheinen (work in progress)
<li>0.9.9.88: (3) Es sollte immer zuerst S1 ausgeschaltet werden, S2 erst dann, wenn 5 Minuten die Lieferung negativ war (S2 regelt selbst herunter bis ca. 1A).
<li>0.9.9.88: (4) Ein wenig umr&auml;umen/optimieren in SMA.cpp/SCM.h 
<li>0.9.9.88: (5) BasisUrl zur Re-Authentifizierung gefixed, wurde ein neues Token geholt, wird es im SPIFFS gesichert.
<li>0.9.9.88: (6) Watchdogtimer abstellen (999s, no panic handler).
<li>0.9.9.88: (7) Fix: Bei der Anforderung, das Teslaladen zu starten, wurde immer ein false übermittelt.
<li>0.9.9.88: (8) Die Config konnte nicht gespeichert werden, wenn bei ihrem Laden zum Programmstart ein Fehler passiert war (_configRead=false), dies wurde entfernt.
<li>0.9.9.88: (9) Authentication f&uuml;r Aufrufe &ber /sbms-Kontext von Remote
<li>0.9.9.88: (10) Formatierung des Tesladivs ge&auml;ndert
<li>0.9.9.89: (1) Statt StaticJsonDocument (Inhalt gemischt auf stack) wird in CFG nun DynamicJsonDocument verwendet und dies dann nur noch Methodenlokal.
<li>0.9.9.89: (2) Fix bei der Verwendung des gesch&uuml;tzten Webzugangs, hier war User/Pass f&uuml;r den /sbms-Kontext noch hart kodiert.
<li>0.9.9.89: (3) UDP Ping entfernt (kein Stabilit&auml;tsgewinn)
<li>0.9.9.89: (4) Statt mit bezug+lieferung zu arbeiten und diese auch ins GUI zu liefern, wird nun nur noch json.n = netto verarbeitet.
<li>0.9.9.89: (5) Die Methoden sbms.read() (vorher readSbms) und sma.read() geben nun true im Erfolgsfall zur&uuml;ck, nur dann wird inverter.check() (sbms) bzw. charger.checkOnIncome() ausgef&uuml;hrt.
<li>0.9.9.89: (6) U&uml;ber die Kommandozeile kann der maximale Loglevel angeschaltet werden.
<li>0.9.9.89: (7) Alle UDP-Pakete m&uuml;ssen 600Byte lang sein, das ist nun hart kodiert. Damit wird auch verhindert, dass ein evtl. laengeres Packet den auf 601Byte vorbereiteten Buffer &uuml;berschreiben kann (Crashpotential)
<li>0.9.9.89: (8) Es wird jede Sekunde ein UDP-Paket gelesen, aber nur alle 3s eines ausgewertet, dies wird nun angeglichen (udp auch 3s)
<li>0.9.9.89: (9) Neue Debugflags aufgenommen: debugSma,debugSbms,debugCharger,debugBattery,debugInverter
<li>0.9.9.89: (10) Falsch dokumentierter Timeout in global.cpp wurde in sbms.read() verwendet, um nur alle 10s eine Verarbeitung eines SBMS-Pakets zu triggern: entfernt
<li>0.9.9.90: (1) Im normalen Kontext ('/sbms') konnte der Tesla nicht zugreifen, fixed
<li>0.9.9.90: (2) DebugKonfig aufgenommen (Flag 9), Upgrade 100AH auf 260AH.
<li>0.9.9.90: (3) Nach einem Speichern des SPIFFS config files sollte auch ein flush() und ein close() angeschlossen werden.
<li>0.9.9.90: (4) Wird anfangs kein Netto-Ertragswert per Json geliefert, wird der toFixed(1)-Fehler auf undefined jetzt abgefangen.
<li>0.9.9.90: (5) Laden von User/Pass f&uuml;r den Webzugang repariert
<li>0.9.9.90: (6) Das Setzen von Bitsetvariablen 9 und 10 ging nicht, da Bitset nur 8 Stellen lang initialisiert war (global.cpp) 
<li>0.9.9.90: (7) In der Webseite wurde +r mit +'</br>' ersetzt und optimiert
<li>0.9.9.90: (8) Blockingproblem von sbms.read mit readString() gel&ouml;st
<li>0.9.9.90: (9) Die Anzeige der Werte f&uuml;r Batt, PV1, PV2 etc. wurde um die Spalte der Wh erg&aauml;nzt (zur&uuml;ckgenommen), die gesamte Berechtung im Html vereinfacht.
<li>0.9.9.91: (1) Die neue Read-Funktion verursacht auch fehlerhafte Verarbeitungen mit Zellspannung von 0V, es werden nur noch Paket mit >50 Zeichen angenommen.
<li>0.9.9.91: (2) Weil in Sbms.read() immer wieder falsche Werte (Temperatur 283°C) gelesen wurden, wird erst einmal wieder auf Serial1.readString() zur&uuml;ckgestellt
<li>0.9.9.91: (3) Aus SOC_LIMIT wird socLimit, der sich aus der CFG ueberschreiben l&aauml;sst; die Hysterese wird nun von 5 auf 2% heruntergenommen (testen!!!)
<li>0.9.9.91: (4) Formatierung Steuerungsbuttons Tesla in Webseite (rounded, height, position)
<li>0.9.9.91: (5) Fix: socLimit wurde nie in Konfig geschrieben
<li>0.9.9.91: (6) Debug2 ist jetzt 'debugWeb'
<li>0.9.9.91: (7) Bisher wurde S2 erst bei -400W &uuml;ber mehr als 5 Minuten abgeschaltet, nun wurde dies auf 100W heruntergesetzt
<h2>TODO</h2>
<li>  Fixme: Serial1.readString() in SBMS.cpp read() ersetzen.
<li>  TODO: Kommt bei einer Tesla-Statusabfrage (oder einer anderen Aktion) ein 408, sollte die Meldung 'Sleep mode aktive' kommen und die aktive Anfage (Button) gelöscht werden
<li>  https://owner-api.teslamotors.com/api/1/vehicles/YOUR_VEHICLE_ID_HERE/data_request/vehicle_state  /  https://medium.com/@jhuang5132/a-beginners-guide-to-the-unofficial-tesla-api-a5b3edfe1467 
)=====";

#define VERSION "0.9.9.91"

const char login[] PROGMEM = R"=====(
<!DOCTYPE html><html>
<style>
body { background-color:#545147;color:beige; }
input {
        background-color:beige;
        color:#505050;
        -webkit-border-radius: 5px;
        -moz-border-radius: 5px;
        border-radius: 5px;
        padding:5px;
}
.button {
        background-color:#505050;
        color:beige;
        border-color:1 solid beige;
        -webkit-border-radius: 5px;
        -moz-border-radius: 5px;
        border-radius: 5px;
        padding:3px;
}
#parent {
   margin-top:200px;
   display: table;
   width: 100%;
}
#form_login {
   display: table-cell;
   text-align: center;
   vertical-align: middle;
}
</style>
<body>
<script>
function submitAuthForm() {
    var login = document.getElementById('login').value;
    var pass = document.getElementById('pass').value;
    location = location.href.replace('://', '://' + encodeURIComponent(login) + ':' + encodeURIComponent(pass) + '@');
    console.log(location);
    // might be required to reload on Firefox (FF shows confirmation dialog)
    setTimeout(function(){
        location.reload(true);
    }, 5000);
}
</script>
<div id="parent">
    <form id="form_login" method="post" onsubmit="submitAuthForm(); return false;">
        SBMS Login
        <p>
            <input type="text" id="login" placeholder="username" />
        </p>
        <p>
            <input type="password" id="pass" placeholder="password" />
        </p>
        <p>
            <input class="button" type="submit" value="&nbsp;&nbsp;Login&nbsp;&nbsp;"/>
        </p>
    </form>
</div>
</body>
</html>
)=====";

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
<style media='screen' type='text/css'>
meter {-moz-appearance:none;appearance:none;width:180px;height:12px;position:absolute;left:10px;box-shadow:1px 2px 3px #ec6;border-radius:3px;}
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
<div2 style='margin-top:-20px' id="datetime">2018-12-27</div2>
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
    </select>
</input>
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
<div2 style='left:5px;color:#ea8;top:22px;'id='d6'>[Source]</div2>
<div2 style='right:580px;text-align: right;'id='d7'>[A]</div2>
<div2 style='right:490px;text-align: right;'id='d8'>[W]</div2>
<div5 style='width: 360px;top:0px;height:22px;text-align: right;'id='d11'></div5>
<div2 style="border:1px solid #918263;left:360px;width:355px;height:160px;border-radius:5px;">
<meter style='height:10px; left: 202px; top:15px; width: 6px;' min='2' max='8' value='0'></meter>
<meter id='bat2' style='height: 30px; width: 190px; top: 2px;margin-top:2px;' min='0' low='20' max='100'></meter>
<div2 style='color:#030;font-size:24px;top:10px;left:87px;text-shadow: -2px -2px 2px #efc;' id='SOC2' >0%</div2>
<input type="checkbox" id="teslaactive" onchange='updateServer();' style="margin-top:45px;"></input>Tesla Ladestrg.

<input type="button" class="bs" id="state" value="Status" onclick="setOn(this);updateServer(this.id);" style="margin-top:5px;margin-bottom:8px;margin-left:3px;width:215px;"><br>
<input type="button" class="bs" id="wakeup" value="Wake" onclick="setOn(this);updateServer(this.id);" style="margin-top:1px;margin-left:3px;">
<input type="button" class="bs" id="charge" value="Laden" onclick="updateServer(this.id);">
<input type="button" class="bs" id="lim50" value="Lim50" onclick="setOn(this);updateServer(this.id);">
<input type="button" class="bs" id="lim90" value="Lim90" onclick="setOn(this);updateServer(this.id);">
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
var bitset = "0000000000";
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
log('Trying to open Webclient socket');
//var timerID=0;
connection.onopen = function () { 
  log('Serververbindung aufgebaut: ' + new Date()); 
};
connection.onerror = function (error) { 
  console.log('wsServer Error ', error);
  log('wsServer Error ' + error);
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
            if(debugJson) {
              log(data);
            }
            json = JSON.parse(data); 
            updateUi();      
            if(json.hasOwnProperty('d')) {       
              updateSbmsData();    
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
var rts_reset = false;
/**
 * Akualisierung nach Empfang von Serverdaten
 * Ab 0.8.11 Abloesung der Einzelnachrichten durch JSon
 */
function updateUi() {
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
        case "lim50":
          o.lm = 50; //charge limit 50%
          queryTeslaStateAfter = true;
        break;    
        case "lim90":
          o.lm = 90; //charge limit 90%
          queryTeslaStateAfter = true;
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

        //Batt
        var n2=w[8]=w[9]=w[10]=w[11]='';
        var cv=dcmp(29,3,data)/1000;
        n2=data.charAt(28);
        w[8]='[A]</br>';
        w[9]='[W]</br>';
        w[10]='[Ah]</br>';
        w[11]='[Wh]</br>';
        w[3] +='Batt</br>' ;
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';       
        
        //PV1
        var n2=w[8]=w[9]=w[10]=w[11]='';
        var cv=dcmp(32,3,data)/1000;
        pv3 +=cv;
        w[3] +='PV1</br>' ;
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';
  
        
        //PV2
        var n2=w[8]=w[9]=w[10]=w[11]='';
        var cv=dcmp(35,3,data)/1000;     
        pv3 +=cv;
        w[3] +='PV2</br>' ;
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';

        //Mmmmh....
        var cv=dcmp(38,3,data)/1000;
        sv=cv;

        //PV1+PV2
        var n2=w[8]=w[9]=w[10]=w[11]='';
        var cv=dcmp(41,3,data)/1000;
        cv=pv3;
        w[3] +='PV1+2</br>' ;
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';

        //Ext.Load
        var n2=w[8]=w[9]=w[10]=w[11]='';
        var cv=dcmp(47,3,data)/1000;
        cv=sv;
        w[3] +='ExtLd</br>';
        w[4] +=w[8]+n2+cv.toFixed(3)+'</br>';
        w[5] +=w[9]+n2+(cv*bv).toFixed(1)+'</br>';

        //div2#d6 Spalte mit 'Batt', 'PV1' etc....
        htm('d6',w[3]);

        //div2#d7 Spalte mit Stromwerten
        htm('d7',w[4]);

        //div2#d8 Spalte mit Wattwerten
        htm('d8',w[5]);

        htm('d'+12,'Typ: LiIon Kap: 260Ah Status: '+dcmp(56,3,data)+'</br>'+'SBMS Temp Int: <val>'+ ((dcmp(24,2,data)/10)-45).toFixed(1)+'</val>&#8451 Ext: <val>'+ ((dcmp(26,2,data)/10)-45).toFixed(1)+'</val>&#8451</br>'+'BattVoltage <Val>'+ bv.toFixed(3)+'</Val>V Cell &#916 <Val>'+((max1-min1)*1000).toFixed(0)+'</Val>mV');
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
