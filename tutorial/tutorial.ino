void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.write(100);
  delay(1000);
  Serial.write(356);
  delay(1000);
}

// void loop(){
//   for (int i=0; i<256; i++){
//     Serial.write(i);
//     delay(100);
//   }
// }