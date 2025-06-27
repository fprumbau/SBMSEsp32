#include "global.h"

#include "MyWifi.h"
#include "WebCom.h"
#include "Inverter.h"
#include "OTA.h"
#include "Tesla.h"

#include <WiFiUdp.h>

const int smaMeasurementsBeforSwitchoff = 10;
const int tenSeconds = 10000; //Pruefung der Kennwerte (SOC, Zellspannungen), minimales Interval in Millisekunden

int LED_RED = 12;
int LED_GREEN = 14;
int LED_BLUE = 27;    

bool debug = false;
bool debugWeb = false;
bool debugJson = false;
bool debugRelais = false;
bool debugSma = false;
bool debugSbms = false;
bool debugCharger = false;
bool debugBattery = false;
bool debugInverter = false;
bool debugConfig = false;
bool debugTesla = false;
bool fastResponse = false; //0.9.9.98 alle Ertragsnachrichten sofort an die Clients senden

//v. 1.0.13: bitset[13]: charger.automatedCharging == true 
String bitset = "0000000000000100"; 

//09.04.22 S2 Relais (war: 32) defekt, Tausch mit S1
int RELAY_S1 = 32;
int RELAY_S2 = 33;    

//Achtung IO34 und IO35 haben keine Outputdriver, sie sind nur als Input zu gebrauchen!!!
int RELAY_3 = 13; //Schalten von S1 (Remote), v.0.9.9.47 (R3 ist das einzige auf NC laufende Relais, d.h. Shorted == HLG600 ist aus)
int RELAY_4 = 23; //Schalten des Luefters, v.0.9.9.40

int RELAY_PIN = 0; //war bis v.1.0.18 auf GPIO21    
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

int t1 = 18; //batt zeit start
int t2 = 9;  //batt zeit stop

Luefter luefter;
Battery battery;
Charger charger;
MyWifi myWifi; 
SMA sma;
WebCom wc;
Inverter inverter;
AsyncWebServer server(80);
OTA updater;
//SBMS sbms;
Utils utils;
AsyncUDP udp;
WiFiUDP ntpUdp;
NTPClient timeClient(ntpUdp);
int udpResets = 0;

AsyncWebSocket ws("/ws");

float temp = 0.0; //aktuelle Temperatur des SBMS120
int TEMP_THRESHOLD_HIGH = 33; //Wird diese Temperatur ueberschritten, werden die Luefter aktiv
int TEMP_THRESHOLD_LOW = 30; //Wird diese Temperatur unterschritten (und laeuft nichts mehr), dann werden die Luefter abgeschaltet

//nicht auf serialSBMS warten, Feste Werte annehmen
bool testFixed = false;
String testData = F("#$$(I?#\\GOGSGSGQGQGPGPGR+<##+#_:%[6#.4%(@###6Of6O_######%N(");
const char* hostName = "esp32a";
unsigned long lastReceivedMillis = 0;

float netto=0.0;
String datetime;

int dayOfMonthLastRestart = -1; //Wann wurde der ESP zum letzten Mal neu gestartet?
bool s2ActForLowCV = false; //Notladungsflag fuer S2, falls Zellspannungen unter LOW_MINIMAL_CV_MILLIS abfallen

String TESLA_CLIENT_ID = F("81527cff06843c8634fdc09e8ac0abefb46ac849f38fe1e431c2ef2106796384");
String TESLA_CLIENT_SECRET = F("c7257eb71a564034f9419ee651c7d0e5f7aa6bfbd18bafb5c5c033b093bb2fa3");
Tesla perry;
boolean teslaCtrlActive = false;
long lastTeslaLoadSwitch = -1;
long TESLA_LOAD_SWITCH_MIN_MILLIS = 300000; //hoechstens alle 5 Minuten schalten
bool teslaCharging = false;
int teslaChargeStartThreshold = 500;
int teslaChargeStopThreshold = -1000;
int lowIncomeCount = 0; //Der Tesla sollte erst abgeschaltet werden, nachdem 10 Messpunkte 
int highIncomeCount = 0;
CFG config;
String lastStatusMsg = String((char*)0);
String runningSince = String((char*)0);
HardwareSerial serialSBMS(1); //0.9.9.95, uart 1
CTRL controller; //0.9.9.96
unsigned long lastConnectCheck = 0; //0.9.9.99
LOG logs; //0.9.9.99
LiquidCrystal_I2C lcd(0x3F, 20, 4); //3.0.1
ZMPT101B voltageSensor(34); //3.0.1, Analog read auf 34
Display display; //3.0.1
