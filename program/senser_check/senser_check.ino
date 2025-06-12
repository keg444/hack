const int photoTransistorPin = A4; // フォトトランジスタが接続されるアナログピン

void setup() {
  Serial.begin(9600);              // シリアル通信を開始
}

void loop() {
  int sensorValue = analogRead(photoTransistorPin);  // フォトトランジスタの値を読み取る

  // センサ値に基づいて4段階の明るさを設定
  
  Serial.print("Sensor Value: ");       // センサ値をシリアルモニタに出力
  Serial.println(sensorValue);

  delay(50);                           // 50ms待機
}

