#ifndef TASTER_H
#define TASTER_H

volatile int interruptCounter = 0;
int numberOfInterrupts = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

volatile int TasterStatus = 0;
const byte TASTER = 19;
unsigned long tasterZeit = 0;
unsigned long entprellZeit = 500;

#endif
