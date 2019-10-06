#ifndef GLOBAL_H
#define GLOBAL_H

#include <ESPAsyncWebServer.h>
#include <esp32-hal-ledc.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SPIFFS.h>
#include <FS.h>

#include "MyWifi.h"
#include "WebCom.h"
#include "Inverter.h"
#include "OTA.h"
#include "SBMS.h"
#include "SMA.h"
#include "battery.h"
#include "charger.h"
#include "Utils.h"
#include "Log.h"

#include "Tesla.h"
#include "CFG.h"
#include "CTRL.h"

//findet die Checkmethode falsche Werte vor, so wird noch einmal
//(4s) gewartet, bevor diese tatsächlich zu einem Fehler führen.
extern int socLimit; 
extern int SOC_HYST;
extern int LOW_VOLTAGE_MILLIS; 
extern int CV_HYST;
extern int LOW_MINIMAL_CV_MILLIS;   
extern const int smaMeasurementsBeforSwitchoff;
extern const int checkMillis;
 
extern int LED_RED;
extern int LED_GREEN;
extern int LED_BLUE;    

extern bool debug;
extern bool debugWeb;
extern bool debugJson;
extern bool debugRelais;
extern bool debugSma;
extern bool debugSbms;
extern bool debugCharger;
extern bool debugBattery;
extern bool debugInverter;
extern bool debugConfig;
extern bool debugTesla;
extern String bitset;
extern bool fastResponse; //0.9.9.98 alle Ertragsnachrichten sofort an die Clients senden

extern int RELAY_S1;
extern int RELAY_S2; 
extern int RELAY_3;
extern int RELAY_4;   
extern int RELAY_PIN;
extern int LED_S1;
extern int LED_S2;
extern int TASTER; //manuelle Inverterumschaltung
extern int PWM_L1; //Luefter Charger
extern int PWM_L2; //Luefter Inverter
extern int PWM_S2; //HLG600-30B
extern int GPIO25; //Channel Lueft. Charger
extern int GPIO26; //Channel Lueferter Inverter
extern int GPIO05; //Channel HLG600-30B

extern Battery battery;
extern Charger charger;
extern Inverter inverter;
extern MyWifi myWifi;
extern SMA sma;  //read SMA energy meter broadcast messages 
extern WebCom wc;
extern AsyncWebServer server;   
extern AsyncWebSocket ws;
extern OTA updater; //Over-the-air updater
extern AsyncUDP udp;
extern WiFiUDP ntpUdp;
extern NTPClient timeClient;
extern SBMS sbms;//SBMS solar battery charger functions, uncompress etc.
extern Utils utils; 
extern int udpResets; //0.9.9.92 Zaehlen von Resets
extern int wifiReconnects; //0.9.9.92 Zaehlen von Wifi Reconnects

extern long soc; //aktueller Wert State Of Charge
extern float temp; //SBMS Temperatur
extern int TEMP_THRESHOLD_HIGH; //Wird diese Temperatur ueberschritten, werden die Luefter aktiv
extern int TEMP_THRESHOLD_LOW; //Wird diese Temperatur unterschritten (und laeuft nichts mehr), dann werden die Luefter abgeschaltet

//nicht auf serialSBMS warten, Feste Werte annehmen
extern bool testFixed;
extern String testData;
extern const char* hostName;
extern unsigned long lastReceivedMillis;

extern float netto;
extern String datetime;

extern int dayOfMonthLastRestart; //Wann wurde der ESP zum letzten Mal neu gestartet?
extern bool s2ActForLowCV;  //0.9.9.46
extern long s1MinRestMillis; //0.9.9.53
extern long s2MinRestMillis; //0.9.9.53

extern int dutyCycle; //0.9.9.60
extern bool enableCountBeforeOff; //0.9.9.64 Die Charger werden nun direkt geschaltet, nicht erst nach n-Messungen (siehe 0.9.9.44)

extern String TESLA_CLIENT_ID;
extern String TESLA_CLIENT_SECRET;
extern boolean teslaCtrlActive;
extern Tesla perry;
extern CFG config;
extern long lastTeslaLoadSwitch;
extern long TESLA_LOAD_SWITCH_MIN_MILLIS; //hoechstens alle 5 Minuten schalten
extern bool teslaCharging;
extern int teslaChargeStartThreshold; //0.9.9.80
extern int teslaChargeStopThreshold; //0.9.9.80
extern int lowIncomeCount; //0.9.9.80
extern int highIncomeCount; //0.9.9.80
extern String lastStatusMsg; //0.9.9.93
extern String runningSince; //0.9.9.93
extern int loopAnalyzer; //0.9.9.94
extern HardwareSerial serialSBMS; //0.9.9.95
extern CTRL controller; //0.9.9.96
extern unsigned long lastConnectCheck; //0.9.9.99
extern LOG logs; //0.9.9.99
#endif
