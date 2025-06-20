#include<hack.h>

const int maxmoji = 256 * 4;  // 最大文字数256文字分（ASCII）×4桁
int base4[maxmoji];           // 4進数の結果を格納する配列
int resultIndex = 0;          // 4進数変換したあとの文字数+1

// 音送信用の変数
const int speaker_pin = 3;  //スピーカー接続ピン
unsigned long t = 300;  // 音を鳴らす時間(ms)
unsigned long GI = t * 0.25; // GI長(tの25%)
unsigned int freq[17];//鳴らす周波数を設定（最低周波数を開始ビットに）
String str[16] = {"00","01","02","03","10","11","12","13","20","21","22","23","30","31","32","33"} ; // 取得した4進数2桁

// 光受信用の変数
const int detectPin = A4; // A4の光（送信中）を検出
const int dataPins[4] = {A0, A1, A2, A3}; // データ光（0〜3）を検出
bool inReceiving = false; // 現在受信中かどうか
const int threshold = 50; // 閾値
const int read_time = 50; // 読み取る間隔[ms]

void setup() {
  Serial.begin(9600);
  pinMode(speaker_pin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(dataPins[i], INPUT);
  }
  for(unsigned int i=0; i<17; i++){
    freq[i] = 2050 + i+100;
  }
}

void loop() {
  int detectValue = analogRead(detectPin);

  if (detectValue > threshold) {
    // 送信中
    if (!inReceiving) {
      resultIndex = 0;      // 初回のみ初期化
      inReceiving = true;
    }

    int val = detectBitByDuration(read_time);  // msごとの読み取り
    if (val != -1) {
      base4[resultIndex++] = val; // 桁数を数える
    }
  }else{
    // 送信終了時
    if (inReceiving) {

      String s = "";

      // 開始用の周波数を出力
      syncTone(speaker_pin, freq[0], t, GI);

      for (int i=0; i<resultIndex; i++){
        s += String(base4[i]);
      }
      for (int k=0; k<resultIndex; k+=2){
        // 前から2桁ずつ音を鳴らす
        for (int l=0; l<16; l++){
          if(s.substring(k, k+2) == str[l]){
            tone(speaker_pin, freq[l+1]);
            delay(t);
            noTone(speaker_pin);
            delay(GI);
          }
        }
      }

      // 終了用の周波数を出力
      syncTone(speaker_pin, freq[0], t, GI);

      // 確認用シリアル表示（読み取り）
      Serial.print("受信データ: ");
      for (int m=0; m<resultIndex; m++) {
        Serial.print(base4[m]);
      }
      Serial.println("");
      Serial.println("-----------------");
      Serial.print("freq: ");
      Serial.println("");
      for(int n=0; n<resultIndex; n+=2){
        String hoge = s.substring(n,n+2);
        int aaa = hoge.toInt();
        Serial.print(freq[aaa+1]);
        Serial.println(" Hz");
      }
      Serial.println("------------------------");
      inReceiving = false;  // フラグを戻す
    }
  }
}


// int detectBitByDuration(int durationMs) {
//   int lightDetected[4] = {0, 0, 0, 0};
//   unsigned long start = millis();

//   while (millis() - start < durationMs) {
//     for (int i = 0; i < 4; i++) {
//       if (analogRead(dataPins[i]) > threshold) {  // 光を検出したらカウント
//         lightDetected[i]++;
//       }
//     }
//   }

//   // 最も多く光を検出したピンを選ぶ（ノイズ対策）
//   int maxIndex = -1;
//   int maxVal = 0;
//   for (int i = 0; i < 4; i++) {
//     if (lightDetected[i] > maxVal) {
//       maxVal = lightDetected[i];
//       maxIndex = i;
//     }
//   }

//   // しきい値以上の光検出があれば有効
//   if (maxVal > 3) {
//     return maxIndex;
//   } else {
//     return -1;  // ノイズとみなして無視
//   }
// }

