const int speaker_pin = 3;  //スピーカー接続ピン
unsigned long t = 300;  // 音を鳴らす時間(ms)
unsigned long GI = t * 0.25; // GI長(tの25%)
unsigned int freq[17] = {450, 650, 850, 1050, 1250, 1450, 1650, 1850, 2050, 2250, 2450, 2650, 2850, 3050, 3250, 3450, 3850}; //鳴らす周波数を設定（最低周波数を開始ビットに）
char* str[16] = {"00","01","02","03","10","11","12","13","20","21","22","23","30","31","32","33"} ; // 取得した4進数2桁

const int maxmoji = 256 * 4;  // 最大文字数256文字分（ASCII）×4桁
int base4[maxmoji];           // 4進数の結果を格納する配列
int resultIndex = 0;          // 4進数変換したあとの文字数+1

void setup() {
  Serial.begin(9600);
  pinMode(speaker_pin, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // 改行まで読み込む
    resultIndex = 0;  // 4進数変換したあとの文字数

    for (int i = 0; i < input.length(); i++) {
      char c = input.charAt(i); // 読み込んだ文字列の長さを取得
      byte ascii = (byte)c; // ascii変換(10進)
      getBase4(ascii);  // 4進変換
    }

    String s="";
    for (int j=0; j<resultIndex; j++){ // 文字列に変換
      s += String(base4[j]);
    }
    for (int k=0; k<resultIndex; k+=2){
      for (int l=0; l<16; l++){
        if(s.substring(k, k+2) == str[l]){
          tone(speaker_pin, freq[l+1]);
          delay(t);
          noTone(speaker_pin);
          delay(GI);
          // Serial.print("frequency:");
          // Serial.println(freq[l]);
        }
      }
    }

    //確認用
    Serial.print("Base4: ");
    for (int i = 0; i < resultIndex; i++) {
      Serial.print(base4[i]); // 4進数の左から表示
      s += String(base4[i]);
    }
    Serial.println("");
    for(int m=0; m<resultIndex; m+=2){
      Serial.println(s.substring(m, m+2));  // 各2桁を表示
    }
    Serial.println("------------------------");
  }
}

void getBase4(byte b) {
  int amari[4];
  for (int i = 3; i >= 0; i--) {
    amari[i] = b % 4;
    b = b / 4;
  }
  for (int i = 0; i < 4; i++) {
    base4[resultIndex++] = amari[i];
  }
}


// メモ　４進数の数列を文字列に変換→
//  String three = "03";
//     int k;
//     for(k=0; k<resultIndex; k+=2){
//       // Serial.println(s.substring(k,k+2));
//       // Serial.print("k+k+2; ");
//       Serial.print(s.substring(k, k+2));
//       if(s.substring(k,k+2) == "03"){
//         Serial.println("aaa");
//       }else{
//         Serial.println("bbb");
//       }



