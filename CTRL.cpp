#include "global.h" 


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
  Serial.println(luefter.isOn());
  Serial.print(F("Temperatur: "));
  Serial.println(temp);  
  Serial.print(F("Now (millis): "));
  Serial.println(millis());        
  Serial.print(F("Udp resets: "));
  Serial.println(udpResets);  
  Serial.print(F("Last status message: "));
  Serial.println(lastStatusMsg );
  Serial.print(F("Free Heap: "));
  Serial.println(ESP.getFreeHeap());  
}
