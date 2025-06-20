#include <arduinoFFT.h>

const uint16_t samples = 256;
const double samplingFrequency = 8000.0;
const double samplingIntervalUs = 1000000.0 / samplingFrequency;
const uint8_t micPin = A0;

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

String decodedData = "";
bool receiving = false;
int startEndSignalCount = 0;

int lastValidDecoded = -2;
// int resultIndex = 0;
// const int maxmoji = 256*4;
// int aaa[maxmoji];

void setup() {
  Serial.begin(9600);
}

void loop() {
  double freq = detectFrequency();
  int decoded = decodeFrequency(freq);

  if (decoded == -1) {  // Start/End シグナル
    startEndSignalCount++; // シグナルが検出されるたびにカウントを増やす
    Serial.print("Start/End Signal Count: ");
    Serial.println(startEndSignalCount);
    if (!receiving) {
      decodedData = "";  // 開始時に初期化
      receiving = true;
      Serial.println("----- Start Receiving -----");
    } else {
      receiving = false;
      Serial.println("----- End Receiving -----");
      Serial.print("Received (quad): ");
      Serial.println(decodedData);
      Serial.print("text: ");
      printAsAscii(decodedData);
      Serial.println("");
      Serial.print("num: ");
      int num = decodedData.toInt();
      Serial.print(num);                                                                        
    }
  } else if (receiving && decoded >= 0 && decoded <= 15) {
    char quadStr[3];
    sprintf(quadStr, "%c%c", '0'+(decoded/4), '0'+(decoded%4)); // 例：10→"22"
    Serial.print("Adding quad: "); // 追加デバッグ
    Serial.println(quadStr);        // 追加デバッグ
    decodedData += String(quadStr); // decodedDataに文字列として追加
  }
  lastValidDecoded = decoded;  // 最後の値を記録
  delay(300);  // 次の音まで待機
}
// -----------------------------------------------------------------
// FFTでピーク周波数を検出する関数
double detectFrequency() {
  unsigned long targetTime = micros();
  for (int i = 0; i < samples; i++) {
    while (micros() < targetTime) {}
    vReal[i] = analogRead(micPin);
    vImag[i] = 0;
    targetTime += samplingIntervalUs;
  }

  // DCオフセット除去
  double mean = 0;
  for (int i = 0; i < samples; i++) {
    mean += vReal[i];
  }
  mean /= samples;
  for (int i = 0; i < samples; i++) {
    vReal[i] -= mean;
  }

  // FFT処理
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  // ピーク周波数検出
  double peak = 0;
  int peakIndex = 0;
  for (int i = 1; i < samples / 2; i++) {
    if (vReal[i] > peak) {
      peak = vReal[i];
      peakIndex = i;
    }
  }
  return (peakIndex * samplingFrequency) / samples;
}

// 周波数から4進数2桁データをデコード（0〜15）
int decodeFrequency(double freq) {
  int freqInt = (int)(freq + 0.5);

  if (freqInt >= 3330 && freqInt <= 3370) return -1;  // Start/End signal

  for (int i = 0; i < 16; i++) {
    int expected = 1950 + 100 * i;
    if (freqInt >= expected - 30 && freqInt <= expected + 30) {
      return i;
    }
  }
  return -2;  // 無効な周波数
}



void printAsAscii(String quadStr) {
  for (int i = 0; i + 3 < quadStr.length(); i += 4) {
    int b0 = quadStr[i]   - '0';
    int b1 = quadStr[i+1] - '0';
    int b2 = quadStr[i+2] - '0';
    int b3 = quadStr[i+3] - '0';
    int bytevalue = b0 * 64 + b1 * 16 + b2 * 4 + b3;
    Serial.print((char)bytevalue);
  }
  Serial.println();
}
