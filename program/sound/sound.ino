// speaker_pin = D3;  //スピーカー接続ピン
float t = 3;  // 音を鳴らす時間
float delay_t = t / 0.25; // GI長
int freq[] = {450, 650, 850, 1050, 1250, 1450, 1650, 1850, 2050, 2250, 2450, 2650, 2850, 3050, 3250, 3450, 3850}; //鳴らす周波数を設定（最低周波数を開始ビットに）

const int maxmoji = 256 * 4;  // 最大文字数256文字分（ASCII）×4桁
int base4[maxmoji];           // 4進数の結果を格納する配列
int resultIndex = 0;          // 4進数変換したあとの文字数+1

void setup() {
  Serial.begin(9600);
  pinMode(A0, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    resultIndex = 0;

    for (int i = 0; i < input.length(); i++) {
      char c = input.charAt(i);
      byte ascii = (byte)c;
      getBase4(ascii);
    }

    String s="";
    int k;
    for (int j=0; j<resultIndex; j+=2){ // 文字列に変換
      s += String(base4[j]);
    }
    for (int k=0; k<resultIndex; k+=2){
      if(s.substring(k, k+2) == "03"){
        digitalWrite(A0, HIGH);
      }
    }

    //確認用
    Serial.print("Base4: ");
    for (int i = 0; i < resultIndex; i++) {
      Serial.print(base4[i]);
      s += String(base4[i]);
    }
    Serial.println("");
    for(k=0; k<resultIndex; k+=2){
      Serial.println(s.substring(k, k+2));  // 各2桁を表示
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

