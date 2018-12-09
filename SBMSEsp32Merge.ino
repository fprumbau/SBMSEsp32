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

//nicht auf Serial1 warten, Feste Werte annehmen
bool testFixed = false;
const char* hostName = "esp32a";

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
 * Neu fuer Async
 */
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

/**
 * Kommandos von Serial lesen, wie z.B.
 * Neustart von WiFi
 */
void commandLine();

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

  //Stattdessen mit Async
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
  taster.check(); //AceButton
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
  yield();
  commandLine();
  
  //Restart erforderlich
  if(updater.restartRequired) {
    Serial.println("Updater requests restart, doing so now...");
    delay(2000);
    ESP.restart();
  }
}

/**********************************************************************/
/*                                                                    */
/*                 Functions                                          */
/*                                                                    */
/**********************************************************************/

void readSbms() {

  String sread;

  if (testFixed) {
    //sread = "5+'/,D$+HNGpGtGuGkH9H5HD+J##-#$'#####&##################$|(";
    //sread = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
    //sread = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
    //sread = "#$7%XS$*GOGRGTGPGOGRGOGP*]##-##9##E#####################%N(";
    sread = "#$87%K$*GDGGGPGDG2GLGLGL*m##-##:##@#####################%N(";    
  } else {
    while (Serial1.available()) {
      sread = Serial1.readString();
    }
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
      if (debug && len > 0) {
        Serial.print(".____");
        Serial.print(sread);
        Serial.println("____.");
        Serial.print("Length 'sread': ");
        Serial.println(len);
      };

      //Wert soc zurücksetzen (Wichtig, wenn mehrere Male nichts gelesen wird, also sread.length=0,dann muss erst der failureCount
      // hochgehen und nachher und schliesslich der Fehlermodus aktiviert werden (Batteriesperre)
      soc = 0;

      //Werte nun lesen
      if (len > 0) {
        //Wert zu Clients publishen (wird dort in Webseite visualisiert oder gelisted)
        wc.sendClients(sread, true);

        const char* txt = sread.c_str();

        String outString = "\nSOC: ";
        if (len >= 8) {
          soc = sbms.dcmp(6, 2, txt, len);
          outString += soc;
          outString += " ( Limit: ";
          outString += inverter.SOC_LIMIT;
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
        if (debug) {
          Serial.println(outString);
          Serial.print("StopBattery: ");
          Serial.println(inverter.stopBattery);
          Serial.println("_______________________________________");
        }

        if (debug) {
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
  if (debug2) {
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
      inverter.stopBattery = true;
      starteNetzvorrang("Interrupt(NZV); " + message);
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

/**
   Netzvorrang starten
*/
void starteNetzvorrang(String reason) {
  String msg = "";
  if (digitalRead(RELAY_PIN) == HIGH) {
    digitalWrite(RELAY_PIN, LOW); //ON, d.h. Netzvorrang aktiv
    wc.sendClients("Toggle battery LOW", false);
    msg += "Starte Netzvorrang :: ";
    msg += reason;
    msg += '\n';
  } else {
    if (debug)  msg = "Kann Netzvorrang nicht starten, da schon aktiv\n";
  }
  if (msg.length() > 0) {
    if (debug) {
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
  if (!inverter.stopBattery) {
    if (digitalRead(RELAY_PIN) == LOW) {
      digitalWrite(RELAY_PIN, HIGH); //OFF, d.h. Batterie aktiv
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
  if (msg.length() > 0) {
    if (debug) {
      Serial.println(msg);
    }
    wc.sendClients(msg, false);
  }
}

void handleButton(AceButton* /* button */, uint8_t eventType, uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("Button pressed");

      bool relayStatus = digitalRead(RELAY_PIN);
      if (relayStatus == HIGH) {
        // starte Netzvorrang
        starteNetzvorrang("Buttonaction");
      } else {
        if (!inverter.stopBattery) {
          starteBatterie("Buttonaction");
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

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){

  switch (type) {
    case WS_EVT_CONNECT: {       
       
          client->text("@ Connected");
         
          //Aktualisieren von debug/debug1/s1/s2/netz bzw batt
          wc.updateUi(client, false);             
          break;
      }    
      case WS_EVT_DATA:

      if (data[0] == '@') {
          if (data[1] == '+') {
            starteBatterie("Websockets");
          } else if (data[1] == '-') {
            starteNetzvorrang("Websockets");
          } else if (data[1] == 's' && len > 3) {
            //Solar charger s1 an / ausschalten
            if (data[2] == '1') {
              if (data[3] == '+') {
                //s1 anschalten
                sma.toggleCharger(1, true, true);
                wc.sendClients("s1 an", false);
              } else {
                //s1 abschalten
                sma.toggleCharger(1, false, true);
                wc.sendClients("s1 aus", false);
              }
            } else {
              if (data[3] == '+') {
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
          if (data[1] == 'd' && len > 4) { 
            toggleDebug(data);
          }
      }
      break;
    }
}
