#include "global.h"

#include "MyWifi.h"
#include "WebCom.h"
#include "wechselrichter.h"
#include "ESP32OTA.h"

#include <WiFiUdp.h>

//findet die Checkmethode falsche Werte vor, so wird noch einmal
//(4s) gewartet, bevor diese tatsächlich zu einem Fehler führen.
int failureCount = 0;
const int errLimit = 5;   
const int smaMeasurementsBeforSwitchoff = 10;
int SOC_LIMIT = 30; //wird aus Config ueberschrieben
int SOC_HYST = 5; //5% Hysterese
int LOW_MINIMAL_CV_MILLIS = 2700; //darunter wird Charger S2 für 5Min aktiviert
int LOW_VOLTAGE_MILLIS = 3100; //darunter wird die Batterie abgeschaltet  
int CV_HYST = 50; //Anschalten der Batterie erst moeglich, wenn CV jeder Zelle > LOW_VOLTAGE_MILLIS + CV_HYST ist

const int checkMillis = 10000; //Pruefung der Kennwerte (SOC, Zellspannungen), minimales Interval in Millisekunden

int LED_RED = 12;
int LED_GREEN = 14;
int LED_BLUE = 27;    

bool debug = false;
bool debug2 = false;
int RELAY_S1 = 33;
int RELAY_S2 = 32;    

//Achtung IO34 und IO35 haben keine Outputdriver, sie sind nur als Input zu gebrauchen!!!
int RELAY_3 = 13; //Schalten von S1 (Remote), v.0.9.9.47 (R3 ist das einzige auf NC laufende Relais, d.h. Shorted == HLG600 ist aus)
int RELAY_4 = 23; //Schalten des Luefters, v.0.9.9.40

int RELAY_PIN = 21;    
int LED_S1 = 25;
int LED_S2 = 26;
int TASTER = 19; //manuelle Inverterumschaltung

//Vorbereitung Board rev.2.8
int PWM_L1 = 25; //Luefter Charger GPIO25
int PWM_L2 = 26; //Luefter Inverter GPIO26
int PWM_S2 = 5; //HLG600-30B, GPIO05
//zug.Channels
int GPIO25 = 0;
int GPIO26 = 1;
int GPIO05 = 2;

Battery battery;
Charger charger;
MyWifi myWifi;
SMA sma;
WebCom wc;
Inverter inverter;
AsyncWebServer server(80);
ESP32OTA updater;
SBMS sbms;
WiFiUDP udp;
WiFiUDP ntpUdp;
NTPClient timeClient(ntpUdp);
long lastUdpRead = -1;
long lastUdpNotification = -1;
AsyncWebSocket ws("/ws");

long soc = -1; //aktueller Wert State Of Charge
int cv[8]; //aktuelle Zellspannungen
float temp = 0.0; //aktuelle Temperatur des SBMS120

//nicht auf Serial1 warten, Feste Werte annehmen
bool testFixed = false;
String testData = "#$87%K$*GDGGGPGDG2GLGLGL*m##-##:##@#####################%N(";
const char* hostName = "esp32a";
unsigned long lastReceivedMillis = -1;
long timeout = 10000;

String sbmsData="";
float bezug=0.0;
float lieferung=0.0;
String datetime;

int dayOfMonthLastRestart = -1; //Wann wurde der ESP zum letzten Mal neu gestartet?
bool s2ActForLowCV = false; //Notladungsflag fuer S2, falls Zellspannungen unter LOW_MINIMAL_CV_MILLIS abfallen
long s1MinRestMillis = 60000; //0.9.9.53
long s2MinRestMillis = 30000; //0.9.9.53

int dutyCycle = 0; //0.9.9.60
float lastNettoAbzug = 0; //0.9.9.60
long checkOnIncomeMinIntervalMillis = 0;
long CHECK_INCOME_MIN_INTERVAL_MILLIS = 10000; //0.9.9.62
bool enableCountBeforeOff = false; //0.9.9.64 Die Charger werden nun direkt geschaltet, nicht erst nach n-Messungen (siehe 0.9.9.44)
