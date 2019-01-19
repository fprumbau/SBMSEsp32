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

const int checkMillis = 3000;

int LED_RED = 12;
int LED_GREEN = 14;
int LED_BLUE = 27;    

bool debug = false;
bool debug2 = false;
int RELAY_S1 = 33;
int RELAY_S2 = 32;    

//Achtung IO34 und IO35 haben keine Outputdriver, sie sind nur als Input zu gebrauchen!!!
int RELAY_3 = 13; //Frei
int RELAY_4 = 23; //Schalten des Luefters, v.0.9.9.40

int RELAY_PIN = 21;    
int LED_S1 = 25;
int LED_S2 = 26;
int TASTER = 19; //manuelle Inverterumschaltung

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
const char* hostName = "esp32a";
unsigned long lastReceivedMillis = -1;
long timeout = 10000;

String sbmsData="";
float bezug=0.0;
float lieferung=0.0;
String datetime;

int lastHourRestart = -1; //Wann wurde der ESP zum letzten Mal neu gestartet?
bool s2ActForLowCV = false; //Notladungsflag fuer S2, falls Zellspannungen unter LOW_MINIMAL_CV_MILLIS abfallen
