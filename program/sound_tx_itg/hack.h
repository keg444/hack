#ifndef HACK_H
#define HACK_H

#include <Arduino.h>

const int dataPins[4] = {A0, A1, A2, A3};
const int threshold = 50;

int detectBitByDuration(int durationMs);
void syncTone(int pin, unsigned int  freqency, unsigned long delay, unsigned long GI);

#endif