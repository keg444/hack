void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // 改行まで読み込む
    Serial.println("ASCIIコード（4桁の4進数）:");


    for (int i = 0; i < input.length(); i++) {
      char c = input.charAt(i);
      byte ascii = (byte)c; // 10進数
      Serial.print(c);
      Serial.print(" -> ");
      MoreNewPrintBase4(ascii);
      Serial.println();
      }
    
    Serial.println("-----");
  }
}

// 考案
void printBase4(byte q){
  int k;
  for(int i=4; i>0; i--){
    k = q % 4;  //余り
    q = q / 4;  //4で割る
    Serial.print(k);
  }
}

// TAヘルプ
void NewPrintBase4(byte q){
  int r;  //余り
  for (int i=4; i>0; i--){
    int div = 1;
    for (int j=0; j<i-1; j++){  // 4の3乗，2乗，1乗
      div *=4;
    }
    r = (q / div) % 4; //余りが4進数の各ビット
    Serial.print(r);
  }
}

// 考案
void MoreNewPrintBase4(byte q){
  // Serial.print(q);
  int r[4];
  for (int i=4; i>0; i--){
    r[i] = q % 4;  //余りを配列に保存
    q /= 4;
  }
  for (int i=1; i<5; i++){
    Serial.print(r[i]); //余りを逆順に読む
  }
}
