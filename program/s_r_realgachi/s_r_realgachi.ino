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
int startSignalCount = 0;
int endSignalCount = 0;
//----------------------------------------------------//
String str[16] = {"00","01","02","03","10","11","12","13","20","21","22","23","30","31","32","33"};
unsigned int binaryarray[] = {0, 1, 10, 11, 100, 101, 110, 111, 1000, 1001, 1010, 1011, 1100, 1101, 1110, 1111};
const int maxlit = 256*4;
int binary[maxlit];
int ReceivedLength;

void printAsAsciiFromFullBinary(String quadStr);

void setup() {
  Serial.begin(9600);
}

void loop() {
  double freq = detectFrequency();
  int decoded = decodeFrequency(freq);
  int binaryIndex = 0;
  for (int i = 0; i < maxlit; i++) {
    binary[i] = 0;
  }

  if (decoded == -1) {
    if (!receiving) {
      startSignalCount++;
      if (startSignalCount >= 2) {
        decodedData = "";
        receiving = true;
        startSignalCount = 0;
        endSignalCount = 0;
        Serial.println("----- Start Receiving -----");
      } else {
        Serial.print("Start signal received ");
        Serial.print(startSignalCount);
        Serial.println(" time(s). Waiting for 2nd signal...");
      }
    } else {
      endSignalCount++;
      if (endSignalCount >= 2) {
        receiving = false;
        startSignalCount = 0;
        endSignalCount = 0;
        Serial.println("----- End Receiving -----");
        Serial.print("Received (quad): ");
        Serial.println(decodedData);
        Serial.print("text (from full binary): ");
        printAsAsciiFromFullBinary(decodedData);
        
        ReceivedLength = decodedData.length();
        Serial.print("長さ: ");
        Serial.println(ReceivedLength);

        for(int i=0; i<ReceivedLength; i+=2){
          String currentQuad = decodedData.substring(i, i+2);
          for(int j=0; j<16; j++){
            if(currentQuad == str[j]){
              if(binaryIndex < (sizeof(binary)/sizeof(binary[0]))){
                String paddedBinaryStr = String(binaryarray[j]);
                while (paddedBinaryStr.length() < 4) {
                    paddedBinaryStr = "0" + paddedBinaryStr;
                }
                binary[binaryIndex] = paddedBinaryStr.toInt();

                Serial.print("binaryarray (padded int): ");
                Serial.println(binary[binaryIndex]);
                binaryIndex++;
              }else{
                Serial.println("Error: binary array overflow!");
              }

              for(int l=0; l<binaryIndex; l++){
                Serial.print(binary[l]);
              }
              Serial.println("");
              Serial.print("binary (original value): ");
              Serial.println(binaryarray[j]);
              break;
            }
          }
        }
        Serial.print("Final binary[] content (padded int): [");
        for (int i = 0; i < binaryIndex; i++) {
          Serial.print(binary[i]);
          if (i < binaryIndex - 1) {
            Serial.print(", ");
          }
        }
        Serial.println("]");

      } else {
        Serial.print("End signal received ");
        Serial.print(endSignalCount);
        Serial.println(" time(s). Waiting for 2nd signal to end...");
      }
    }
  } else if (receiving && decoded >= 0 && decoded <= 15) {
    endSignalCount = 0;
    char quadDigit1 = '0'+(decoded/4);
    char quadDigit2 = '0'+(decoded%4);
    String quadStr = String(quadDigit1) + String(quadDigit2);
    Serial.print("Adding quad: ");
    Serial.println(quadStr);
    decodedData += quadStr;
  } else if (!receiving && decoded >= 0 && decoded <= 15) {
    startSignalCount = 0;
  }
  delay(290);
}

double detectFrequency() {
  unsigned long targetTime = micros();
  for (int i = 0; i < samples; i++) {
    while (micros() < targetTime) {}
    vReal[i] = analogRead(micPin);
    vImag[i] = 0;
    targetTime += samplingIntervalUs;
  }

  double mean = 0;
  for (int i = 0; i < samples; i++) {
    mean += vReal[i];
  }
  mean /= samples;
  for (int i = 0; i < samples; i++) {
    vReal[i] -= mean;
  }

  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

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

int decodeFrequency(double freq) {
  int freqInt = (int)(freq + 0.5);

  if (freqInt >= 3540 && freqInt <= 3570) return -1;

  for (int i = 0; i < 16; i++) {
    int expected = 1950 + 100 * i;
    if (freqInt >= expected - 30 && freqInt <= expected + 40) {
      return i;
    }
  }
  return -2;
}

void printAsAsciiFromFullBinary(String quadStr) {
  String fullBinaryString = "";
  int currentReceivedLength = quadStr.length();

  for(int i = 0; i < currentReceivedLength; i += 2){
    String currentQuad = quadStr.substring(i, i + 2);
    bool found = false;

    for(int j = 0; j < 16; j++){
      if(currentQuad == str[j]){
        String binStr = String(binaryarray[j]);
        while (binStr.length() < 4) {
            binStr = "0" + binStr;
        }
        fullBinaryString += binStr;
        found = true;
        break;
      }
    }
    if (!found) {
      Serial.print("Warning: Unknown 4-ary chunk '");
      Serial.print(currentQuad);
      Serial.println("' detected. Skipping and inserting '0000'.");
      fullBinaryString += "0000";
    }
  }

  Serial.print("  Full Binary String: ");
  Serial.println(fullBinaryString);

  Serial.print("  Converted ASCII: ");
  for (int i = 0; i + 7 < fullBinaryString.length(); i += 8) {
    String byteBinaryStr = fullBinaryString.substring(i, i + 8);
    long byteValue = 0;
    for (int k = 0; k < 8; k++) {
      if (byteBinaryStr.charAt(k) == '1') {
        byteValue += (1 << (7 - k));
      }
    }
    Serial.print((char)byteValue);
  }
  Serial.println();
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