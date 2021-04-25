#include "ZMPT101B.h"

ZMPT101B::ZMPT101B(uint8_t _pin) {
  pin = _pin;
  sensitivity = 0.003835;
}

int ZMPT101B::calibrate() {
  uint32_t acc = 0;
  for (int i = 0; i < 100; i++) {
    acc += analogRead(pin);
  }
  zero = acc / 100;
  Serial.print(F("Calibated zero point: "));
  Serial.println(zero);
  return zero;
}

void ZMPT101B::setSensitivity(float sens) {
  sensitivity = sens;
}

float ZMPT101B::getSensitivity() {
  return sensitivity;
}

float ZMPT101B::getVoltageDC() {
  int16_t acc = 0;
  for (int i = 0; i < 10; i++) {
    acc += analogRead(pin) - zero;
  }
  float V = (float)acc / 10.0 / ADC_SCALE * VREF / sensitivity;
  return V;
}

float ZMPT101B::getVoltageAC(uint16_t frequency) {
  uint32_t period = 1000000 / frequency;
  uint32_t t_start = micros();

  uint32_t Vsum = 0, measurements_count = 0;
  int32_t Vnow;

 //Serial.println(String("\nZero: ") + zero);
 //Serial.println("Values: ");
 while (micros() - t_start < period) {
    Vnow = analogRead(pin) - zero;

    //Serial.print(String(",") + Vnow);
    
    Vsum += Vnow*Vnow;
    measurements_count++;
  }
  
  currentAc = sqrt(Vsum / measurements_count) / ADC_SCALE * VREF / sensitivity;
  return currentAc;
}

bool ZMPT101B::hasNetzspannung() {
  return currentAc > 200.0;
}
