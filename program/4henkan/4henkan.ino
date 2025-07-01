const int maxmoji = 256 * 4;  // 最大文字数256文字分（ASCII）×4桁
int base4[maxmoji];           // 4進数の結果を格納する配列
int resultIndex = 0;
int time = 5;

void setup() {
  Serial.begin(9600);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
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

    Base4sousin();
    
    for (int i = 0; i < resultIndex; i++) {
      Serial.print(base4[i]);
    }
    Serial.println("");
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

void Base4sousin() {
  digitalWrite(A4, HIGH);

  for (int k = 0; k < resultIndex; k++) {
    int pin;
    if (base4[k] == 0) {
      pin = A0;
    } else if (base4[k] == 1) {
      pin = A1;
    } else if (base4[k] == 2) {
      pin = A2;
    } else {
      pin = A3;
    }

    digitalWrite(pin, HIGH);
    delay(time);              // この時間内に受信側が読み取り
    digitalWrite(pin, LOW);
  }

  digitalWrite(A4, LOW);    // 送信終了通知
}
