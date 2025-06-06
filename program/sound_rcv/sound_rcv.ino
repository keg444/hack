#include <arduinoFFT.h>

const uint16_t samples = 128;               // FFTサンプル数
const double samplingFrequency = 8000.0;  // サンプリング周波数約333Hz（3ms間隔）
const uint8_t micPin = A0;                   // マイク入力ピン

const double startSignalFreq = 450.0;        // 開始・終了信号の中心周波数
const double freqTolerance = 100.0;          // 周波数許容幅±100Hz
const double infoFreqMin = 650.0;             // 情報信号最低周波数
const double infoFreqMax = 3850.0;            // 情報信号最高周波数
const int infoCount = 16;                     // 情報信号の個数（4進数2桁分）

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

int receivedQuads[infoCount];   // 受信した4進数2桁データ
int receivedCount = 0;
bool isStartSignalDetected = false;

void setup() {
  Serial.begin(9600);
}

void loop() {
  double freq = detectFrequency();

  if (!isStartSignalDetected) {
    if (freq >= startSignalFreq - freqTolerance && freq <= startSignalFreq + freqTolerance) {
      isStartSignalDetected = true;
      receivedCount = 0;
      Serial.println("Start signal detected!");
    }
  } else {
    if (freq >= startSignalFreq - freqTolerance && freq <= startSignalFreq + freqTolerance) {
      Serial.println("End signal detected!");
      if (receivedCount == infoCount) {
        char asciiChar = 0;
        convertQuadsToAscii(receivedQuads, infoCount, asciiChar);
        Serial.print("Decoded ASCII char: ");
        Serial.println(asciiChar);
      } else {
        Serial.println("Received data length error.");
      }
      isStartSignalDetected = false;
    } else {
      if (freq >= infoFreqMin - freqTolerance && freq <= infoFreqMax + freqTolerance) {
        double step = (infoFreqMax - infoFreqMin) / infoCount;
        int index = (int)((freq - infoFreqMin + freqTolerance) / step);
        if (index >= 0 && index < infoCount && receivedCount < infoCount) {
          receivedQuads[receivedCount++] = index;
          Serial.print("Received quad: ");
          Serial.println(index);
        }
      }
    }
  }

  delay(300);
}

// FFTで周波数を検出
double detectFrequency() {
  for (int i = 0; i < samples; i++) {
    vReal[i] = analogRead(micPin);
    vImag[i] = 0;
    delayMicroseconds((int)(1000000.0 / samplingFrequency));
  }
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  double peak = 0;
  int peakIndex = 1;
  for (int i = 1; i < samples / 2; i++) {
    if (vReal[i] > peak) {
      peak = vReal[i];
      peakIndex = i;
    }
  }
  return (peakIndex * samplingFrequency) / samples;
}

// 4進数2桁×16個をまとめてASCII(7bit)に変換
void convertQuadsToAscii(int quads[], int length, char &asciiChar) {
  uint32_t combined = 0;
  for (int i = 0; i < length; i++) {
    combined <<= 2;
    combined |= (quads[i] & 0x03);
  }
  asciiChar = (char)(combined & 0x7F);
}
