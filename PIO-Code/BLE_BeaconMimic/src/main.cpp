#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEBeacon.h>
#include <BLEAdvertising.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Init BLE...");

  BLEDevice::init("BeaconMimic");

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  BLEBeacon oBeacon;

  oBeacon.setManufacturerId(0x004C); // Apple iBeacon
  oBeacon.setProximityUUID(BLEUUID("E2C56DB5-DFFB-48D2-B060-D0F5A71096E0"));
  oBeacon.setMajor(1);
  oBeacon.setMinor(1);
  oBeacon.setSignalPower(-59);

  BLEAdvertisementData advData;
  BLEAdvertisementData scanResp;

  advData.setFlags(0x04);  // BR/EDR not supported
  // langsung gunakan manufacturer data
  advData.setManufacturerData(oBeacon.getData());

  pAdvertising->setAdvertisementData(advData);
  pAdvertising->setScanResponseData(scanResp);

  pAdvertising->setMinInterval(0x20);
  pAdvertising->setMaxInterval(0x40);

  pAdvertising->start();
  Serial.println("Beacon started...");
}

void loop() {
}
