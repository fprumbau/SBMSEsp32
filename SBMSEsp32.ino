#include <AceButton.h>
#include <ButtonConfig.h>

#include "html.h"
#include "global.h"

using namespace ace_button;

ButtonConfig tasterConfig;
AceButton taster(&tasterConfig);

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

  Serial.begin(115200);  //USB Serial1 Pins 4,2, 
  
  //WROOM hat 16/17 auf RX2/TX2 verbunden
  Serial1.begin(9600, SERIAL_8N1, 16, 17); //Serial2 Pins 16,17

  //WROVER-B hier sind die Pins 16+17 disabled, 
  //Serial1.begin(9600, SERIAL_8N1, 15, 17); //TX2 wird nicht gebraucht (Kommunikation -> SBMS), RX2 auf GPIO15, hierfür muss dann in der Schaltung GPIO16 auf GPIO15 gebrückt werden!
  
  Serial2.begin(115200); //wegen Restart

  Serial.println("Starting...");

  //Pins fuer Taster und Relay initialisieren
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_S1, OUTPUT);
  pinMode(RELAY_S2, OUTPUT);

  //v.0.9.9.40
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);

  //manuell Invertersteuerung
  pinMode(TASTER, INPUT_PULLUP);
  tasterConfig.setEventHandler(handleButton);
  taster.init(TASTER, HIGH, 0 /* id */);

  digitalWrite(RELAY_S1, HIGH); //Schaltet S1 (HLG600B, regelbar) an/aus (240V!)
  digitalWrite(RELAY_S2, HIGH); //Schaltet S2 (HLG600A, fest) an/aus (Remoteeingang des Laders)

  digitalWrite(RELAY_3, HIGH); //Schaltet S2 (s.o.) an/aus (Remoteeingang des Laders)
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
  
  // Zugriff von aussen
  server.on("/sbms", HTTP_GET, [](AsyncWebServerRequest *request){
    //if(!request->authenticate("admin", "Go8319!"))
    //    request->redirect("/login");
    request->send(200, "text/html", html);
  });

  server.on("/lbprobe", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "online");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
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

  // v.0.9.9.65 favicon via SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

/**********************************************************************/
/*                                                                    */
/*                 Loop                                               */
/*                                                                    */
/**********************************************************************/
void loop() {
  if(!updater.stopForOTA) {
    taster.check();   //Buttonsteuerung (Inverter-/Batterieumschaltung)
    yield();
    sbms.readSbms();  //SMBS-Werte auslesen (State of Charge, Cell voltages)
    yield();
    inverter.check(); //oben ausgelesene Werte pruefen und ggfls. den Inverter umschalten
    yield();
    sma.read();       //energymeter lesen, wenn upd-Paket vorhanden, dann auswerten und beide Charger steuern
    yield();
    commandLine();    //Pruefen, ob eine Kommandozeileneingabe vorhanden ist
  }
  
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
      String cmd = Serial.readString();
      Serial.print("Echo: ");
      Serial.println(cmd);
      String msg = "-";
      if(cmd.startsWith("restart wifi")) {      
        myWifi.reconnect();
      } else if(cmd.startsWith("restart esp")) {      
        msg = "Restarting ESP...";
        Serial.println(msg);
        wc.sendClients(msg);
        ESP.restart();
      } else if(cmd.startsWith("start RELAY_S1")) {      
        msg = "Starting RELAY_S1...";
        digitalWrite(RELAY_S1, LOW);        
      } else if(cmd.startsWith("stop RELAY_S1")) {      
        msg = "Stopping RELAY_S1...";
        digitalWrite(RELAY_S1, HIGH);
      } else if(cmd.startsWith("start RELAY_S2")) {      
        msg = "Starting RELAY_S2...";
        digitalWrite(RELAY_S2, LOW);        
      } else if(cmd.startsWith("stop RELAY_S2")) {      
        msg = "Stopping RELAY_S2...";
        digitalWrite(RELAY_S2, HIGH);
      } else if(cmd.startsWith("start RELAY_3")) {      
        msg = "Starting RELAY_3...";
        digitalWrite(RELAY_3, LOW);        
      } else if(cmd.startsWith("stop RELAY_3")) {      
        msg = "Stopping RELAY_3...";
        digitalWrite(RELAY_3, HIGH);
      } else if(cmd.startsWith("start RELAY_4")) {      
        msg = "Starting RELAY_4...";
        digitalWrite(RELAY_4, LOW);        
      } else if(cmd.startsWith("stop RELAY_4")) {      
        msg = "Stopping RELAY_4...";
        digitalWrite(RELAY_4, HIGH);       
      } else if(cmd.startsWith("start RELAY_W")) {      
        msg = "Starting RELAY_W...";
        digitalWrite(RELAY_PIN, LOW);        
      } else if(cmd.startsWith("stop RELAY_W")) {      
        msg = "Stopping RELAY_W...";
        digitalWrite(RELAY_PIN, HIGH);       
      } else if(cmd.startsWith("test on")) {      
        msg = "test simulation on...";     
        testFixed = true;        
      } else if(cmd.startsWith("test off")) {      
        msg = "test simulation off...";     
        testFixed = false;        
      } else if(cmd.startsWith("data ")) {      
        msg = "setze Testdaten:";     
        testData = cmd.substring(5); 
        msg+=testData;     
      } else if(cmd.startsWith("pwm ")) {      
        msg = "setze PWM:";     
        String pwm = cmd.substring(4); 
        msg+=pwm; 
        String val = pwm.substring(5);    
        int dutyCycle = val.toInt();
        if(testFixed) {
          if(pwm.startsWith("io25")) {
            ledcWrite(GPIO25, dutyCycle);
          } else if(pwm.startsWith("io26")) {
            ledcWrite(GPIO26, dutyCycle);
          } else if(pwm.startsWith("io05")) {
            ledcWrite(GPIO05, dutyCycle);          
          }
        } else {
          msg = "please enable 'test on' first";
        }
      } else {
        Serial.println("Available commands:");
        Serial.println(" - restart wifi  :: restarting Wifi connection");
        Serial.println(" - restart esp   :: restarting whole ESP32");
        Serial.println(" - start RELAY_S1|_S2|_3|_4|_W :: start relays S1,S2,3,4 und W");
        Serial.println(" - stop  RELAY_S1|_S2|_3|_4|_W :: stop relays S1,S2,3,4 und W");
        Serial.println(" - test  on|off :: enable test simulation");
        Serial.println(" - data  TESTDATA :: Testdaten setzen");
        Serial.println(" - pwm io26|io25|io05 PERCENTAGE :: PWM setzen (nur wenn test on)");
        return;
      }
      Serial.println(msg);
      wc.sendClients(msg);
    }  
}
