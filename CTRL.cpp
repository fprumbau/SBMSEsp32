#include "global.h" 

bool CTRL::fansRunning() {
  return !digitalRead(RELAY_4);
}

void CTRL::fansOn() {
  digitalWrite(RELAY_4, LOW);
}

void CTRL::fansOff() {
  digitalWrite(RELAY_4, HIGH);
}

bool CTRL::isUpForSeconds(int seconds) {
  long sinceMillis = seconds * 1000;
  long diff = millis() - sinceMillis;
  return  diff > 0;
}

void CTRL::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("Running since: "));
  Serial.println(runningSince);
  Serial.print(F("Lüfter aktiv: "));
  Serial.println(fansRunning());
  Serial.print(F("Temperatur: "));
  Serial.println(temp);  
  Serial.print(F("Now (millis): "));
  Serial.println(millis());        
  Serial.print(F("Udp resets: "));
  Serial.println(udpResets);  
  Serial.print(F("Wifi reconnects: "));
  Serial.println(myWifi.wifiReconnects);
  Serial.print(F("Wifi IP: "));
  Serial.println(myWifi.localIP());
  Serial.print(F("Last status message: "));
  Serial.println(lastStatusMsg );
  Serial.print(F("Free Heap: "));
  Serial.println(ESP.getFreeHeap());
  Serial.print(F("\nLoopanalyzer steht auf: "));
  Serial.println(loopAnalyzer);
  Serial.print(F("SbmsAnalyzer steht auf: "));
  Serial.println(sbms.sbmsAnalyzer);  
  Serial.print(F("MyWifi.connected() :: "));
  Serial.println(myWifi.connected());  
}
