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

// BLE objects
BLEServer* pServer;
BLECharacteristic* pHRChar;
BLECharacteristic* pBatteryChar;

bool deviceConnected = false;
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
  )->setValue("MEDLOC-PT00");

  disService->createCharacteristic(
    FIRMWARE_REVISION_UUID,
    BLECharacteristic::PROPERTY_READ
  )->setValue("1.0.0");

  disService->start();

  // ---- Start Advertising ----
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(HEART_RATE_SERVICE_UUID);
  pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
  pAdvertising->addServiceUUID(DEVICE_INFO_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("Heart Rate Monitor is advertising...");
}

void loop() {
  if (deviceConnected && millis() - lastUpdate > 1000) {
    lastUpdate = millis();

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

    // Simulate battery drain every 10s
    if (millis() % 10000 < 1000 && batteryLevel > 0) {
      batteryLevel--;
      pBatteryChar->setValue(&batteryLevel, 1);
      pBatteryChar->notify();
      Serial.printf("Battery = %d%%\n", batteryLevel);
    }
  }
}
