#ifndef SDCARD_H
#define SDCARD_H
#include <Arduino.h>

const int chipSelect = 15;//D8-HCS
extern bool initsdcard;

extern void SD_setup();
extern void SD_write(String namedata, String value);
extern void SD_delete(String namedata);

#endif
