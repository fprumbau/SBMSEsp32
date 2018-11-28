#ifndef TASTER_H
#define TASTER_H

volatile unsigned long tasterZeit = 0;
volatile int interruptCounter = 0;

int numberOfInterrupts = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

volatile int TasterStatus = 0;
int TASTER = 19;
unsigned long entprellZeit = 500;

#endif
