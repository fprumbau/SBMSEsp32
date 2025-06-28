#include "SBMS.h"

#include "global.h"
#include <Arduino.h>

const long pwrs[] = { 1, 91, 8281, 753571 }; // 91^0, 91^1, 91^2, 91^3

const unsigned char dec[91] = {
  '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<',
  '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
  'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
  'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`', 'a', 'b', 'c',
  'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
  'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}'
};

/**
   Dekodierung der vom SBMS120 kommenden Werte
*/
long SBMS::dcmp(int loc, int size,  const char* sbms, int sbmsLen) {
  if (loc > sbmsLen) {
    return 0;
  }
  long rv = 0;
  for (int i = 0; i < size; i++) {
    int ind = loc + size - 1 - i;
    char c = sbms[ind];
    int offc = SBMS::char_off(c);
    rv += ( offc * pwrs[i] );
  }
  return rv;
}

unsigned int SBMS::char_off(char c) {
  for (int i = 0; i < 91; i++) {
    if (dec[i] == c) {
      return i;
    }

  }
  return 0;
}

bool SBMS::read() {

  unsigned long now = millis();

  if((now -lastDisplayUpdate) > 300) {
      display.update();
      lastDisplayUpdate = now;
  }

  /**
     Solange etwas empfangen wird (data gefuellt) sollte ausgewertet werden.
     Wenn aber der Timeout zuschlaegt, dann fuehrt das Lesen der nicht empfangenen
     Werte, dazu, soc und battery.cv[] zurueckzusetzen, woraufhin der naechste Lauf der
     Interruptmethode isrHandler(..) dazu, dass die Status-LED auf rot schaltet.
     Gleichzeitig ist es nicht mehr möglich, auf Batterie zu wechseln.

     Ist die Batterie gerade aktiv, wird das Relais wieder zurückgeschaltet (normal connected)
  */
  if (( now - lastReceivedMillis ) > 4000 && ( now - lastChecked ) > 2000) { //Verarbeitung hoechstens alle 4 Sekunden, Versuch nur alle 2 Sekunden ( SBMS aktualisiert nun alle 5s )   

    lastChecked = now;

    int len;
    if (testFixed) {
      //data = "5+'/,D$+HNGpGtGuGkH9H5HD+J##-#$'#####&##################$|(";
      //data = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
      //data = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
      //data = "#$7%XS$*GOGRGTGPGOGRGOGP*]##-##9##E#####################%N(";
      //data = "#$87%K$*GDGGGPGDG2GLGLGL*m##-##:##@#####################%N(";
      
      //strncpy(dataBuffer, testData, SBMS_DATA_BUFFER_SIZE);
      //dataBuffer[SBMS_DATA_BUFFER_SIZE - 1] = '\0';
      //len = strlen(dataBuffer);
    } else {

      int index = 0;
      const int STRING_BUFFER_SIZE = 70; //Max. Laenge der Pakete oben ist 61
      char stringBuffer[STRING_BUFFER_SIZE];
      memset(stringBuffer, 0, STRING_BUFFER_SIZE); // Initialisiere mit \0

      // Überspringe ungültige Daten bis zum nächsten \0
      if (serialSBMS.available() > 61) {
        while (serialSBMS.available() > 0) {
          char c = serialSBMS.read();
          if (c == '\0') break;
          yield(); // Scheduler freigeben
        }
      }

      // Lese Daten in stringBuffer
      while (serialSBMS.available() > 0) {
        char c = serialSBMS.read();
        if (c >= 32) {
          stringBuffer[index++] = c;
        } else {
          stringBuffer[index] = '\0'; // Terminiere String
          if (debugSbms) {
            Serial.print(F("Empfangenes String-Endezeichen / aktueller Index: "));
            Serial.print((int)c);
            Serial.print(F(" / "));
            Serial.println(index);
          }
          break;
        }
        yield(); // Scheduler freigeben
      }

      stringBuffer[index] = '\0'; // Sicherstellen, dass String terminiert ist
      if (index == 0) {
        if (debugSbms) 
          Serial.println("Keine Daten von serialSBMS empfangen");
        return false;
      }

      strncpy(dataBuffer, stringBuffer, SBMS_DATA_BUFFER_SIZE);
      dataBuffer[SBMS_DATA_BUFFER_SIZE - 1] = '\0';
      len = strlen(dataBuffer);

    }

    if (len == 0) {
      return false;
    }

    yield();

    if (debugSbms && len > 0) {
      Serial.print(">");
      Serial.print(dataBuffer);
      Serial.println("<");
      Serial.print("Length 'data': ");
      Serial.println(len);
    };

    if (len < 50) {
      if (debugSbms) {
        String errData = String((char*)0);
        errData.reserve(100);
        errData += "Fehler, SBMS-Daten zu kurz: (len: ";
        errData += len;
        errData += ");";
        Serial.println(errData);
        wc.sendClients(errData.c_str());
      }
      inverter.setBlue();
      return false;
    }
    inverter.setGreen();

    //Wert soc zurücksetzen (Wichtig, wenn mehrere Male nichts gelesen wird, also data.length=0,dann muss erst 
    //                       der failureCount hochgehen und nachher und schliesslich der Fehlermodus aktiviert werden (Batteriesperre)
    //v.1.0.15 in SBMS.cpp wurd bei jedem SBMS-Datenpaket erst der soc auf 0 gestellt. Dadurch ist oft unn&ouml;tig auf Netzbetrieb umgeschaltet worden (ab 10Uhr)
    //battery.soc = 0;

    //Werte nun ans Webfrontend schicken
    wc.updateUi(); //ab 0.9.9.22 wird data per JSon uebermittelt

    const char* txt = dataBuffer;

    if (debugSbms) {
      char outString[100];
      snprintf(outString, sizeof(outString), "\nSOC: ");
      if (len >= 8) {
        battery.soc = dcmp(6, 2, txt, len);
        snprintf(outString + strlen(outString), sizeof(outString) - strlen(outString),
                 "%ld(Limit:%ld); ", battery.soc, battery.socLimit);
      }
      if (len >= 24) {
        for (int k = 0; k < 8; k++) {
          int loc = k * 2 + 8;
          battery.cv[k] = dcmp(loc, 2, txt, len);
          snprintf(outString + strlen(outString), sizeof(outString) - strlen(outString),
                   "cv%d[%ld], ", k + 1, battery.cv[k]);
        }
      }
      if (len >= 26) {
        temp = (dcmp(24, 2, txt, len) / 10) - 45;
      }
      Serial.print(outString);
      Serial.print(F("; StopBattery: "));
      Serial.println(inverter.stopBattery);
    } else {
      if (len >= 8) {
        readSoc(txt, len);
      }
      if (len >= 24) {
        for (int k = 0; k < 8; k++) {
          int loc = k * 2 + 8;
          battery.cv[k] = dcmp(loc, 2, txt, len);
        }
      }
      if (len >= 26) {
        temp = (dcmp(24, 2, txt, len) / 10) - 45;
      }
    }
    yield();

    //Timeoutcounter nur zuruecksetzen, wenn etwas empfangen wurde
    lastReceivedMillis = millis();
    return true;
  }

  return false;
}

/**
 * 
 */
void SBMS::readSoc(const char* txt, int len) {
  int val = sbms.dcmp(6, 2, txt, len);
  if (val > 0 && val <= 100 ) {
    if (battery.soc <= 0) {
      battery.soc = val;  
    } else {
      if (std::abs(val - battery.soc) < 10) {
        battery.soc = val;         
      } else {
        Serial.println(F("SBMS: Ignoriere den Wert von SoC, weil Abweichung zu gross; alt: "));
        Serial.print(battery.soc); 
        Serial.print(F(" ; neu: "));
        Serial.println(val);          
      }
    }
  }
}

void SBMS::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("SBMS.lastReceivedMillis (in Sekunden): "));
  Serial.println(utils.secondsSince(lastReceivedMillis));
  Serial.print(F("SBMS.testFixed: "));
  Serial.println(testFixed);
}

SBMS sbms;
