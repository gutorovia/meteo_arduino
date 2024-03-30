#include <MQ135.h>
#include <stDHT.h>

// #define DHT11_DEBUG
// #define MQ135_DEBUG

#define SERIAL_SPEED 57600

void setup() {
  Serial.begin(SERIAL_SPEED);

  lightSetup();
  mq135Setup();
  buttonSetup();
  lcdSetup();
  zoomerSetup();
}

void loop() {
  mq135Processing();
  delay(1000);
}
