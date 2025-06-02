const int maxmoji = 256 * 4;  // 最大文字数256文字分（ASCII）×4桁
int base4[maxmoji];           // 4進数の結果を格納する配列
int resultIndex = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  //改行まで読み込む
    resultIndex = 0;

    for (int i = 0; i < input.length(); i++) {  //読み込んだ文字列の長さを取得
      char c = input.charAt(i); //１文字ずつcに代入
      byte ascii = (byte)c; //10進数のasciiコードに変換
      getBase4(ascii);  //4進数に変換
    }
    
    //確認用
    String s="";
    int k;
    Serial.print("Base4: ");
    for (int i = 0; i < resultIndex; i++) {
      Serial.print(base4[i]);
      s += String(base4[i]);
    }
    Serial.println("");
    for(k=0; k<resultIndex; k+=2){
      Serial.println(s.substring(k, k+2));
    }
    Serial.println("-------------------------");
  }
}

void getBase4(byte b) {
  int amari[4];
  for (int i = 3; i >= 0; i--) {
    amari[i] = b % 4;
    b = b / 4;
  }
  for (int i = 0; i < 4; i++) {
    base4[resultIndex++] = amari[i];  //resultIndexに代入して1増やす
  }
}
