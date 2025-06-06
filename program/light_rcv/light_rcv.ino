const int maxmoji = 256 * 4;
int base4[maxmoji];
int resultIndex = 0;

const int detectPin = A4; // A4の光（送信中）を検出
const int dataPins[4] = {A0, A1, A2, A3}; // データ光（0〜3）を検出

bool inReceiving = false; // 現在受信中かどうか

void setup() {
  Serial.begin(9600);
  pinMode(detectPin, INPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(dataPins[i], INPUT);
  }
}

void loop() {
  int detectValue = analogRead(detectPin);

  if (detectValue > 500) {
    // 送信中
    if (!inReceiving) {
      resultIndex = 0;      // 初回のみ初期化
      inReceiving = true;
    }

    int val = detectBitByDuration(50);  // 50msごとの読み取り
    if (val != -1) {
      base4[resultIndex++] = val;
    }

  } else {
    // 送信終了時に出力
    if (inReceiving) {
      Serial.print("受信データ: ");
      for (int i = 0; i < resultIndex; i++) {
        Serial.print(base4[i]);
      }
      Serial.println();
      inReceiving = false;  // フラグを戻す
    }
  }
}

int detectBitByDuration(int durationMs) {
  int lightDetected[4] = {0, 0, 0, 0};
  unsigned long start = millis();

  while (millis() - start < durationMs) {
    for (int i = 0; i < 4; i++) {
      if (analogRead(dataPins[i]) > 500) {  // 光を検出したらカウント
        lightDetected[i]++;
      }
    }
  }

  // 最も多く光を検出したピンを選ぶ（ノイズ対策）
  int maxIndex = -1;
  int maxVal = 0;
  for (int i = 0; i < 4; i++) {
    if (lightDetected[i] > maxVal) {
      maxVal = lightDetected[i];
      maxIndex = i;
    }
  }

  // しきい値以上の光検出があれば有効
  if (maxVal > 3) {
    return maxIndex;
  } else {
    return -1;  // ノイズとみなして無視
  }
}
