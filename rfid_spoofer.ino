#define switchPin 9

boolean fullread, bdot, data[64], rowsum[10], colsum[4];
int i, j, idot;
unsigned long code[2];

void encode() {
  code[0] = 0;
  code[1] = 0;
  fullread = false;
  while (!fullread) {
    if (Serial.available()) {
      idot = Serial.read() - 48;
      if (idot < 0) fullread = true;
      else {
        code[0] *= 10;
        if (code[1] > 429496729) code[0] += ((code[1] / 65536) * 10) / 65536;
        code[1] *= 10;
        Serial.print(idot);
        code[1] += idot;
      }
    } else delay(1);
  }
  Serial.print("\nSetting tag code to ");
  Serial.print(code[0], DEC);
  Serial.print(":");
  Serial.println(code[1]);
  for (i = 0; i < 9; i++) data[i] = true;
  data[63] = false;
  for (i = 0; i < 10; i++) rowsum[i] = false;
  for (i = 0; i < 4; i++) colsum[i] = false;
  for (i = 1; i > -1; i--) { //High half
    for (j = 3; j > -1; j--) {
      bdot = code[0] % 2;
      data[9+5*i+j] = bdot;
      rowsum[i] += bdot;
      colsum[j] += bdot;
      code[0] /= 2;
    }
  }
  for (i = 7; i > -1; i--) { //Low half
    for (j = 3; j > -1; j--) {
      bdot = code[1] % 2;
      data[19+5*i+j] = bdot;
      rowsum[2+i] += bdot;
      colsum[j] += bdot;
      code[1] /= 2;
    }
  }
  //Parity bits
  for (i = 0; i < 10; i++) data[13+5*i] = rowsum[i] % 2;
  for (i = 0; i < 4; i++) data[59+i] = colsum[i] % 2;
  Serial.println("Tag code set.");
}

void setup()
{
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, LOW);
  Serial.begin(9600);
  Serial.println("Monitor serial open.");
  Serial.println("Input a number to set spoofer's code.");
  Serial.println("End with a whitespace character.");
  Serial.println("125 kHz RFID tags have 5-byte codes.");
  Serial.println("Recieved number is treated as decimal.");
}

void loop()
{
  if (Serial.available()) encode();
  for(int i = 0; i < 64; i++)
  {
    digitalWrite(switchPin, data[i] ? HIGH : LOW);
    delayMicroseconds(256);
    digitalWrite(switchPin, data[i] ? LOW : HIGH);
    delayMicroseconds(256);
  }
}
