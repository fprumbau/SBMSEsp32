#include <AceButton.h>

#include "html.h"
#include "global.h"
#include "esp_task_wdt.h"

using namespace ace_button;

ButtonConfig tasterConfig;
AceButton taster(&tasterConfig);

//Erster Versuch mit Multi-Core Multitasking
TaskHandle_t Task0;
//SemaphoreHandle_t semaphore;

/**
 * Buttonsteuerung, um manuell den Inverter schalten zu koennen
*/
void handleButton(AceButton*, uint8_t eventType, uint8_t);

/**
 * Kommandos von Serial lesen, wie z.B.
 * Neustart von WiFi
 */
void commandLine();

/**
 * Registriere Eventhandler für WebSocketEvents in WebCom
 */
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  wc.onWsEvent(server, client, type, arg, data, len);
}

/**********************************************************************/
/*                                                                    */
/*                Setup                                               */
/*                                                                    */
/**********************************************************************/
void setup() {

  esp_task_wdt_init(999,false); //0.9.9.88

  Serial.begin(115200);  //USB Serial Pins 4,2, 
  Serial.setDebugOutput(true); //0.9.9.98

  //3.0.7 LOG_LEVEL_SILENT, LOG_LEVEL_FATAL, LOG_LEVEL_ERROR, LOG_LEVEL_WARNING, LOG_LEVEL_INFO, LOG_LEVEL_TRACE, LOG_LEVEL_VERBOSE
  Log.begin(LOG_LEVEL_WARNING, &Serial);
  
  //WROOM hat 16/17 auf RX2/TX2 verbunden
  serialSBMS.begin(9600, SERIAL_8N1, 16, 17); //Serial2 Pins 16,17
  serialSBMS.setTimeout(100); //Default 1000ms; 0.9.9.94; ( 9600 baud ist ca. 800 Byte/s, da nur ca 100 Byte benoetigt werden, kann es schneller gehen)

  //WROVER-B hier sind die Pins 16+17 disabled, 
  //serialSBMS.begin(9600, SERIAL_8N1, 15, 17); //TX2 wird nicht gebraucht (Kommunikation -> SBMS), RX2 auf GPIO15, hierfür muss dann in der Schaltung GPIO16 auf GPIO15 gebrückt werden!
  
  Serial2.begin(115200); //wegen Restart

  Log.warningln(F("Starting..."));

  //Pins fuer Taster und Relay initialisieren
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); //v.3.0.4 LOW sollte der Initialstatus sein. Mit nach 1.0.18 wurde wird hier PIN 0 genutzt. Der switched aber bei jedem kompletten An/Aus (Spannung weg)

  //FIXME: RELAY_S1 sollte nicht mehr den Charger S1 schalten; RELAY_3 könnte dann in RELAY_S1 umbenannt werden
  pinMode(RELAY_S1, OUTPUT); //Charger S2, hart (wird in der Loging aber RELAY_3 geschaltet); 
  pinMode(RELAY_S2, OUTPUT);

  //v.0.9.9.40
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);

  //manuell Invertersteuerung
  pinMode(TASTER, INPUT_PULLUP);
  tasterConfig.setEventHandler(handleButton);
  taster.init(TASTER, HIGH, 0 /* id */);

  //Gibt Strom auf BEIDE Charger
  digitalWrite(RELAY_S1, LOW); //schaltet nur Power an beider Charger an; (S1 wird zusätzlich über RELAY_3 gestartet)  

  digitalWrite(RELAY_3, HIGH); //Schaltet S1, HLG600b,regelbar, in den Initialstatus AUS (Remoteeingang des Laders S1)
  digitalWrite(RELAY_S2, HIGH); //Schaltet S2, HLG600A, fest, in den Intialstatus AUS (Remoteeingang des Laders S2)

  digitalWrite(RELAY_4, HIGH); //Schaltet die Luefter an/aus (24V)

  //v.0.9.9.51 ff
  pinMode(PWM_L1, OUTPUT); //Luefter Charger GPIO25 (noch nicht in Gebrauch)
  pinMode(PWM_L2, OUTPUT); //Luefter Inverter GPIO26 (noch nicht in Gebrauch)
  pinMode(PWM_S2, OUTPUT); //HLG600-30B, GPIO05 (steuert den Ladestrom von S2 über PWM)

  int freq = 100;
  int resolution = 10; //0...1024
  
  ledcSetup(GPIO25, freq, resolution); 
  ledcSetup(GPIO26, freq, resolution); 
  ledcSetup(GPIO05, freq, resolution); //HLG600B

  ledcAttachPin(PWM_L1, 0);
  ledcAttachPin(PWM_L2, 1);
  ledcAttachPin(PWM_S2, 2);

  //Leds
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_S1, OUTPUT);
  pinMode(LED_S2, OUTPUT);

  //Startup-LED-Test
  digitalWrite(LED_S1, HIGH);
  digitalWrite(LED_S2, HIGH);
  inverter.setRed();
  delay(600);
  inverter.setGreen();
  delay(600);
  inverter.setBlue(); //Beim Laden BLAU zeigen
  delay(600);
  digitalWrite(LED_S1, LOW);
  digitalWrite(LED_S2, LOW);
  //LED-Test ENDE

  // etabliere Wifi Verbindung
  myWifi.connect();

  // start Webserver
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", html);
  });

  //Test headers
  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
    //List headers
    for(int i = 0; i<request->headers(); i++) {
        Log.warningln(F("%s:%s"), request->headerName(i), request->header(i));
    }
    String header = request->header("User-Agent");
    if(header.indexOf("Tesla")>1) {
      request->send(200, "text/html", html);
    } else {
      if(!request->authenticate(config.webUser(), config.webPass())) {
        //FIXME: Geht leider nicht mit FormBasedAuth, siehe html.h: request->send(200, "text/html", login);
        request->requestAuthentication();
      } else {
        request->send(200, "text/html", html);
      }
    }
  });
  
  // Zugriff von aussen
  server.on("/sbms", HTTP_GET, [](AsyncWebServerRequest *request){
    if(debug) {
        for(int i = 0; i<request->headers(); i++) {
            Log.notice(F("%s:%s"CR), request->headerName(i), request->header(i));
        }
    }
    String header = request->header("User-Agent");
    if(header.indexOf("Tesla")>1) {
      request->send(200, "text/html", html);
    } else {
      if(!request->authenticate(config.webUser(), config.webPass())) {
          //FIXME: Geht leider nicht mit FormBasedAuth, siehe html.h: request->send(200, "text/html", login);
          request->requestAuthentication();
      } else {
        request->send(200, "text/html", html);
      }
    }
  });

  server.on("/lbprobe", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "online");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/favicon.ico", "image/x-icon");
  });

  //Registriere Eventhandler WebsocketEvents
  ws.onEvent(onWsEvent);
    
  server.addHandler(&ws);
  server.begin();

  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  timeClient.setTimeOffset(3600);
  
  // initialize other the air updates
  updater.init(hostName);

  //neuer Taskcode Multicore
  xTaskCreatePinnedToCore(loop0, "Task0", 5000, NULL, 0, &Task0, 0);

  // v.0.9.9.65 favicon via LITTLEFS
  if(!LITTLEFS.begin()){
    Log.error(F(">> An Error has occurred while mounting LITTLEFS"));
    return;
  }

  //0.9.9.76 Load config LITTLEFS
  //config.save(); //NUR mit vorher eingestellten Werten einkommentieren!!!, siehe CFG.save(..)
  config.load();

  //Initialisiere LCD
  display.init();

  adcAttachPin(34); //AC-Voltagesensor
  analogReadResolution(12); //4095 Werte
}

/**********************************************************************/
/*                                                                    */
/*                 Loop0 (Core0)                                      */
/*                                                                    */
/**********************************************************************/
void loop0(void * parameter) {
  //Der normale Arduino loop laeuft automatisch in einer Schleife; die Schleife DARF nicht leer sein!!!
  for(;;) {
      commandLine();    //Pruefen, ob eine Kommandozeileneingabe vorhanden ist
      vTaskDelay(10);   //if not: Task watchdog got triggered. The following tasks did not feed the watchdog in time
      //0.9.9.94 Wird mehr als 10Min keine SBMS-Nachricht mehr empfangen und läuft der Batteriemodus, dann beende diesen
      long now = millis();
      long lrm = lastReceivedMillis;
      long diff = now - lrm;
      if(battery.isOn() && diff > 600000 && !inverter.stopBattery) {
        Log.error(F("Seit mehr als 10 Minuten wurde keine SBMS-Aktualisierung mehr empfangen, beende Batteriemodus\nnow: %s\nlastReceivedMillis: %d\ndiff: %d\nlrm: %d" CR), 
        now, lastReceivedMillis, diff, lrm);      
        inverter.stopBattery = true; //verhindert wiederanlaufen und die Wiederholung der Stopaufforderung ( loop geht vielfach pro Sekunde!!! )
        inverter.starteNetzvorrang(lastStatusMsg);
        inverter.setRed();
        digitalWrite(RELAY_4, HIGH); //Lüfter abschalten, da meist der Task1 hängt und darum keine Steuerung mehr erfolgt.
      } 
      //0.9.9.99 myWifi connection check
      if((now - lastConnectCheck) > 30000) {
        if(!myWifi.connected()) {
          Log.warning(F("myWifi ist nicht verbunden, versuche einen Reconnect"));    
          myWifi.reconnect();
        }
        //1.0.10 pegel und Stringdaten empfangen
        lastConnectCheck = now;
        controller.retrieveData();
      }
      //xSemaphoreGive(semaphore);
  }
}

/**********************************************************************/
/*                                                                    */
/*                 Loop (Core1, Default)                              */
/*                                                                    */
/**********************************************************************/
void loop() {

    if(!updater.stopForOTA) {

      taster.check();   //Buttonsteuerung (Inverter-/Batterieumschaltung)
      yield();      

      //SMBS-Werte auslesen (State of Charge, Cell voltages)
      if(sbms.read()) { 
        yield();
        inverter.check(); //oben ausgelesene Werte pruefen und ggfls. den Inverter umschalten
      }

      if(sma.hasNewPacket()) {       //energymeter lesen, wenn upd-Paket vorhanden, dann auswerten und beide Charger steuern
        yield();
        charger.checkOnIncome();     
      }

    }

    //xSemaphoreTake(semaphore, portMAX_DELAY); geht erst weiter, wenn erster Task das semaphore gegeben hat  
    
    //Restart erforderlich, wird durch updater-Objekt nach Upload einer neuen Firmware geregelt
    if(updater.restartRequired) {
      delay(2000);
      ESP.restart();
    }
}

/**********************************************************************/
/*                                                                    */
/*                 Functions                                          */
/*                                                                    */
/**********************************************************************/

void handleButton(AceButton* /* button */, uint8_t eventType, uint8_t /* buttonState */) {
    inverter.handleButtonPressed();
}

void commandLine() {
  if(Serial.available()) {
      String cmd = Serial.readStringUntil('\n'); 
      Log.warning(F("Echo: %s"), cmd);
      String msg = String((char*)0);
      msg.reserve(32);
      if(cmd.startsWith(F("restart wifi"))) {      
        myWifi.reconnect();
      } else if(cmd.startsWith(F("restart esp"))) {      
        msg = F("Restarting ESP...");
        Log.warning(F("Restarting ESP..." CR));
        wc.sendClients(msg.c_str());
        ESP.restart();
      } else if(cmd.startsWith(F("start RELAY_S1"))) {      
        msg = F("Starting RELAY_S1...");
        digitalWrite(RELAY_S1, LOW);        
      } else if(cmd.startsWith(F("stop RELAY_S1"))) {      
        msg = F("Stopping RELAY_S1...");
        digitalWrite(RELAY_S1, HIGH);
      } else if(cmd.startsWith(F("start RELAY_S2"))) {      
        msg = F("Starting RELAY_S2...");
        digitalWrite(RELAY_S2, LOW);        
      } else if(cmd.startsWith(F("stop RELAY_S2"))) {      
        msg = F("Stopping RELAY_S2...");
        digitalWrite(RELAY_S2, HIGH);
      } else if(cmd.startsWith(F("start RELAY_3"))) {      
        msg = F("Starting RELAY_3...");
        digitalWrite(RELAY_3, LOW);        
      } else if(cmd.startsWith(F("stop RELAY_3"))) {      
        msg = F("Stopping RELAY_3...");
        digitalWrite(RELAY_3, HIGH);
      } else if(cmd.startsWith(F("start RELAY_4"))) {      
        msg = F("Starting RELAY_4...");
        digitalWrite(RELAY_4, LOW);        
      } else if(cmd.startsWith(F("stop RELAY_4"))) {      
        msg = F("Stopping RELAY_4...");
        digitalWrite(RELAY_4, HIGH);       
      } else if(cmd.startsWith(F("start RELAY_W"))) {      
        msg = F("Starting RELAY_W...");
        digitalWrite(RELAY_PIN, LOW);        
      } else if(cmd.startsWith(F("stop RELAY_W"))) {      
        msg = F("Stopping RELAY_W...");
        digitalWrite(RELAY_PIN, HIGH);       
      } else if(cmd.startsWith(F("test on"))) {      
        msg = F("test simulation set to on");     
        testFixed = true;        
      } else if(cmd.startsWith(F("test off"))) {      
        msg = F("test simulation set to off");     
        testFixed = false;        
        wc.updateUi();
      } else if(cmd.startsWith(F("data "))) {      
        msg = F("Setze Testdaten: ");     
        testData = cmd.substring(5); 
        msg+=testData;     
      } else if(cmd.startsWith(F("tesla status"))) {          
        debugTesla=true;
        perry.readChargeState();
        debugTesla=false;
      } else if(cmd.startsWith(F("tesla wakeup"))) {          
        debugTesla=true;
        perry.wakeup();
      } else if(cmd.startsWith(F("tesla charge start"))) {     
        debugTesla=true;     
        perry.startCharge();
        debugTesla=false;
      } else if(cmd.startsWith(F("tesla charge stop"))) {    
        debugTesla=true;     
        perry.stopCharge();
        debugTesla=false;
      } else if(cmd.startsWith(F("tesla authorize"))) {      
        debugTesla=true;
        String password = cmd.substring(15); //alles hinter 'authorize'
        password.trim();
        perry.authorize(password.c_str());
        debugTesla=false;
      } else if(cmd.startsWith(F("debug on"))) {        
        debug = true;
      } else if(cmd.startsWith(F("debug off"))) {         
        debug = false;
      } else if(cmd.startsWith(F("battery off"))) {        
        debugInverter=true;
        inverter.stopBattery = true;
        inverter.starteNetzvorrang(F("Schalte Batterie ueber Kommandozeile ab"));
        inverter.setRed();        
        debugInverter=false;
      } else if(cmd.startsWith(F("battery on"))) {   
        debugInverter=true;      
        inverter.stopBattery = false;
        inverter.starteBatterie(F("Schalte Batterie ueber Kommandozeile an"));
        inverter.setGreen();
        debugInverter=false;
      } else if(cmd.startsWith(F("battery mode on"))) {        
        battery.enabled = true;      
      } else if(cmd.startsWith(F("battery mode off"))) {         
        battery.enabled = false;        
      } else if(cmd.startsWith(F("print"))) {         
        perry.print();
        config.print();
        sbms.print();
        wc.print();
        battery.print();
        controller.print();
        logs.print(false);
        myWifi.print();
        charger.print();
        inverter.print();
        display.print();
      } else if(cmd.startsWith(F("show heap"))) {
       // Log.warning(F("Free heap: %d" CR),ESP.getFreeHeap()); 
      } else if(cmd.startsWith(F("tesla control on"))) {
        teslaCtrlActive = true;
      } else if(cmd.startsWith(F("tesla control off"))) {
        teslaCtrlActive = false;
      } else if(cmd.startsWith(F("config load"))) {
        debugConfig=true;
        config.load();
        debugConfig=false;
      } else if(cmd.startsWith(F("config save"))) {
        config.save();
      } else if(cmd.startsWith(F("reset sma"))) {
        debugSma=true;
        sma.reset();
        debugSma=false;
      } else if(cmd.startsWith(F("config set"))) {
        debugConfig=true;
        String keyVal = cmd.substring(10); //alles hinter 'set'
        keyVal.trim();
        config.set(keyVal);
        debugConfig=false;
      } else if(cmd.startsWith(F("config persist"))) {
        debugConfig=true;
        String keyVal = cmd.substring(14); //alles hinter 'persist'
        keyVal.trim();
        String key = config.getValue(keyVal, ':', 0);
        String val = config.getValue(keyVal, ':', 1);
        config.save(key,val);
        debugConfig=false;
      } else if(cmd.startsWith(F("config show"))) {
        String key = cmd.substring(11); //alles hinter 'show'
        const char* val = config.load(key);
        Log.warningln(val);
      } else if(cmd.startsWith(F("retrieve data"))) {
        controller.debugCtrl=true;
        controller.retrieveData();
        controller.debugCtrl=false;
      } else if(cmd.startsWith(F("pwm "))) {      
        msg = F("setze PWM:");     
        String pwm = cmd.substring(4); 
        msg+=pwm; 
        String val = pwm.substring(5);    
        int dutyCycle = val.toInt();
        if(testFixed) {
          if(pwm.startsWith(F("io25"))) {
            ledcWrite(GPIO25, dutyCycle);
          } else if(pwm.startsWith(F("io26"))) {
            ledcWrite(GPIO26, dutyCycle);
          } else if(pwm.startsWith(F("io05"))) {
            ledcWrite(GPIO05, dutyCycle);          
          }
        } else {
          msg = F("please enable 'test on' first");
        }
      } else if(cmd.startsWith(F("cmd"))) {  
        String nrStr = cmd.substring(3);
        nrStr.trim();
        int nr = nrStr.toInt();
        switch(nr) {
          case 0:
            Log.warning(F("serialSBMS.flush();"));
            serialSBMS.flush();
            break;
          case 1:
            Log.warning(F("Serial.println(serialSBMS.available()); %s" CR), serialSBMS.available());
            break;             
          case 2:
            Log.warning(F("luefter.start();"));
            luefter.start();
            break;
          case 3:
            Log.warning(F("luefter.stop();"));
            luefter.stop();
            break;      
          case 4:
            Log.warning(F("Clear serialSBMS"));
            while(serialSBMS.available()) {
              serialSBMS.read();
            }
            break;
          default:
            Log.warning("Kein Kommando mit dieser Nummer gefunden");
        }
        
      } else if(cmd.startsWith(F("test wifi"))) { 
        bool ok = WiFi.status() == WL_CONNECTED;
        Log.warning(F("Wifi status: %T" CR), ok);
      } else if(cmd.startsWith(F("logs add"))) { 
        String entry = cmd.substring(8); 
        logs.append(entry);
      } else if(cmd.startsWith(F("reset flags"))) { 
        Log.warningln(F("Reset all debugging flags"));
        bitset = "0000000000";
        wc.updateUi();
      } else if(cmd.startsWith(F("verbose"))) { 
        esp_log_level_set("*", ESP_LOG_VERBOSE);
      } else if(cmd.startsWith(F("calibrate"))) { 
          Log.warningln(F("voltageSensor.calibrate();"));
          voltageSensor.calibrate();  
      } else {
        Log.warningln(F("Available commands:"));
        Log.warningln(F(" - battery on|off :: Schalte Batteriebetrieb an / aus"));
        Log.warningln(F(" - restart wifi  :: restarting Wifi connection"));
        Log.warningln(F(" - restart esp   :: restarting whole ESP32"));
        Log.warningln(F(" - reset flags   :: reset all debug flags"));  
        Log.warningln(F(" - reset sma   :: reset sma wg. udp.close()"));  
        Log.warningln(F(" - batterymode on|off   :: Batteriebetrieb blockieren (nur laden)"));  
        Log.warningln(F(" - start RELAY_S1|_S2|_3|_4|_W :: start relays S1,S2,3,4 und W"));
        Log.warningln(F(" - stop  RELAY_S1|_S2|_3|_4|_W :: stop relays S1,S2,3,4 und W"));
        Log.warningln(F(" - test  on|off :: enable/disable test simulation"));
        Log.warningln(F(" - debug  on|off :: enable/disable debug"));        
        Log.warningln(F(" - data  TESTDATA :: Testdaten setzen"));
        Log.warningln(F(" - logs add 'some log entry' :: Logeintraeg schreiben"));
        Log.warningln(F(" - cmd NR :: Kommando mit der u.a. Nummer ausfuehren"));
        Log.warningln(F(" -      0 :: serialSBMS.flush();"));
        Log.warningln(F(" -      1 :: Log.warningln(serialSBMS.available());"));
        Log.warningln(F(" -      2 :: luefter.start();"));
        Log.warningln(F(" -      3 :: luefter.stop();"));
        Log.warningln(F(" -      4 :: clear serialSBMS"));        
        Log.warningln(F(" - pwm io26|io25|io05 PERCENTAGE :: PWM setzen (nur wenn test on)"));
        Log.warningln(F(" - tesla authorize password :: Wieder anmelden (neues bearer token erzeugen)"));
        Log.warningln(F(" - tesla status :: Check tesla charge state"));
        Log.warningln(F(" - tesla wakeup :: Wake tesla"));
        Log.warningln(F(" - tesla charge start :: Start charging tesla and setting charge level to 90%"));
        Log.warningln(F(" - tesla charge stop :: Stop charging tesla and setting charge level to 50%"));
        Log.warningln(F(" - tesla control on|off :: Starte/Stoppe Tesla ChargeKontrolle (wird nicht gespeichert)"));
        Log.warningln(F(" - config load|save :: Schreiben/Lesen der Konfig aus LITTLEFS"));
        Log.warningln(F(" - config set key:value :: Hinzufuegen/aendern eines Konfigwertes (ohne Speichern!), z.B. socLimit"));
        Log.warningln(F(" - config persist key:value :: Speichern/aendern eines Konfigwertes (mit Speichern!), z.B. socLimit"));        
        Log.warningln(F(" - config show key :: Ausgabe des gespeicherten Values von 'key' auf Serial"));
        Log.warningln(F(" - show heap :: Schreibe den noch verfuegbaren Heap in die Ausgabe"));
        Log.warningln(F(" - test wifi :: Verbindungsstatus von Wifi ausgeben"));
        Log.warningln(F(" - retrieve data :: Dateneinsammeln und ausgeben"));
        Log.warningln(F(" - verbose :: Aktiviert ESP verbose logging ( esp_log_level_set('*', ESP_LOG_VERBOSE) )"));
        Log.warningln(F(" - print :: Schreibe einige abgeleitete Werte auf den Bildschirm"));
        Log.warningln(F(" - calibrate :: Calibrate AC voltage sensor and output it to serial"));
        return;
      }
      Log.warningln(msg.c_str());
      wc.sendClients(msg.c_str());
    }  
} 
