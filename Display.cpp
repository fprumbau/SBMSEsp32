#include "global.h" 

void Display::init() {
  Wire.begin();
  lcd.begin(20, 4);
  //lcd.backlight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 

  String s = screen;
  Serial.println(s);

  offsets[0]=0;
  offsets[1]=0;
  offsets[2]=0;
}

void Display::update() {

    long now = millis();
    if((now -lastVoltUpdate) > 5000) {
        //v.3.0.0 Netzspannung pruefen (wenn Ausfall, startet Inverter selbst, die Luefter aber nicht)
        float U = voltageSensor.getVoltageAC();
        String v = String("AC: ") + U + "V SOC:" + battery.soc + "%      ";
        v=v.substring(0,20);
        line2buff(v.c_str(), 0);
        lastVoltUpdate = now;
    }

    prepareLine(0, 40); //Die Displayzeilen sind so geordnet: 1-3-2-4
    prepareLine(1, 20);
    prepareLine(2, 60);

    draw();
}
 
void Display::prepareLine(int ln, int offset) {

    String line = logs.get(ln);  
    if(line.length() > 0) {
           
      int len = line.length();
      int start = offsets[ln];
          
      if(len > start+20) {
        //den Anfangswert (noscroll) für SHOW_MSG_START_TICKS beibehalten
        int remain = onStart[ln];
        if(remain < SHOW_MSG_START_TICKS) {
          onStart[ln]++;
        } else {
          line = line.substring(start, start+20);
          offsets[ln]=start+1;
        }            
      } else {
        offsets[ln]=0;
        onStart[ln]=0;
      }
      line2buff(line.c_str(), offset);          
    }
}

void Display::line2buff(const char* txt, int offset) {

  for(int i=offset; i<offset+20; i++) {
    char c = txt[i-offset];
    //Serial.println(c);  
    if(c == '\0') break;
    screen[i]=c;  
  }
  
}

void Display::draw() {
  lcd.setCursor(0, 0);
  String s = screen;
  lcd.print(s);
}

void Display::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("Displaybuffer: "));
  String s = screen;
  Serial.println(s);
}

bool isI2CDevicePresent(uint8_t address) {
  Wire.beginTransmission(address);
  uint8_t error = Wire.endTransmission();
  return (error == 0); // true, wenn Gerät antwortet
}