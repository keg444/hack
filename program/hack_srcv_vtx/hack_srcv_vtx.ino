#include <arduinoFFT.h>

// --- 受信側（マイク）設定 ---
const uint16_t samples = 256;              // FFTサンプル数
const double samplingFrequency = 8000.0;   // サンプリング周波数 (Hz)
const double samplingIntervalUs = 1000000.0 / samplingFrequency; // サンプリング間隔 (マイクロ秒)
const uint8_t micPin = A0;                 // マイク入力用アナログピン

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

String decodedData = "";
bool receiving = false;
int startSignalCount = 0;
int endSignalCount = 0;

// --- 受信側データマッピング ---
String str[16] = {"00","01","02","03","10","11","12","13","20","21","22","23","30","31","32","33"};
unsigned int binaryarray[] = {0, 1, 10, 11, 100, 101, 110, 111, 1000, 1001, 1010, 1011, 1100, 1101, 1110, 1111};
const int maxlit = 256 * 4; // 受信データ格納用配列の最大サイズ
int binary[maxlit]; // パディングされた2進数を整数として格納（主にデバッグ表示用）
int ReceivedLength;

// --- 送信側（振動モーター）設定 ---
const int motorPin = 5; // 振動モーター用デジタルピンをD5に設定
const unsigned long bitDuration = 200;  // 1ビットの持続時間 (ms)
const int signalBitCount = 7;           // 送信開始信号ビット数（1を7回）
const int endSignalBitCount = 10;       // 送信終了信号ビット数（0を10回）

// --- 関数プロトタイプ宣言 ---
// 受信側
double detectFrequency();
int decodeFrequency(double freq);
// 受信した4進数文字列をASCIIバイト列に変換し、各バイトを振動送信する関数
void convertAndSendVibration(String quadStr);

// 送信側
void sendVibrationBit(int bitValue);
void sendByte(byte data);
void sendStartSignal();
void sendEndSignal();


void setup() {
  Serial.begin(9600);           // シリアル通信の初期化
  pinMode(motorPin, OUTPUT);    // モーターピンを出力に設定
  digitalWrite(motorPin, LOW);  // モーターを初期状態はOFFにする
}

void loop() {
  // --- 受信処理ブロック ---
  double freq = detectFrequency();
  int decoded = decodeFrequency(freq);
  int binaryIndex = 0; // binary配列の書き込みインデックスをここで宣言・初期化
  for (int i = 0; i < maxlit; i++) {
    binary[i] = 0; // binary配列を毎回リセット
  }

  if (decoded == -1) {  // 開始/終了シグナル
    if (!receiving) {
      // 通信開始前
      startSignalCount++;
      if (startSignalCount >= 2) { // 2回以上受け取ったら開始
        decodedData = "";  // 開始時に初期化
        receiving = true;
        startSignalCount = 0;
        endSignalCount = 0;
        Serial.println("----- Start receiving -----");
      } else {
        Serial.print("Start signal received ");
        Serial.print(startSignalCount);
        Serial.println(" time(s). Waiting for 2nd signal...");
      }
    } else {
      // 通信中
      endSignalCount++;
      if (endSignalCount >= 2) { // 2回以上受け取ったら終了
        receiving = false;
        startSignalCount = 0;
        endSignalCount = 0;
        Serial.println("----- End receiving -----");
        Serial.print("Received(row): ");
        Serial.println(decodedData);

               // --- ここから新しい変更 ---
        Serial.print("Decoded ASCII: "); // 新しいラベル
        String fullBinaryStringForDisplay = "";
        int currentReceivedLengthForDisplay = decodedData.length();

        for(int i = 0; i < currentReceivedLengthForDisplay; i += 2){
          String currentQuad = decodedData.substring(i, i + 2);
          bool found = false;

          for(int j = 0; j < 16; j++){
            if(currentQuad == str[j]){
              String binStr = String(binaryarray[j]);
              while (binStr.length() < 4) {
                  binStr = "0" + binStr;
              }
              fullBinaryStringForDisplay += binStr;
              found = true;
              break;
            }
          }
        }

        // 8ビットごとにASCII文字に変換して表示
        for (int i = 0; i + 7 < fullBinaryStringForDisplay.length(); i += 8) {
          String byteBinaryStr = fullBinaryStringForDisplay.substring(i, i + 8);
          long byteValue = 0;
          for (int k = 0; k < 8; k++) {
            if (byteBinaryStr.charAt(k) == '1') {
              byteValue += (1 << (7 - k));
            }
          }
          Serial.print((char)byteValue); // 文字を直接表示
        }
        Serial.println(); // デコードされた文字の表示後に改行
        // --- ここまで新しい変更 ---

        // 受信した4進数データをASCIIに変換し、そのバイナリ表現を振動として送信 // 変更点
        convertAndSendVibration(decodedData); // 変更点
        
        // --- 以下はデバッグ表示用（受信機のデバッグ出力として残す） ---
        ReceivedLength = decodedData.length();
        Serial.print("Received length(row):  ");
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
                Serial.println("Error: Storage overflow for binary array.");
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
        Serial.print("finally binary[] contents (padded int): [");
        for (int i = 0; i < binaryIndex; i++) {
          Serial.print(binary[i]);
          if (i < binaryIndex - 1) {
            Serial.print(", ");
          }
        }
        Serial.println("]");
        // -----------------------------------------------------------------

      } else {
        Serial.print("End signal reveived ");
        Serial.print(endSignalCount);
        Serial.println(" time(s). Waiting for 2nd signal to end...");
      }
    }
  } else if (receiving && decoded >= 0 && decoded <= 15) {
    // データ受信中
    endSignalCount = 0; // データが来たら終了カウントをリセット（連続性を保つため）
    char quadDigit1 = '0'+(decoded/4);
    char quadDigit2 = '0'+(decoded%4);
    String quadStr = String(quadDigit1) + String(quadDigit2);
    Serial.print("Adding quad: ");
    Serial.println(quadStr);
    decodedData += quadStr;
  } else if (!receiving && decoded >= 0 && decoded <= 15) {
    // 通信開始前にデータが来ても無視し、開始カウントをリセット
    startSignalCount = 0;
  }
  delay(290); // 次の音まで待機
}

// --- 受信側関数定義 ---

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

// 周波数から4進数2桁データをデコード（0〜15）
int decodeFrequency(double freq) {
  int freqInt = (int)(freq + 0.5);

  if (freqInt >= 3570 && freqInt <= 3630) return -1; // 開始/終了信号

  for (int i = 0; i < 16; i++) {
    int expected = 2000 + 100 * i;
    if (freqInt >= expected - 49 && freqInt <= expected + 49) {
      return i;
    }
  }
  return -2; // 無効な周波数
}

// 受信した4進数文字列をASCIIバイト列に変換し、各バイトを振動送信する関数
void convertAndSendVibration(String quadStr) {
  String fullBinaryString = "";
  int currentReceivedLength = quadStr.length();

  // 4進数チャンクをパディングされた4ビットの2進数文字列に変換して連結
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
      Serial.print("警告: 未知の4進数チャンク '");
      Serial.print(currentQuad);
      Serial.println("' が検出されました。スキップして '0000' を挿入します。");
      fullBinaryString += "0000"; // 未知のチャンクの場合、デフォルトで0000を挿入
    }
  }

  Serial.print(" Binary string (for vibration): ");
  Serial.println(fullBinaryString);
  Serial.print(" Converted ASCII (for vibration): ");
  Serial.println("");

  // まず送信開始信号を送る
  sendStartSignal();
  delay(bitDuration); // 送信開始信号後の待機

  // 8ビットごとにASCII文字に変換し、そのバイトを振動送信
  for (int i = 0; i + 7 < fullBinaryString.length(); i += 8) {
    String byteBinaryStr = fullBinaryString.substring(i, i + 8);
    long byteValue = 0;

    for (int k = 0; k < 8; k++) {
      if (byteBinaryStr.charAt(k) == '1') {
        byteValue += (1 << (7 - k));
      }
    }
    Serial.print((char)byteValue); // シリアルモニターに文字を表示 (ASCII)

    // デコードされたバイトを振動として送信
    sendByte((byte)byteValue);
  }
  Serial.println();

  // 全てのバイト送信後に送信終了信号を送る
  sendEndSignal();
  Serial.println("Vibration transmission completed");
  digitalWrite(motorPin, LOW); // モーターをOFFにする
}


// --- 送信側関数定義 ---

// 1ビットの値を振動として出力する関数
void sendVibrationBit(int bitValue) {
  digitalWrite(motorPin, bitValue == 1 ? HIGH : LOW);
  delay(bitDuration);
  digitalWrite(motorPin, LOW);  // 振動のOFFを明示
}

// 1バイトのデータをビット列に分解して振動として送信する関数
void sendByte(byte data) {
  Serial.print("  Bits for '");
  Serial.print((char)(data & 0x7F)); // 送信する文字（7ビットASCII）を表示
  Serial.print("': ");
  for (int bit = 6; bit >= 0; bit--) { // 7ビットASCIIの範囲でループ (MSBからLSBへ)
    int bitValue = (data >> bit) & 0x01; // ビット値を取得
    sendVibrationBit(bitValue);          // 振動として送信
    Serial.print(bitValue);              // シリアルモニターにビット値を表示
  }
  Serial.println();
}

// 送信開始信号を送る関数
void sendStartSignal() {
  Serial.println("--- 振動送信開始信号 ---"); // シリアルモニターに表示
  for (int i = 0; i < signalBitCount; i++) {
    sendVibrationBit(1); // 1 (HIGH) を signalBitCount 回送信
  }
}

// 送信終了信号を送る関数
void sendEndSignal() {
  Serial.println("--- 振動送信終了信号 ---"); // シリアルモニターに表示
  for (int i = 0; i < endSignalBitCount; i++) {
    sendVibrationBit(0); // 0 (LOW) を endSignalBitCount 回送信
  }
}