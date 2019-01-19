#ifndef GLOBAL_H
#define GLOBAL_H

#include "ESPAsyncWebServer.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "MyWifi.h"
#include "WebCom.h"
#include "wechselrichter.h"
#include "ESP32OTA.h"
#include "SBMS.h"
#include "SMA.h"
#include "battery.h"
#include "charger.h"

//findet die Checkmethode falsche Werte vor, so wird noch einmal
//(4s) gewartet, bevor diese tatsächlich zu einem Fehler führen.
extern int SOC_LIMIT; 
extern int SOC_HYST;
extern int LOW_VOLTAGE_MILLIS; 
extern int CV_HYST;
extern int LOW_MINIMAL_CV_MILLIS;
extern int failureCount;
extern const int errLimit;   
extern const int smaMeasurementsBeforSwitchoff;
extern const int checkMillis;
 
extern int LED_RED;
extern int LED_GREEN;
extern int LED_BLUE;    

extern bool debug;
extern bool debug2;
extern int RELAY_S1;
extern int RELAY_S2; 
extern int RELAY_3;
extern int RELAY_4;   
extern int RELAY_PIN;
extern int LED_S1;
extern int LED_S2;
extern int TASTER; //manuelle Inverterumschaltung

extern Battery battery;
extern Charger charger;
extern Inverter inverter;
extern MyWifi myWifi;
extern SMA sma;  //read SMA energy meter broadcast messages
extern WebCom wc;
extern AsyncWebServer server;   
extern AsyncWebSocket ws;
extern ESP32OTA updater; //Over-the-air updater
extern WiFiUDP udp;
extern WiFiUDP ntpUdp;
extern NTPClient timeClient;
extern SBMS sbms;//SBMS solar battery charger functions, uncompress etc.
extern long lastUdpRead; //letztes, erfolgreiches lesen eines UDP-Paketes (WiFi isAlive)
extern long lastUdpNotification; //verhindert Stackoverflow, senden von zu vielen Websocketpaketen 0.9.9.24

extern long soc; //aktueller Wert State Of Charge
extern int cv[]; //aktuelle Zellspannungen
extern float temp; //SBMS Temperatur

//nicht auf Serial1 warten, Feste Werte annehmen
extern bool testFixed;
extern const char* hostName;
extern unsigned long lastReceivedMillis;
extern long timeout; //Empfangstimeout ( wird 10s nichts empfangen, muss die Batterie abgeschaltet werden )

extern String sbmsData;
extern float bezug;
extern float lieferung;
extern String datetime;

extern int lastHourRestart; //Wann wurde der ESP zum letzten Mal neu gestartet?
extern bool s2ActForLowCV;  //0.9.9.46

#endif
