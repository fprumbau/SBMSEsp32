#include <AceButton.h>
#include <ButtonConfig.h>
#include <ArduinoJson.h>

#include "global.h"
#include "webpage.h"
#include "config.h"

using namespace ace_button;

ButtonConfig tasterConfig;
AceButton taster(&tasterConfig);

//Ticker ticker;
int counter = 0;

long soc = -1; //aktueller Wert State Of Charge
int cv[8]; //aktuelle Zellspannungen

//Empfangstimeout ( wird 10s nichts empfangen, muss die Batterie abgeschaltet werden )
long timeout = 10000;
unsigned long lastReceivedMillis = -1;
unsigned long lastCheckedMillis = -1;

//findet die Checkmethode falsche Werte vor, so wird noch einmal
//(4s) gewartet, bevor diese tatsächlich zu einem Fehler führen.
int failureCount = 0;
const int errLimit = 5;

int LED_RED = 12;
int LED_GREEN = 14;
int LED_BLUE = 27;

//manuelle Inverterumschaltung
int TASTER = 19;

//nicht auf Serial1 warten, Feste Werte annehmen
bool testFixed = true;
const char* hostName = "esp32b";

/*
 * Schreibt die Webseite in Teilen (<6kb)
 * auf den Webclient, der sich gerade verbunden
 * hat. Es ist wichtig, hier die korrekte
 * Laenge des contents zu senden. Weitere Teile
 * sollten immer mit server.sendContent_P(partN)
 * versendet werden. Das _P ist hier wichtig, da
 * die Seitendefinition im PROGMEM liegen (s. webpage.h)
*/
void sbmsPage();

/**
 * SBMS über serielle Schnittstelle auslesen.
 * Wird 'testFixed' gesetzt (s.o.), dann wird
 * hier immer nur ein fester Werte ausgewertet.
 */
void readSbms();

/**
 * Toggle:
 * - @d1-true
 * - @d1-false
 * - @d2-true
 * - @d2-false
*/
void toggleDebug(unsigned char* payload);

/**
 * Pruefen aller Werte
*/
void checkValues();

void setBlue();
void setGreen();
void setRed();

/**
 * Netzvorrang starten
*/
void starteNetzvorrang(String reason);

/**
 * Batteriebetrieb starten
*/
void starteBatterie(String reason);

/**
 * Buttonsteuerung, um manuell den Inverter schalten zu koennen
*/
void handleButton(AceButton*, uint8_t eventType, uint8_t);

/**
 * Websocket-Events, wenn neue Clients sich verbinden, wenn die clients
 * selbst senden oder wenn sie geschlossen werden.
*/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

/**********************************************************************/
/*                                                                    */
/* Setup                                                              */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
void setup() {

  Serial.begin(115200);  //USB
  Serial1.begin(9600, SERIAL_8N1, 16, 17); //Serial1 Pins 4,2, Serial2 Pins 16,17

  Serial.println("Starting...");

  //Pins fuer Taster und Relay initialisieren
  pinMode(vars.RELAY_PIN, OUTPUT);
  pinMode(vars.RELAY_S1, OUTPUT);
  pinMode(vars.RELAY_S2, OUTPUT);

  //manuell Invertersteuerung
  pinMode(TASTER, INPUT_PULLUP);
  tasterConfig.setEventHandler(handleButton);
  taster.init(TASTER, HIGH, 0 /* id */);

  digitalWrite(vars.RELAY_S1, HIGH);
  digitalWrite(vars.RELAY_S2, HIGH);

  //Leds
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(vars.LED_S1, OUTPUT);
  pinMode(vars.LED_S2, OUTPUT);

  //Startup-LED-Test
  digitalWrite(vars.LED_S1, HIGH);
  digitalWrite(vars.LED_S2, HIGH);
  setRed();
  delay(600);
  setGreen();
  delay(600);
  setBlue(); //Beim Laden BLAU zeigen
  delay(600);
  digitalWrite(vars.LED_S1, LOW);
  digitalWrite(vars.LED_S2, LOW);
  //LED-Test ENDE

  // etabliere Wifi Verbindung
  myWifi.connect();

  // register WebsocketServer handler and start server
  wc.begin(webSocketEvent);

  // start Webserver
  server.on("/", sbmsPage);
  server.on("/sbms", sbmsPage);
  server.begin();

  // initialize other the air updates
  ota.init(hostName);
}

/**********************************************************************/
/*                                                                    */
/*                 Loop                                               */
/*                                                                    */
/**********************************************************************/
void loop() {
  taster.check(); //AceButton
  yield();
  wc.loop();
  yield();
  server.handleClient();
  yield();
  readSbms();
  yield();
  if (( millis() - lastCheckedMillis ) > 3000) { //Pruefung hoechstens alle 3 Sekunden
    Serial.print("Check...  ; failureCount: ");
    Serial.println(failureCount);
    lastCheckedMillis = millis();
    checkValues();
  }
  yield();
  sma.read(); //energymeter lesen, wenn upd-Paket vorhanden
}

/**********************************************************************/
/*                                                                    */
/*                 Functions                                          */
/*                                                                    */
/**********************************************************************/
void sbmsPage() {
  long s1 = sizeof(part1);
  long s2 = sizeof(part2);
  long s3 = sizeof(part3);

  String connStr = "var connection = new ReconnectingWebSocket('ws://";
  connStr += myWifi.getIpAddress();
  //connStr += ":81/', ['arduino']);"; //für new WebSocket('ws..
  connStr += ":81/', null, { debug:true, reconnectInterval: 6000, reconnectDecay: 1.1, maxReconnectInterval: 10000 });";
  int s4 = connStr.length();

  long totalSize = s1 + s2 + s3 + s4;
  if (vars.debug) {
    Serial.print("\np1: ");
    Serial.println(s1);
    Serial.print("p2: ");
    Serial.println(s2);
    Serial.print("total: ");
    Serial.println(totalSize);
    Serial.println("");
  }
  server.setContentLength(totalSize);
  server.send_P(200, "text/html", part1);
  server.sendContent_P(part2);
  server.sendContent(connStr);
  server.sendContent_P(part3);
}

void readSbms() {

  String sread;

  if (!testFixed) {
    while (Serial1.available()) {
      sread = Serial1.readString();
    }
  } else {
    //sread = "5+'/,D$+HNGpGtGuGkH9H5HD+J##-#$'#####&##################$|(";
    //sread = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
    //sread = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
    //sread = "#$7%XS$*GOGRGTGPGOGRGOGP*]##-##9##E#####################%N(";
    sread = "#$87%K$*GDGGGPGDG2GLGLGL*m##-##:##@#####################%N(";
  }
  sread.trim();
  int len = sread.length();

  /**
     Solange etwas empfangen wird (sread gefuellt) sollte ausgewertet werden.
     Wenn aber der Timeout zuschlaegt, dann fuehrt das Lesen der nicht empfangenen
     Werte, dazu, soc und cv[] zurueckzusetzen, woraufhin der naechste Lauf der
     Interruptmethode isrHandler(..) dazu, dass die Status-LED auf rot schaltet.
     Gleichzeitig ist es nicht mehr möglich, auf Batterie zu wechseln.

     Ist die Batterie gerade aktiv, wird das Relais wieder zurückgeschaltet (normal connected)
  */
  if (len > 1 || ( millis() - lastReceivedMillis ) > timeout ) {
    if (( millis() - lastReceivedMillis ) > 3000) { //Verarbeitung hoechstens alle 3 Sekunden
      if (vars.debug && len > 0) {
        Serial.print(".____");
        Serial.print(sread);
        Serial.println("____.");
        Serial.print("Length 'sread': ");
        Serial.println(len);
      };

      //Wert zu Clients publishen (wird dort in Webseite visualisiert oder gelisted)
      if (wc.ready) {
        if (len > 0) {
          wc.sendClients(sread, true);
        }
      } else {
        if (!wc.notifiedNoClient) {
          wc.notifiedNoClient = true;
          if (vars.debug) {
            Serial.println("no client connected, yet");
          }
        }
      }

      //Wert soc zurücksetzen (Wichtig, wenn mehrere Male nichts gelesen wird, also sread.length=0,dann muss erst der failureCount
      // hochgehen und nachher und schliesslich der Fehlermodus aktiviert werden (Batteriesperre)
      soc = 0;

      //Werte nun lesen
      if (len > 0) {

        const char* txt = sread.c_str();

        String outString = "\nSOC: ";
        if (len >= 8) {
          soc = sbms.dcmp(6, 2, txt, len);
          outString += soc;
          outString += " ( Limit: ";
          outString += battery.SOC_LIMIT;
          outString += " ) \n";
        }
        if (len >= 24) {
          for (int k = 0; k < 8; k++) {
            int loc = k * 2 + 8;
            cv[k] = sbms.dcmp(loc, 2, txt, len);

            outString += "\ncv";
            outString += ( k + 1 );
            outString += ": ";
            outString += cv[k];
          }
        }

        //Werte
        if (vars.debug) {
          Serial.println(outString);
          Serial.print("StopBattery: ");
          Serial.println(battery.stopBattery);
          Serial.println("_______________________________________");
        }

        if (vars.debug) {
          String mem = " Heap (free): ";
          mem += ESP.getFreeHeap();
          wc.sendClients(mem , false);
        }

        //Timeoutcounter nur zuruecksetzen, wenn etwas empfangen wurde
        lastReceivedMillis = millis();
      }
    }
  }
}

/**
   Toggle:
   - @d1-true
   - @d1-false
   - @d2-true
   - @d2-false
*/
void toggleDebug(unsigned char* payload) {
  String msg;
  if (payload[2] == '1') {
    if (payload[4] == 't') {
      vars.debug = true;
      msg = "Switched debug1 to true";
    } else {
      vars.debug = false;
      msg = "Switched debug1 to false";
    }
  } else {
    if (payload[4] == 't') {
      vars.debug2 = true;
      msg = "Switched debug2 to true";
    } else {
      vars.debug2 = false;
      msg = "Switched debug2 to false";
    }
  }
  if (vars.debug2) {
    Serial.println(msg);
  }
  wc.sendClients(msg, false);
}

void checkValues()  {
  if (soc < 0) return; //die Main-Loop sollte erstmal Werte lesen

  if (testFixed) {
    return; //keine Auswertung, wenn Testwerte
  }

  boolean stop = false;
  String message = "";
  if (soc < battery.SOC_LIMIT) {
    message = "State of charge below ";
    message += battery.SOC_LIMIT;
    message += "%";
    stop = true;
  }
  if (!stop) {
    for (int k = 0; k < 7; k++) {
      if (cv[k] < battery.LOW_VOLTAGE_MILLIS) {
        message = "Undervoltage cell: ";
        message += k;
        stop = true;
      }
    }
  }
  if (stop) {
    failureCount++;
    if (failureCount < errLimit) { //einen 'Fehlversuch' ignorieren.
      if (vars.debug) {
        Serial.print("Error found, waiting until failureCount reaches ");
        Serial.print(errLimit);
        Serial.print("; now: ");
        Serial.println(failureCount);
      }
    } else {
      if (!battery.stopBattery) {
        if (vars.debug) {
          Serial.println("Error limit reached, stopping battery...");
        }
      }
      battery.stopBattery = true;
      starteNetzvorrang("Interrupt(NZV); " + message);
      setRed();
    }
  } else {
    if (failureCount >= errLimit) {
      failureCount = 0;
    }
    //Hier sollte nicht die Batterie gestartet, sondern nur freigeschaltet werden!!!
    battery.stopBattery = false;
    setGreen();
  }
}

/**
   Netzvorrang starten
*/
void starteNetzvorrang(String reason) {
  String msg = "";
  if (digitalRead(vars.RELAY_PIN) == HIGH) {
    digitalWrite(vars.RELAY_PIN, LOW); //ON, d.h. Netzvorrang aktiv
    wc.sendClients("Toggle battery LOW", false);
    msg += "Starte Netzvorrang :: ";
    msg += reason;
    msg += '\n';
  } else {
    if (vars.debug)  msg = "Kann Netzvorrang nicht starten, da schon aktiv\n";
  }
  if (vars.debug) {
    msg += "wc.clientCount: ";
    msg += wc.clientCount;
    msg += '\n';
    msg += reason;
    msg += '\n';
  }
  if (msg.length() > 0) {
    if (vars.debug) {
      Serial.println(msg);
    }
    wc.sendClients(msg, false);
  }
}

/**
   Batteriebetrieb starten
*/
void starteBatterie(String reason) {
  String msg = "";
  if (!battery.stopBattery) {
    if (digitalRead(vars.RELAY_PIN) == LOW) {
      digitalWrite(vars.RELAY_PIN, HIGH); //OFF, d.h. Batterie aktiv
      wc.sendClients("Toggle battery HIGH", false);
      msg += "Starte Netzvorrang :: ";
      msg += reason;
      msg += '\n';
    } else {
      return;
    }
  } else {
    msg = "Kann Netzvorrang nicht stoppen, da Stopflag aktiv\n";
  }
  if (vars.debug) {
    msg += "wc.clientCount: ";
    msg += wc.clientCount;
    msg += '\n';
  }
  if (msg.length() > 0) {
    if (vars.debug) {
      Serial.println(msg);
    }
    wc.sendClients(msg, false);
  }
}

void handleButton(AceButton* /* button */, uint8_t eventType, uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("Button pressed");

      bool relayStatus = digitalRead(vars.RELAY_PIN);
      if (relayStatus == HIGH) {
        // starte Netzvorrang
        starteNetzvorrang("Buttonaction");
      } else {
        if (!battery.stopBattery) {
          starteBatterie("Buttonaction");
        } else {
          if (vars.debug) {
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

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED: {
        //Client 'num' aus Liste rausnehmen
        uint8_t newClients[256];
        for (int a = 0; a < 256; a++) {
          newClients[a] = wc.clients[a];
        }
        int c = 0;
        for (int x = 0; x < wc.clientCount; x++) {
          if (num != newClients[x]) {
            wc.clients[c] = newClients[x];
            c++;
          } else {
            wc.clientCount--;
          }
        }
        if (wc.clientCount == 0) {
          wc.notifiedNoClient = false;
          wc.ready = false;
        }
        if (vars.debug) {
          Serial.printf("[%u] Disconnected! Remaining %u\n", num, wc.clientCount);
        }
        break;
      }
    case WStype_CONNECTED: {
        IPAddress ip = wc.remoteIP(num);
        Serial.println("");

        // send message to client
        String msg = "@ Connected";
        wc.sendTXT(num, msg);

        bool alreadyListed = false;
        int y = 0;
        for (; y < wc.clientCount; y++) {
          if (num == wc.clients[y]) {
            alreadyListed = true;
            break;
          }
        }
        if (!alreadyListed) {
          wc.clients[y] = num;
          wc.clientCount++;
        }
        if (vars.debug) {
          Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s; ConnCount: %u\n", num, ip[0], ip[1], ip[2], ip[3], payload, wc.clientCount);
        }
        wc.ready = true;
        //mit JSON
        StaticJsonBuffer<300> jsonBuffer; //letzte Zaehlung: 114
        JsonObject& root = jsonBuffer.createObject();
        root["d1"]=vars.debug;
        root["d2"]=vars.debug2;
        root["s1"]=sma.isChargerOn(1);
        root["s2"]=sma.isChargerOn(2);
        root["battery"]=battery.isBatteryOn();
        char jsonChar[512];
        root.printTo(jsonChar);
        String str(jsonChar);
        if(vars.debug2) {
          Serial.println(str);
        }
        wc.sendTXT(num, str);        
        break;
      }
    case WStype_TEXT:
      if (vars.debug) {
        Serial.printf("[Client %u] received: %s\n", num, payload);
      }
      //if(length > 1) {
      if (payload[0] == '@') {
        if (payload[1] == '+') {
          starteBatterie("Websockets");
        } else if (payload[1] == '-') {
          starteNetzvorrang("Websockets");
        } else if (payload[1] == 's' && length > 3) {
          //Solar charger s1 an / ausschalten
          if (payload[2] == '1') {
            if (payload[3] == '+') {
              //s1 anschalten
              sma.toggleCharger(1, true, true);
              wc.sendClients("s1 an", false);
            } else {
              //s1 abschalten
              sma.toggleCharger(1, false, true);
              wc.sendClients("s1 aus", false);
            }
          } else {
            if (payload[3] == '+') {
              //s2 anschalten
              sma.toggleCharger(2, true, true);
              wc.sendClients("s2 an", false);
            } else {
              //s2 abschalten
              sma.toggleCharger(2, false, true);
              wc.sendClients("s2 aus", false);
            }
          }
        }
        if (payload[1] == 'd') { //&& length > 4
          toggleDebug(payload);
        }
      }
      //}
      break;
  }
}
