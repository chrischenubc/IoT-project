#include <SoftwareSerial.h>

SoftwareSerial BT(9, 10); 
float value = 1000.22;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BT.begin(9600);
}

void loop() {
  Serial.println(value);
  BT.print(value);
  value++;
  delay(1000);
}

float addOneToLeft(float value, String beginNum) {
  String string1 = String(value);
  String string2 = "1" + beginNum + string1;
  return string2.toFloat();
}
