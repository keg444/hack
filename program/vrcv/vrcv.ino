
const int xPin = A0;
const int yPin = A1;
const int zPin = A2;

const unsigned long bitDuration = 230;           // 1ビットの持続時間（ms）
const float vibrationThreshold = 12.9;           // 振動検出のしきい値
const int signalBitCount = 5;                    // 開始信号（1×7回）
const int endSignalBitCount = 11;                // 終了信号（0×10回）
const int maxBits = 512;                         // 最大ビット数バッファ

// 加速度センサから3軸データを読み取る
void readXYZ(int &x, int &y, int &z) {
  x = analogRead(xPin);
  y = analogRead(yPin);
  z = analogRead(zPin);
}

// 指定時間内に振動があったかを検出
bool detectVibration(unsigned long duration) {
  unsigned long start = millis();
  int baseX, baseY, baseZ;
  readXYZ(baseX, baseY, baseZ);

  int vibrationCount = 0;
  int samples = 0;

  while (millis() - start < duration) {
    int xNow, yNow, zNow;
    readXYZ(xNow, yNow, zNow);
    float delta = sqrt(
      pow(xNow - baseX, 2) +
      pow(yNow - baseY, 2) +
      pow(zNow - baseZ, 2)
    );
    if (delta > vibrationThreshold) vibrationCount++;
    samples++;
    delay(5);  // 過剰サンプリング防止
  }

  return (vibrationCount > samples / 2);
}

// 開始信号（1×7）を検出
bool detectStartSignal() {
  Serial.println("Checking start signal...");
  for (int i = 0; i < signalBitCount; i++) {
    if (!detectVibration(bitDuration)) {
      Serial.println("Start signal failed");
      return false;
    }
    Serial.print("1");
  }
  Serial.println("\n[Start Detected]");
  return true;
}

// 終了信号（最後の10ビットが0）を検出
bool detectEndSignal(const bool *bits, int len) {
  if (len < endSignalBitCount) return false;
  for (int i = len - endSignalBitCount; i < len; i++) {
    if (bits[i]) return false;
  }
  return true;
}

// --- 初期化 ---
void setup() {
  Serial.begin(9600);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(zPin, INPUT);
  Serial.println("Ready to receive vibration-based communication");
}

// --- メインループ ---
void loop() {
  static bool bitsBuffer[maxBits];
  static int bitsCount = 0;

  Serial.println("Waiting for start signal...");
  while (!detectStartSignal());  // 開始信号を待機

  delay(3*bitDuration);

  bitsCount = 0;
  unsigned long bitStart;

  // --- ビット受信ループ ---
  while (true) {
    bitStart = millis();
    bool bit = detectVibration(bitDuration);
    bitsBuffer[bitsCount++] = bit;
    Serial.print(bit ? '1' : '0');

    // 終了信号検出
    if (bitsCount >= endSignalBitCount && detectEndSignal(bitsBuffer, bitsCount)) {
      Serial.println("\n[End Detected]");
      break;
    }

    // ビット数上限
    if (bitsCount >= maxBits) {
      Serial.println("\n[Error] Buffer overflow. Resetting.");
      return;
    }

    // 次のビットまで待機（正確な周期維持）
    while (millis() - bitStart < bitDuration) {
      delay(1);
    }
  }

  // --- 7ビットASCIIに復元 ---
  int dataBits = bitsCount - endSignalBitCount;
  int usableBits = (dataBits / 7) * 7;

  Serial.print("Decoded message: ");
  for (int i = 0; i + 6 < usableBits; i += 7) {
    byte c = 0;
    for (int b = 0; b < 7; b++) {
      if (bitsBuffer[i + b]) {
        c |= (1 << (6 - b));  // MSB→LSB順に復元
      }
    }
    Serial.print((char)c);
  }
  Serial.println();
}
