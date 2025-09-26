/*
  ESP32 BLE Heart Rate Monitor (HRS example)
  - Service: Heart Rate Service (0x180D)
    - Heart Rate Measurement (0x2A37) [Notify]
    - Body Sensor Location (0x2A38) [Read]
  - Service: Battery Service (0x180F)
    - Battery Level (0x2A19) [Read/Notify]
  - Service: Device Information Service (0x180A)
    - Manufacturer Name, Model, Firmware version
*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs
#define HEART_RATE_SERVICE_UUID        BLEUUID((uint16_t)0x180D)
#define HEART_RATE_MEASUREMENT_UUID    BLEUUID((uint16_t)0x2A37)
#define BODY_SENSOR_LOCATION_UUID      BLEUUID((uint16_t)0x2A38)

#define BATTERY_SERVICE_UUID           BLEUUID((uint16_t)0x180F)
#define BATTERY_LEVEL_UUID             BLEUUID((uint16_t)0x2A19)

#define DEVICE_INFO_SERVICE_UUID       BLEUUID((uint16_t)0x180A)
#define MANUFACTURER_NAME_UUID         BLEUUID((uint16_t)0x2A29)
#define MODEL_NUMBER_UUID              BLEUUID((uint16_t)0x2A24)
#define FIRMWARE_REVISION_UUID         BLEUUID((uint16_t)0x2A26)
#define SERIAL_NUMBER_UUID             BLEUUID((uint16_t)0x2A25)

#define RAW_DATA_SERVICE_UUID          "0d5dfa4b-4a3c-4b1d-97d1-7f434944d7e0"
#define RAW_PPG_CHAR_UUID              "4e24c3eb-5a3d-47b8-8e36-886443a17099"
#define RAW_TEMP_CHAR_UUID             "26d2f394-df93-490a-b380-5a33d2709b50"
#define RAW_ACC_CHAR_UUID              "7bb89974-520b-4c09-80f6-ed73e218692f"
#define RAW_GYRO_CHAR_UUID             "06c8523b-09ac-4817-8fc5-6d79e2f9536a"
#define RAW_MAG_CHAR_UUID              "aaa3de1c-be4a-454d-9417-1603c85927c1"

#define INDOOR_POSITIONING_SERVICE_UUID "7c52afe7-f7d4-471e-a36b-45026e261027"
#define BEACON_COUNT_CHAR_UUID    "c9dafa1a-996b-412f-8371-f49d370d0b2f"
#define BEACON_DATA_CHAR_UUID     "84407d0b-fc3c-4c13-b758-60cd7a0e4c0a"

// BLE objects
BLEServer* pServer;
BLECharacteristic* pHRChar;
BLECharacteristic* pBatteryChar;
BLECharacteristic* pRawPPGChar;
BLECharacteristic* pRawTempChar;
BLECharacteristic* pRawAccChar;
BLECharacteristic* pRawGyroChar;
BLECharacteristic* pRawMagChar;
BLECharacteristic* pBeaconCountChar;
BLECharacteristic* pBeaconDataChar;
// BLEDescriptor* pBatteryDesc;
// BLEDescriptor* pRawMagDesc;
// BLEDescriptor* pRawGyroDesc;
// BLEDescriptor* pRawAccDesc;
// BLEDescriptor* pRawTempDesc;
// BLEDescriptor* pRawPPGDesc;
// BLEDescriptor* pBeaconCountDesc;
// BLEDescriptor* pBeaconDataDesc;

bool deviceConnected = false;
uint8_t ppgData[2] = {0, 0}; // Placeholder for PPG data
uint8_t tempData[2] = {0, 0}; // Placeholder for Temp data
uint8_t accData[6] = {0, 0, 0, 0, 0, 0}; // Placeholder for Acc data
uint8_t gyroData[6] = {0, 0, 0, 0, 0, 0}; // Placeholder for Gyro data
uint8_t magData[6] = {0, 0, 0,  0, 0, 0}; // Placeholder for Mag data
uint8_t heartRate = 75;     // initial HR
uint8_t batteryLevel = 100; // start full battery
unsigned long lastUpdate = 0;

class ServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Client connected");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Client disconnected");
    BLEDevice::startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32 Medloc");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // ---- Heart Rate Service ----
  BLEService* hrService = pServer->createService(HEART_RATE_SERVICE_UUID);

  // Heart Rate Measurement (Notify)
  pHRChar = hrService->createCharacteristic(
    HEART_RATE_MEASUREMENT_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pHRChar->addDescriptor(new BLE2902()); // enable notifications

  // Body Sensor Location (Read) - 1 = Chest, 2 = Wrist, etc.
  BLECharacteristic* bodyLoc = hrService->createCharacteristic(
    BODY_SENSOR_LOCATION_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  uint8_t sensorLoc = 2; // wrist
  bodyLoc->setValue(&sensorLoc, 1);

  hrService->start();

  // ---- Battery Service ----
  BLEService* battService = pServer->createService(BATTERY_SERVICE_UUID);

  pBatteryChar = battService->createCharacteristic(
    BATTERY_LEVEL_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  // pBatteryDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901)); // User Description
  // pBatteryDesc->setValue("Battery Level 0-100%");  
  // pBatteryChar->addDescriptor(pBatteryDesc);

  pBatteryChar->addDescriptor(new BLE2902());
  pBatteryChar->setValue(&batteryLevel, 1);

  battService->start();

  // ---- Device Info Service ----
  BLEService* disService = pServer->createService(DEVICE_INFO_SERVICE_UUID);

  disService->createCharacteristic(
    MANUFACTURER_NAME_UUID,
    BLECharacteristic::PROPERTY_READ
  )->setValue("MIKROJOS Corp.");

  disService->createCharacteristic(
    MODEL_NUMBER_UUID,
    BLECharacteristic::PROPERTY_READ
  )->setValue("MEDLOC-PT01");

  disService->createCharacteristic(
    SERIAL_NUMBER_UUID,
    BLECharacteristic::PROPERTY_READ
  )->setValue("5F9B34FB");

  disService->createCharacteristic(
    FIRMWARE_REVISION_UUID,
    BLECharacteristic::PROPERTY_READ
  )->setValue("1.0.0");

  disService->start();

  // ---- Raw Data Service ----
  BLEService* rawService = pServer->createService(RAW_DATA_SERVICE_UUID);

  BLECharacteristic* pRawPPGChar = rawService->createCharacteristic(
    RAW_PPG_CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );

  // pRawPPGDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  // pRawPPGDesc->setValue("Raw PPG Data");
  // pRawPPGChar->addDescriptor(pRawPPGDesc);

  pRawPPGChar->addDescriptor(new BLE2902());

  BLECharacteristic* pRawTempChar = rawService->createCharacteristic( 
    RAW_TEMP_CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  // pRawTempDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  // pRawTempDesc->setValue("Raw Temperature Data");
  // pRawTempChar->addDescriptor(pRawTempDesc);

  pRawTempChar->addDescriptor(new BLE2902());

  BLECharacteristic* pRawAccChar = rawService->createCharacteristic(
    RAW_ACC_CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  // pRawAccDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  // pRawAccDesc->setValue("Raw Accelerometer Data");
  // pRawAccChar->addDescriptor(pRawAccDesc);

  pRawAccChar->addDescriptor(new BLE2902());

  BLECharacteristic* pRawGyroChar = rawService->createCharacteristic(
    RAW_GYRO_CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  // pRawGyroDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  // pRawGyroDesc->setValue("Raw Gyroscope Data");
  // pRawGyroChar->addDescriptor(pRawGyroDesc);

  pRawGyroChar->addDescriptor(new BLE2902());

  BLECharacteristic* pRawMagChar =  rawService->createCharacteristic(
    RAW_MAG_CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  // pRawMagDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  // pRawMagDesc->setValue("Raw Magnetometer Data");
  // pRawMagChar->addDescriptor(pRawMagDesc);

  pRawMagChar->addDescriptor(new BLE2902());

  rawService->start();

  // ---- Indoor Positioning Service ----
  BLEService* ipService = pServer->createService(INDOOR_POSITIONING_SERVICE_UUID);
  pBeaconCountChar = ipService->createCharacteristic(
    BEACON_COUNT_CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  // pBeaconCountDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  // pBeaconCountDesc->setValue("Beacon Count");
  // pBeaconCountChar->addDescriptor(pBeaconCountDesc);

  pBeaconCountChar->addDescriptor(new BLE2902());

  pBeaconDataChar = ipService->createCharacteristic(
    BEACON_DATA_CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  // pBeaconDataDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  // pBeaconDataDesc->setValue("Beacon Data");
  // pBeaconDataChar->addDescriptor(pBeaconDataDesc);

  pBeaconDataChar->addDescriptor(new BLE2902());

  // ---- Start Advertising ----
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(HEART_RATE_SERVICE_UUID);
  pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
  pAdvertising->addServiceUUID(DEVICE_INFO_SERVICE_UUID);
  pAdvertising->addServiceUUID(RAW_DATA_SERVICE_UUID);
  pAdvertising->addServiceUUID(INDOOR_POSITIONING_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("Heart Rate Monitor is advertising...");
}

void loop() {
  if (deviceConnected && millis() - lastUpdate > 1000) {
    lastUpdate = millis();

    // Simulate PPG data and notify (not implemented here)
    float ppgValue = 120.33; // Placeholder for actual PPG value
    uint16_t ppgInt = (uint16_t)(ppgValue * 100); // Convert to integer (e.g., 12033 for 120.33 bpm)
    ppgData[0] = (uint8_t)(ppgInt & 0xFF);
    ppgData[1] = (uint8_t)((ppgInt >> 8) & 0xFF);
    pRawPPGChar->setValue(ppgData, 2);
    pRawPPGChar->notify();

    Serial.printf("Notify PPG = %.2f\n", ppgValue);

    // Simulate Temperature data and notify (not implemented here)
    float tempValue = 36.5; // Placeholder for actual Temperature value
    uint16_t tempInt = (uint16_t)(tempValue * 100); // Convert to integer (e.g., 3650 for 36.5°C)
    tempData[0] = (uint8_t)(tempInt & 0xFF);
    tempData[1] = (uint8_t)((tempInt >> 8) & 0xFF);
    pRawTempChar->setValue(tempData, 2);
    pRawTempChar->notify();

    Serial.printf("Notify Temp = %.2f C\n", tempValue);

    // Simulate Accelerometer data and notify (not implemented here)
    int accRandom = random(0, 255); // Simulate Acc value
    for (int i = 0; i < 6; i++) { 
      accData[i] = accRandom;  // Simulate Acc value
    } 
    pRawAccChar->setValue(accData, 6);
    pRawAccChar->notify();

    Serial.printf("Notify Acc = %d\n", accRandom);

    // Simulate Gyroscope data and notify (not implemented here)
    for (int i = 0; i < 6; i++) { 
      gyroData[i] = random(0, 128);  // Simulate Gyro value
    }
    pRawGyroChar->setValue(gyroData, 6);
    pRawGyroChar->notify();

    Serial.printf("Notify Gyro = ");
    for (int i = 0; i < 6; i++) {
      Serial.printf("%d ", gyroData[i]);
    }
    Serial.println();

    // Simulate Magnetometer data and notify (not implemented here)
    for (int i = 0; i < 6; i++) { 
      magData[i] = random(128, 255);  // Simulate Mag value
    }
    pRawMagChar->setValue(magData, 6);
    pRawMagChar->notify();

    Serial.printf("Notify Mag = ");
    for (int i = 0; i < 6; i++) {
      Serial.printf("%d ", magData[i]);
    }
    Serial.println();

    // Simulate heart rate (random walk around 70–100 bpm)
    heartRate += (int8_t)random(-2, 3);
    if (heartRate < 60) heartRate = 60;
    if (heartRate > 160) heartRate = 160;

    // HR Measurement format:
    // [Flags:1][HeartRateValue:1 or 2 bytes][Optional fields...]
    uint8_t hrm[2];
    hrm[0] = 0x00;        // Flags = 0 (uint8 bpm only)
    hrm[1] = heartRate;   // bpm
    pHRChar->setValue(hrm, 2);
    pHRChar->notify();

    Serial.printf("Notify HR = %d bpm\n", heartRate);

    // Simulate battery drain every 20s
    if (millis() % 20000 < 1000 && batteryLevel > 0) {
      batteryLevel--;
      pBatteryChar->setValue(&batteryLevel, 1);
      pBatteryChar->notify();
      Serial.printf("Battery = %d%%\n", batteryLevel);
    }
  }
}
