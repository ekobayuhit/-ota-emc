#ifndef RELAY_H
#define RELAY_H
#include <Arduino.h>

extern const int relayPin;
extern bool state_load;

extern void ReadLoadState();
extern void TurnONLoad();
extern void TurnOFFLoad();

#endif
