#include <Wire.h>
#include <ds3231.h>

ts t;

void setup() {
  Wire.begin();
  DS3231_init(DS3231_INTCN);
  Serial.begin(9600);
  while(!Serial){}
}

void loop() {
  DS3231_get(&t);
  Serial.print("Day : ");
  Serial.print(t.mon);
  Serial.print("/");
  Serial.print(t.mday);
  Serial.print("/");
  Serial.println(t.year);
  Serial.print("Hour : ");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(".");
  Serial.println(t.sec);
#ifdef CONFIG_UNIXTIME
  Serial.print("Timestamp : ");
  Serial.print(t.unixtime);
#endif
  Serial.println();
  delay(1000);
}
