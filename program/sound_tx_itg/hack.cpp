#include <hack.h>

int detectBitByDuration(int durationMs) {
  int lightDetected[4] = {0, 0, 0, 0};
  unsigned long start = millis();

  while (millis() - start < durationMs) {
    for (int i = 0; i < 4; i++) {
      if (analogRead(dataPins[i]) > threshold) {  // 光を検出したらカウント
        lightDetected[i]++;
      }
    }
  }

  // 最も多く光を検出したピンを選ぶ（ノイズ対策）
  int maxIndex = -1;
  int maxVal = 0;
  for (int i = 0; i < 4; i++) {
    if (lightDetected[i] > maxVal) {
      maxVal = lightDetected[i];
      maxIndex = i;
    }
  }

  // しきい値以上の光検出があれば有効
  if (maxVal > 3) {
    return maxIndex;
  } else {
    return -1;  // ノイズとみなして無視
  }
}

void syncTone(int pin, unsigned int  freqency, unsigned long delayt, unsigned long GI){
    tone(pin, freqency);
    delay(delayt);
    noTone(pin);
    delay(GI);
}
