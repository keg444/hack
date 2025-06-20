#include"base4.h"
const int speaker_pin = 3;  //スピーカー接続ピン
unsigned long t = 300;  // 音を鳴らす時間(ms)
unsigned long GI = t * 0.30; // GI長(tの25%)
unsigned int freq[16] = {1950, 2050, 2150, 2250, 2350, 2450, 2550, 2650, 2750, 2850, 2950, 3050, 3150, 3250, 3350, 3450};//鳴らす周波数を設定（最低周波数を開始ビットに）
unsigned int sfreq = 3550;
String str[16] = {"00","01","02","03","10","11","12","13","20","21","22","23","30","31","32","33"} ; // 取得した4進数2桁

// const int maxmoji = 256 * 4;  // 最大文字数256文字分（ASCII）×4桁
// int base4[maxmoji];           // 4進数の結果を格納する配列
// int resultIndex = 0;          // 4進数変換したあとの文字数+1

void setup() {
  Serial.begin(9600);
  pinMode(speaker_pin, OUTPUT);
  // for (int i=0; j<18; j++){
  //   for (int j=2050; j<3700; j+=100){
  //     freq[i] += j;
  //   }
  // }
  
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // 改行まで読み込む
    resultIndex = 0;  // 4進数変換したあとの文字数

    for (int i = 0; i < input.length(); i++) {
      char c = input.charAt(i); // 読み込んだ文字列を取得
      byte ascii = (byte)c; // ascii変換(10進)
      getBase4(ascii);  // 4進変換
    }

    String s="";

    tone(speaker_pin, sfreq); // 開始用の周波数を出力
    delay(t);
    noTone(speaker_pin);
    delay(GI);
    tone(speaker_pin, sfreq); // 開始用の周波数を出力
    delay(t);
    noTone(speaker_pin);
    delay(GI);



    for (int j=0; j<resultIndex; j++){ // 文字列に変換
      s += String(base4[j]);
    }
    for (int k=0; k<resultIndex; k+=2){ // 前から2桁ずつ音を鳴らす
      for (int l=0; l<16; l++){
        if(s.substring(k, k+2) == str[l]){
          tone(speaker_pin, freq[l]);
          delay(t);
          noTone(speaker_pin);
          delay(GI);
        }
      }
    }

    tone(speaker_pin, sfreq); // 終了用の周波数を出力
    delay(t);
    noTone(speaker_pin);
    delay(GI);
    tone(speaker_pin, sfreq); // 開始用の周波数を出力
    delay(t);
    noTone(speaker_pin);
    delay(GI);

    //確認用
    Serial.print("Base4: ");
    for (int i = 0; i < resultIndex; i++) {
      Serial.print(base4[i]); // 4進数の左から表示
      s += String(base4[i]);
    }
    Serial.println("");
    for (int l=0; l<resultIndex; l+=2){ // 前から2桁ずつ音を鳴らす
      for (int m=0; m<16; m++){
        if(s.substring(l, l+2) == str[m]){
          String foo = s.substring(l, l+2);
          int bar = ((foo.charAt(0) - '0') * 4 + (foo.charAt(1) - '0'));
          Serial.print(freq[bar+1]);
          Serial.println(" Hz");
        }
      }
    }
    Serial.println("------------------------");
  }
}
// void getBase4(byte b) {
//   int amari[4];
//   for (int i = 3; i >= 0; i--) {
//     amari[i] = b % 4;
//     b = b / 4;
//   }
//   for (int i = 0; i < 4; i++) {
//     base4[resultIndex++] = amari[i];
//   }
// }




