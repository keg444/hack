const int motorPin = 3;
const unsigned long bitDuration = 200;  // 1ビット = 200ms
const int signalBitCount = 7;           // 開始信号ビット数（1を7回）
const int endSignalBitCount = 10;       // 終了信号ビット数（0を10回）

void sendVibrationBit(int bitValue) {
  digitalWrite(motorPin, bitValue == 1 ? HIGH : LOW);
  delay(bitDuration);
  digitalWrite(motorPin, LOW);  // 振動のOFFを明示
}

void sendByte(byte data) {
  Serial.print("Sending bits: ");
  for (int bit = 6; bit >= 0; bit--) {
    int bitValue = (data >> bit) & 0x01;
    sendVibrationBit(bitValue);
    Serial.print(bitValue);
  }
  Serial.println();
}

void sendStartSignal() {
  for (int i = 0; i < signalBitCount; i++) {
    sendVibrationBit(1);
  }
}

void sendEndSignal() {
  for (int i = 0; i < endSignalBitCount; i++) {
    sendVibrationBit(0);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    sendStartSignal();
    delay(bitDuration);  // ← ここを追加して開始信号後に待機

    for (int i = 0; i < input.length(); i++) {
      byte b = input.charAt(i) & 0x7F;  // 7ビットASCIIに制限
      sendByte(b);
    }

    sendEndSignal();
    Serial.println("Message sent.");
    digitalWrite(motorPin, LOW);
  }
}