#include <M5StickCPlus.h>
#include "BluetoothSerial.h"

#define DEVICE_NAME "BT_SPP_SLAVE"

BluetoothSerial SerialBT;

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 0, 2);

  // setup BLE serial
  SerialBT.begin(DEVICE_NAME); //Bluetooth device name
  Serial.println("Initialized");
}

void loop() {
  if (SerialBT.available()) {
    char value = SerialBT.read();
    Serial.write(value);
    M5.Lcd.print(value);
  }
  delay(20);
}
