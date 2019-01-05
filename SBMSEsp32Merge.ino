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
/*                                                                    */
/**********************************************************************/
void setup() {

  Serial.begin(115200);  //USB
  Serial1.begin(9600, SERIAL_8N1, 16, 17); //Serial1 Pins 4,2, Serial2 Pins 16,17
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

  digitalWrite(RELAY_S1, HIGH);
  digitalWrite(RELAY_S2, HIGH);

  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);

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
    sbms.readSbms();  //SMBS-Werte auslesen (State of Charge, Cell voltages
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
      if(cmd.startsWith("restart wifi")) {      
        myWifi.reconnect();
      } else if(cmd.startsWith("restart esp")) {      
        Serial.println("Restarting ESP...");
        ESP.restart();
      } else {
        Serial.println("Available commands:");
        Serial.println(" - restart wifi  :: restarting Wifi connection");
        Serial.println(" - restart esp   :: restarting whole ESP32");
      }
    }  
}
