#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

BLEScan* pBLEScan;

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true); // active scan for more data
}

void loop() {
  Serial.println("Scanning...");
  BLEScanResults foundDevices = pBLEScan->start(5, false); // scan for 5 seconds
  for (int i = 0; i < foundDevices.getCount(); ++i) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    Serial.print("Device: ");
    Serial.print(device.getAddress().toString().c_str());
    Serial.print(" RSSI: ");
    Serial.println(device.getRSSI());
  }
  Serial.println("Scan done.");
  delay(2000); // wait before next scan
}