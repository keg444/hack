const int maxmoji = 256 * 4;
int base4[maxmoji]; 
int resultIndex = 0;  

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