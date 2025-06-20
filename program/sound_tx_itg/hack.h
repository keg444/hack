#ifndef HACK_H
#define HACK_H

#include <Arduino.h>

const int dataPins[4];
const int threshold;

int detectBitByDuration(int durationMs);
void syncTone(int pin, unsigned int  freqency, unsigned long delay, unsigned long GI);

#endif