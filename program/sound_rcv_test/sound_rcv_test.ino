#include <arduinoFFT.h>
const uint16_t samples = 256;                     // FFTサンプル数（2のべき乗）
const double samplingFrequency = 8000.0;          // サンプリング周波数8kHz
const double samplingIntervalUs = 1000000.0 / samplingFrequency;  // 125us
const uint8_t micPin = A0;

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long targetTime = micros();

  for (int i = 0; i < samples; i++) {
    while (micros() < targetTime) {
      // サンプリング間隔になるまで待機
    }
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
  double peakFrequency = (peakIndex * samplingFrequency) / samples;

  Serial.print("Peak frequency: ");
  Serial.print(peakFrequency, 2);
  Serial.println(" Hz");

  delay(300);  // 100ms待機
}
