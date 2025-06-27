#include"hack.h"

const int maxmoji = 256 * 4;  // 最大文字数256文字分（ASCII）×4桁
int base4[maxmoji];           // 4進数の結果を格納する配列
int resultIndex = 0;          // 4進数変換したあとの文字数+1

// 音送信用の変数
const int speaker_pin = 3;  //スピーカー接続ピン
unsigned long t = 300;  // 音を鳴らす時間(ms)
unsigned long GI = t * 0.25; // GI長(tの25%)
unsigned int freq[16] = {2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500};//鳴らす周波数を設定（最低周波数を開始ビットに）
unsigned int sfreq = 3600;
String str[16] = {"00","01","02","03","10","11","12","13","20","21","22","23","30","31","32","33"} ; // 取得した4進数2桁
double txtime = 0;

// 光受信用の変数
const int detectPin = A4; // A4の光（送信中）を検出
// const int dataPins[4] = {A0, A1, A2, A3}; // データ光（0〜3）を検出
bool inReceiving = false; // 現在受信中かどうか
// const int threshold = 50; // 閾値
const int read_time = 10; // 読み取る間隔[ms]

void setup() {
  Serial.begin(9600);
  pinMode(speaker_pin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(dataPins[i], INPUT);
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
      txtime = 0;
      txtime = millis();
      String s = "";

      // 開始用の周波数を出力
      syncTone(speaker_pin, sfreq, t, GI);
      syncTone(speaker_pin, sfreq, t, GI);

      for (int i=0; i<resultIndex; i++){
        s += String(base4[i]);
      }
      for (int k=0; k<resultIndex; k+=2){
        // 前から2桁ずつ音を鳴らす
        for (int l=0; l<16; l++){
          if(s.substring(k, k+2) == str[l]){
            tone(speaker_pin, freq[l]);
            delay(t);
            noTone(speaker_pin);
            delay(GI);
          }
        }
      }
      delay(GI);

      // 終了用の周波数を出力
      syncTone(speaker_pin, sfreq, t, GI);
      syncTone(speaker_pin, sfreq, t, GI);

      // 確認用シリアル表示（読み取り）
      Serial.print("受信データ: ");
      for (int m=0; m<resultIndex; m++) {
        Serial.print(base4[m]);
      }
      Serial.println("");
      Serial.println("-----------------");
      Serial.print("Base4: ");
      for (int i = 0; i < resultIndex; i++) {
        Serial.print(base4[i]); // 4進数の左から表示
        s += String(base4[i]);
      }
      // Serial.println("");
      // Serial.print("time: ");
      // Serial.print(txtime/1000);
      // Serial.println(" s");
      Serial.println("");
      for (int l=0; l<resultIndex; l+=2){
        for (int m=0; m<16; m++){
          if(s.substring(l, l+2) == str[m]){
            String foo = s.substring(l, l+2);
            int bar = ((foo.charAt(0) - '0') * 4 + (foo.charAt(1) - '0'));
            Serial.print(freq[bar]);
            Serial.println(" Hz");
          }
        }
      }
      Serial.println("------------------------");
      inReceiving = false;  // フラグを戻す
    }
  }
}


