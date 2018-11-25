#include <WebServer.h>
#include <WebSocketsServer.h>

#include "MyWifi.h"
#include "webpage.h"
#include "config.h"
#include "SBMS.h"
#include "Taster.h"
#include "Relay.h"
#include "Battery.h"
#include "OTA.h"

OTA ota;
SBMS sbms;

MyWifi myWifi;
Battery battery;
WebServer server(80);
WebSocketsServer wsServer = WebSocketsServer(81);

//client connected to send?
bool ready = false;

uint8_t clientCount = 0;
uint8_t clients[256] = { -1};
bool notifiedNoClient = false;

//Ticker ticker;
int counter = 0;

bool debug = false;
bool debug2 = false;

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

unsigned long wsServerLastSend = -1;

int LED_RED = 12;
int LED_GREEN = 14;
int LED_BLUE = 27;

int LED_S1 = 25;
int LED_S2 = 26;

/*
   Schreibt die Webseite in Teilen (<6kb)
   auf den Webclient, der sich gerade verbunden
   hat. Es ist wichtig, hier die korrekte
   Laenge des contents zu senden. Weitere Teile
   sollten immer mit server.sendContent_P(partN)
   versendet werden. Das _P ist hier wichtig, da
   die Seitendefinition im PROGMEM liegen (s. webpage.h)
*/
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
  if (debug) {
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

/**
   Sende Daten zu allen über Websockets verbundenen
   Clients. Alles, was NICHT SBMS-Daten sind, also
   Fehler- bzw. Statusmeldungen MUSS mit einem '@'
   eingeleitet werden, sonst wird es von der Webseite
   falsch interpretiert und führt zu wilden Werten
   z.B. beim Batteriestatus.
*/
void sendClients(String msg, bool data) {
  if (clientCount <= 0) {
    return;
  }
  /*if(wsServerLastSend>0 && (millis()-wsServerLastSend) < 100) {
    if(debug) {
      Serial.print("Could not send data multiple times in 100ms; disgarding ");
      Serial.println(data);
    }
    return;
    }*/
  wsServerLastSend = millis();
  for (int m = 0; m < clientCount; m++) {
    uint8_t client = clients[m];
    if (debug) {
      Serial.printf("Sending client %u ( %u ) from %u clients\n", (m + 1), client, clientCount);
    }
    if (!data) {
      msg = "@ " + msg;
    }
    wsServer.sendTXT(client, msg);
  }
}

/**
   Websocket-Events, wenn neue Clients sich verbinden, wenn die clients
   selbst senden oder wenn sie geschlossen werden.
*/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED: {
        //Client 'num' aus Liste rausnehmen
        uint8_t newClients[256];
        for (int a = 0; a < 256; a++) {
          newClients[a] = clients[a];
        }
        int c = 0;
        for (int x = 0; x < clientCount; x++) {
          if (num != newClients[x]) {
            clients[c] = newClients[x];
            c++;
          } else {
            clientCount--;
          }
        }
        if (clientCount == 0) {
          notifiedNoClient = false;
          ready = false;
        }
        if (debug) {
          Serial.printf("[%u] Disconnected! Remaining %u\n", num, clientCount);
        }
        break;
      }
    case WStype_CONNECTED: {
        IPAddress ip = wsServer.remoteIP(num);
        Serial.println("");

        // send message to client
        wsServer.sendTXT(num, "@ Connected");

        bool alreadyListed = false;
        int y = 0;
        for (; y < clientCount; y++) {
          if (num == clients[y]) {
            alreadyListed = true;
            break;
          }
        }
        if (!alreadyListed) {
          clients[y] = num;
          clientCount++;
        }
        if (debug) {
          Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s; ConnCount: %u\n", num, ip[0], ip[1], ip[2], ip[3], payload, clientCount);
        }
        ready = true;

        //Relaistatus uebermitteln
        relayStatus = digitalRead(RELAY_PIN);
        String msg = "Batteriestatus: "; //wird die Message von @ eingeleitet, wird sie nicht als SBMS-Datum interpretiert!
        if (relayStatus == 0) {
          msg += "LOW";
        } else {
          msg += "HIGH";
        }
        sendClients(msg, false);

        break;
      }
    case WStype_TEXT:
      if (debug) {
        Serial.printf("[Client %u] received: %s\n", num, payload);
      }
      //if(length > 1) {
        if (payload[0] == '@') {
          if (payload[1] == '+') {
            starteBatterie("Websockets");
          } else if (payload[1] == '-') {
            starteNetzvorrang("Websockets");
          }
          if (payload[1] == 'd') { //&& length > 4
            toggleDebug(payload);
          }
        }
      //}
      break;
  }
}

//nicht auf Serial1 warten, Feste Werte annehmen
bool testFixed = false;

void readSbms() {

  String sread;

  if (!testFixed) {
    while (Serial1.available()) {
      sread = Serial1.readString();
    }
  } else {
    //sread = "5+'/,D$+HNGpGtGuGkH9H5HD+J##-#$'#####&##################$|(";
    //sread = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
    sread = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
  }

  /**
     Solange etwas empfangen wird (sread gefuellt) sollte ausgewertet werden.
     Wenn aber der Timeout zuschlaegt, dann fuehrt das Lesen der nicht empfangenen
     Werte, dazu, soc und cv[] zurueckzusetzen, woraufhin der naechste Lauf der
     Interruptmethode isrHandler(..) dazu, dass die Status-LED auf rot schaltet.
     Gleichzeitig ist es nicht mehr möglich, auf Batterie zu wechseln.

     Ist die Batterie gerade aktiv, wird das Relais wieder zurückgeschaltet (normal connected)
     und damit der Netzvorrang aktiv.
  */
  if (sread.length() > 1 || ( millis() - lastReceivedMillis ) > timeout ) {
    if (( millis() - lastReceivedMillis ) > 3000 && sread.length() > 0) { //Verarbeitung hoechstens alle 3 Sekunden
      if (debug) {
        Serial.println(sread);
      }
      evaluate(sread);
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
      debug = true;
      msg = "Switched debug1 to true";
    } else {
      debug = false;
      msg = "Switched debug1 to false";
    }
  } else {
    if (payload[4] == 't') {
      debug2 = true;
      msg = "Switched debug2 to true";
    } else {
      debug2 = false;
      msg = "Switched debug2 to false";
    }    
  }
  if (debug) {
    Serial.println(msg);
  }
  sendClients(msg, false);
}

/**
   Auswertung des vom SBMS120 über Seriell
   empfangenen Datenpakets.
*/
void evaluate(String& sread) {

  if (debug2) Serial.println(sread);

  if (ready) {
    if (debug2) {
      Serial.print("Length: ");
      Serial.println(sread.length());
    }
    if (sread.length() > 0) {
      sendClients(sread, true);
    }
  } else {
    if (!notifiedNoClient) {
      notifiedNoClient = true;
      if (debug) {
        Serial.println("no client connected, yet");
      }
    }
  }

  int len = sread.length();
  const char* txt = sread.c_str();

  String outString = "\nSOC: ";
  if(len>=8) {
    soc = sbms.dcmp(6, 2, txt, len);
    outString += soc;
    outString += " ( Limit: ";
    outString += battery.SOC_LIMIT;
    outString += " ) \n";
  }
  if(len>=24) {
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
  if (debug2) {
    Serial.println(outString);
    Serial.print("StopBattery: ");
    Serial.println(battery.stopBattery);
    Serial.println("_______________________________________");
  }

  if (debug2) {
    String mem = " Heap (free): ";
    mem += ESP.getFreeHeap();
    sendClients(mem , false);
  }

  //Timeoutcounter nur zuruecksetzen, wenn etwas empfangen wurde
  if (sread.length() > 1) {
    lastReceivedMillis = millis();
  }
}

/*
   Pruefen aller Werte
*/
void checkValues()  {
  if (soc < 0) return; //die Main-Loop sollte erstmal Werte lesen

  if(testFixed) {
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
  if (debug) {
    Serial.println("");
    Serial.print("Value evaluation message: ");
    Serial.println(message);
    Serial.print("Stop-value: ");
    Serial.println(stop);
    Serial.print("SOC: ");
    Serial.println(soc);
    for (int j = 0; j < 7; j++) {
      Serial.print("CV[");
      Serial.print(j);
      Serial.print("]: ");
      Serial.println(cv[j]);
    }
    Serial.println("");
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
      if (!battery.stopBattery) {
        if (debug) {
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
    //starteBatterie("Interrupt(BAT); " + message);
    battery.stopBattery = false;
    setGreen();
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

/**
   Netzvorrang starten
*/
void starteNetzvorrang(String reason) {
  String msg = "";
  if (digitalRead(RELAY_PIN) == HIGH) {
    digitalWrite(RELAY_PIN, LOW); //ON, d.h. Netzvorrang aktiv
    sendClients("Toggle battery LOW", false);
    msg += "Starte Netzvorrang :: ";
    msg += reason;
    msg += '\n';
  } else {
    if (debug)  msg = "Kann Netzvorrang nicht starten, da schon aktiv\n";
  }
  if (debug) {
    msg += "Clientcount: ";
    msg += clientCount;
    msg += '\n';
  }
  if (msg.length() > 0) {
    if (debug) {
      Serial.println(msg);
    }
    sendClients(msg, false);
  }
}

/**
   Batteriebetrieb starten
*/
void starteBatterie(String reason) {
  String msg = "";
  if (!battery.stopBattery) {
    if (digitalRead(RELAY_PIN) == LOW) {
      digitalWrite(RELAY_PIN, HIGH); //OFF, d.h. Batterie aktiv
      sendClients("Toggle battery HIGH", false);
      msg += "Starte Netzvorrang :: ";
      msg += reason;
      msg += '\n';
    } else {
      return;
    }
  } else {
    msg = "Kann Netzvorrang nicht stoppen, da Stopflag aktiv\n";
  }
  if (debug) {
    msg += "Clientcount: ";
    msg += clientCount;
    msg += '\n';
  }
  if (msg.length() > 0) {
    if (debug) {
      Serial.println(msg);
    }
    sendClients(msg, false);
  }
}

/**
   Der Button wird über ein in der Setup-Funktion definierten
   Interrupt angebunden:

   attachInterrupt(digitalPinToInterrupt(TASTER), handleButton, RISING);
*/
void IRAM_ATTR handleButton() {
  if (debug) Serial.println("Button pressed");

  if ((millis() - tasterZeit) > entprellZeit) {

    tasterZeit = millis();

    relayStatus = digitalRead(RELAY_PIN);
    if (relayStatus == HIGH) {
      // starte Netzvorrang
      starteNetzvorrang("Buttonaction");
    } else {
      if (!battery.stopBattery) {
        starteBatterie("Buttonaction");
      } else {
        if (debug) {
          Serial.println("ON, kann Netzvorrang nicht abschalten (Stop wegen SOC oder Low Voltage)");
        }
      }
    }

  }
}

/**********************************************************************/
/*                                                                    */
/* Setup                                                              */
/*       - ´                          */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
void setup() {

  Serial.begin(115200);  //USB
  Serial1.begin(9600, SERIAL_8N1, 16, 17); //Serial1 Pins 4,2, Serial2 Pins 16,17

  delay(1000);

  Serial.println("Starting...");

  //Pins fuer Taster und Relay initialisieren
  pinMode(TASTER, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  //Leds
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  pinMode(LED_S1, OUTPUT);
  //digitalWrite(LED_S1, HIGH);

  pinMode(LED_S2, OUTPUT);
  //digitalWrite(LED_S2, HIGH);
  

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  

  //Beim Laden BLAU zeigen
  setBlue();

  //Button-Handlermethode anbinden
  //Guru Meditation Error: Core  1 panic'ed (Cache disabled but cached memory region accessed)
  //attachInterrupt(digitalPinToInterrupt(TASTER), handleButton, RISING);

  // etabliere Wifi Verbindung
  myWifi.connect();

  // start WebsocketServer server
  wsServer.onEvent(webSocketEvent);
  wsServer.begin();

  // start Webserver
  server.on("/", sbmsPage);
  server.on("/sbms", sbmsPage);
  server.begin();

  // initialize other the air updates
  ota.init(server, "esp32b");
}

/**********************************************************************/
/*                                                                    */
/*                 Loop                                               */
/*                                                                    */
/**********************************************************************/
void loop() {
  wsServer.loop();
  server.handleClient();
  readSbms();
  yield();
  if (( millis() - lastCheckedMillis ) > 3000) { //Pruefung hoechstens alle 3 Sekunden
      Serial.println("Check...");
      lastCheckedMillis = millis();
      checkValues();
  }
}
