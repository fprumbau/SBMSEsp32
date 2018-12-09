#include <AceButton.h>
#include <ButtonConfig.h>

#include "html.h"
#include "global.h"
#include "config.h"

using namespace ace_button;

ButtonConfig tasterConfig;
AceButton taster(&tasterConfig);

//Ticker ticker;
int counter = 0;

unsigned long lastCheckedMillis = -1;

//findet die Checkmethode falsche Werte vor, so wird noch einmal
//(4s) gewartet, bevor diese tatsächlich zu einem Fehler führen.
int failureCount = 0;
const int errLimit = 5;

int LED_RED = 12;
int LED_GREEN = 14;
int LED_BLUE = 27;

/**
 * Pruefen aller Werte
*/
void checkValues();

void setBlue();
void setGreen();
void setRed();

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
/* Setup                                                              */
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

  //manuell Invertersteuerung
  pinMode(TASTER, INPUT_PULLUP);
  tasterConfig.setEventHandler(handleButton);
  taster.init(TASTER, HIGH, 0 /* id */);

  digitalWrite(RELAY_S1, HIGH);
  digitalWrite(RELAY_S2, HIGH);

  //Leds
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_S1, OUTPUT);
  pinMode(LED_S2, OUTPUT);

  //Startup-LED-Test
  digitalWrite(LED_S1, HIGH);
  digitalWrite(LED_S2, HIGH);
  setRed();
  delay(600);
  setGreen();
  delay(600);
  setBlue(); //Beim Laden BLAU zeigen
  delay(600);
  digitalWrite(LED_S1, LOW);
  digitalWrite(LED_S2, LOW);
  //LED-Test ENDE

  // etabliere Wifi Verbindung
  myWifi.connect();

  // start Webserver
  //server.on("/", sbmsPage);
  server.on("/sbms", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", html);
  });

  //Registriere Eventhandler WebsocketEvents
  ws.onEvent(onWsEvent);
    
  server.addHandler(&ws);
  
  server.begin();
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
    taster.check(); //AceButton
    yield();
    sbms.readSbms();
    yield();
    if (( millis() - lastCheckedMillis ) > 3000) { //Pruefung hoechstens alle 3 Sekunden
      Serial.print("Check...  ; failureCount: ");
      Serial.println(failureCount);
      lastCheckedMillis = millis();
      checkValues();
    }
    yield();
    sma.read(); //energymeter lesen, wenn upd-Paket vorhanden
    yield();
    commandLine();
  }
  
  //Restart erforderlich
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

void checkValues()  {
  if (soc < 0) return; //die Main-Loop sollte erstmal Werte lesen

  if (testFixed) {
    return; //keine Auswertung, wenn Testwerte
  }

  boolean stop = false;
  String message = "";
  if (soc < inverter.SOC_LIMIT) {
    message = "State of charge below ";
    message += inverter.SOC_LIMIT;
    message += "%";
    stop = true;
  }
  if (!stop) {
    for (int k = 0; k < 7; k++) {
      if (cv[k] < inverter.LOW_VOLTAGE_MILLIS) {
        message = "Undervoltage cell: ";
        message += k;
        stop = true;
      }
    }
  }
  if (stop) {
    failureCount++;
    if (failureCount < errLimit) { //einen 'Fehlversuch' ignorieren.
      if (debug) {
        Serial.print("Error found, waiting until failureCount reaches ");
        Serial.print(errLimit);
        Serial.print("; now: ");
        Serial.println(failureCount);
      }
    } else {
      if (!inverter.stopBattery) {
        if (debug) {
          Serial.println("Error limit reached, stopping inverter...");
        }
      }
      inverter.stopBattery = true; //
      inverter.starteNetzvorrang("Interrupt(NZV); " + message);
      setRed();
    }
  } else {
    if (failureCount >= errLimit) {
      failureCount = 0;
    }
    //Hier sollte nicht die Batterie gestartet, sondern nur freigeschaltet werden!!!
    inverter.stopBattery = false;
    setGreen();
  }
}

void handleButton(AceButton* /* button */, uint8_t eventType, uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("Button pressed");

      bool relayStatus = digitalRead(RELAY_PIN);
      if (relayStatus == HIGH) {
        // starte Netzvorrang
        inverter.starteNetzvorrang("Buttonaction");
      } else {
        if (!inverter.stopBattery) {
          inverter.starteBatterie("Buttonaction");
        } else {
          if (debug) {
            Serial.println("ON, kann Netzvorrang nicht abschalten (Stop wegen SOC oder Low Voltage)");
          }
        }
      }
      break;
  }
}

void setBlue() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

void setGreen() {
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
}

void setRed() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, LOW);
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
