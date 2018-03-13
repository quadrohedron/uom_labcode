#include <SoftwareSerial.h>

SoftwareSerial rfid(2, 3);
int dot, index = 0, data[12];
unsigned long tag[2];

void decode() {
  tag[0] = 0;
  tag[1] = 0;
  for (index = 0; index < 10; index++) {
    tag[0] *= 16;
    if (tag[1] >= 268435456) tag[0] += tag[1] / 268435456;
    tag[1] *= 16;
    tag[1] += data[index];
  }
  Serial.print("Tag code: ");
  Serial.print(tag[1]);
  if (tag[0]) {
    Serial.print("   ( ");
    Serial.print(tag[0]);
    Serial.print(":");
    Serial.print(tag[1]);
    Serial.println(" )");
  } else Serial.println("");
  for (index = 1; index < 6; index++) {
    data[0] ^= data[2*index];
    data[1] ^= data[2*index+1];
  }
  if (data[0]*data[1]) Serial.println("Checksum verification failed!");
  else Serial.println("Checksum verified.");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Monitor serial open.");
  rfid.begin(9600);
}

void loop() {
  if (rfid.available()) {
    dot = rfid.read();
    if (dot == 2) index = 0;
    else if (dot == 3) decode();
    else {
      data[index] = (dot > 64) ? dot - 55 : dot - 48;
      index++;
    }
  }
}
