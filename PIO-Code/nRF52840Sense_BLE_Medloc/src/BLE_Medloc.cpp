/*********************************************************************
 This is MEDLOC WEARABLE BLUETOOTH DEVICE with Power Button, Enjoy!!
*********************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>
#include <Adafruit_MLX90614.h>
#include <DFRobot_Heartrate.h>

// Pin definitions
#define POWER_BUTTON_PIN 0
#define HEARTRATE_PIN 8

// Power management
bool deviceOn = false;
bool lastButtonState = HIGH;
unsigned long buttonPressTime = 0;
bool buttonPressed = false;
const unsigned long holdTimeToOff = 2000;  // Hold for 2 seconds to turn off

/* HRM Service Definitions
 * Heart Rate Monitor Service:  0x180D
 * Heart Rate Measurement Char: 0x2A37
 * Body Sensor Location Char:   0x2A38
 */
BLEService        hrms = BLEService(UUID16_SVC_HEART_RATE);
BLECharacteristic hrmc = BLECharacteristic(UUID16_CHR_HEART_RATE_MEASUREMENT);
BLECharacteristic bslc = BLECharacteristic(UUID16_CHR_BODY_SENSOR_LOCATION);

/* HT Service Definitions
 * Health Thermometer Service:  0x1809
 * Temperature Measurement Char: 0x2A1C
 * Temperature Type Char:       0x2A1D
 */
BLEService        hts = BLEService(UUID16_SVC_HEALTH_THERMOMETER);
BLECharacteristic tempm = BLECharacteristic(UUID16_CHR_TEMPERATURE_MEASUREMENT);
BLECharacteristic temptype = BLECharacteristic(UUID16_CHR_TEMPERATURE_TYPE);

BLEDis bledis;    // DIS (Device Information Service) helper class instance
BLEBas blebas;    // BAS (Battery Service) helper class instance

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
DFRobot_Heartrate heartrate(DIGITAL_MODE);   // Using DIGITAL_MODE for pin D0

// identifier
char bname[] = "Medloc-Wearable";
char manu[] = "Mikrojos Corp.";
char model[] = "Medloc-Charlie";
char fwver[] = "1.3.0";

uint8_t bps = 0;
uint8_t batteryLevel = 100; // start full battery

void connect_callback(uint16_t conn_handle) {
  BLEConnection* connection = Bluefruit.Connection(conn_handle);
  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));
  Serial.print("Connected to ");
  Serial.println(central_name);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void) conn_handle;
  (void) reason;
  Serial.print("Disconnected, reason = 0x"); 
  Serial.println(reason, HEX);
  Serial.println("Advertising!");
}

void cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value) {
  Serial.print("CCCD Updated: ");
  Serial.print(cccd_value);
  Serial.println("");

  if (chr->uuid == hrmc.uuid) {
    if (chr->notifyEnabled(conn_hdl)) {
      Serial.println("Heart Rate Measurement 'Notify' enabled");
    } else {
      Serial.println("Heart Rate Measurement 'Notify' disabled");
    }
  }
  if (chr->uuid == tempm.uuid) {
    if (chr->notifyEnabled(conn_hdl)) {
      Serial.println("Temperature Measurement 'Notify' enabled");
    } else {
      Serial.println("Temperature Measurement 'Notify' disabled");
    }
  }
}

void startAdv(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(hrms);
  Bluefruit.Advertising.addService(hts);
  Bluefruit.Advertising.addName();
  
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
}

void setupHRM(void) {
  hrms.begin();

  hrmc.setProperties(CHR_PROPS_NOTIFY);
  hrmc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  hrmc.setFixedLen(2);
  hrmc.setCccdWriteCallback(cccd_callback);
  hrmc.begin();
  uint8_t hrmdata[2] = { 0b00000110, 0x40 };
  hrmc.write(hrmdata, 2);

  bslc.setProperties(CHR_PROPS_READ);
  bslc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  bslc.setFixedLen(1);
  bslc.begin();
  bslc.write8(2);    // Set to 'Wrist' (2)
}

void setupHTS(void) {
  hts.begin();

  tempm.setProperties(CHR_PROPS_NOTIFY);
  tempm.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  tempm.setFixedLen(5);
  tempm.begin();

  temptype.setProperties(CHR_PROPS_READ);
  temptype.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  temptype.setFixedLen(1);
  temptype.begin();
  temptype.write8(2);
}

void powerOn() {
  Serial.println("\n=== POWERING ON ===");
  deviceOn = true;
  
  // Initialize Bluefruit
  Bluefruit.begin();
  Bluefruit.setName(bname);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Configure services
  bledis.setManufacturer(manu);
  bledis.setModel(model);
  bledis.setFirmwareRev(fwver);
  bledis.begin();

  blebas.begin();
  blebas.write(batteryLevel);

  setupHRM();
  setupHTS();
  startAdv();

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Device ON - Advertising");
}

void powerOff() {
  Serial.println("\n=== POWERING OFF ===");
  deviceOn = false;
  
  // Stop advertising and disconnect
  Bluefruit.Advertising.stop();
  if (Bluefruit.connected()) {
    Bluefruit.disconnect(Bluefruit.connHandle());
  }
  
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  Serial.println("Device OFF");
}

void checkPowerButton() {
  int reading = digitalRead(POWER_BUTTON_PIN);
  
  // Button pressed (LOW)
  if (reading == LOW && lastButtonState == HIGH) {
    buttonPressTime = millis();
    buttonPressed = true;
    
    // If device is OFF, turn it ON with a quick press
    if (!deviceOn) {
      powerOn();
      buttonPressed = false;  // Prevent holding check after power on
    }
  }
  
  // Button released (HIGH)
  if (reading == HIGH && lastButtonState == LOW) {
    buttonPressed = false;
  }
  
  // Check if button is being held while device is ON
  if (deviceOn && buttonPressed && reading == LOW) {
    unsigned long holdDuration = millis() - buttonPressTime;
    
    // Visual feedback while holding
    if (holdDuration > 500) {
      // Blink LED faster to indicate hold is being registered
      if ((millis() / 100) % 2 == 0) {
        digitalWrite(LED_RED, LOW);
      } else {
        digitalWrite(LED_RED, HIGH);
      }
    }
    
    // Turn off after holding for specified time
    if (holdDuration >= holdTimeToOff) {
      powerOff();
      buttonPressed = false;
    }
  }
  
  lastButtonState = reading;
}

void setup() {
  Serial.begin(9600);
  
  // Setup power button
  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);  
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  Serial.println("-----------------------");
  Serial.println("Medloc Wearable v1.3.0");
  Serial.println("-----------------------\n");

  // Check MLX90614
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX90614 sensor. Check wiring.");
    while (1);
  } else {
    Serial.println("MLX90614 sensor connected.");
    Serial.print("Emissivity set to: ");
    Serial.println(mlx.readEmissivity());
  }

  Serial.println("\nPress power button (D10) to turn ON");
  Serial.println("Hold button for 2 seconds to turn OFF");
  Serial.println("Device is OFF - Waiting for power button...");
}

void loop() {
  // Always check power button
  checkPowerButton();
  
  // Only run main functions if device is ON
  if (!deviceOn) {
    delay(100);
    return;
  }
  
  // Toggle LED to show activity
  digitalToggle(LED_RED);
  
  if (Bluefruit.connected()) {
    // Read heart rate from DFRobot sensor
    heartrate.getValue(HEARTRATE_PIN);
    uint8_t rateValue = heartrate.getRate();
    
    // Use sensor value if available, otherwise keep last value
    if (rateValue > 0 && rateValue < 220) {  // Valid heart rate range
      bps = rateValue;
    }

    uint8_t hrmdata[2] = {0b00000110, bps};
    
    if (hrmc.notify(hrmdata, sizeof(hrmdata))) {
      Serial.print("Heart Rate: ");
      Serial.print(bps);
      Serial.println(" BPM");
    } else {
      Serial.println("ERROR: HR Notify failed!");
    }

    // Read temperature from MLX90614 sensor
    float tempC = mlx.readObjectTempC();

    // Encode as IEEE-11073 FLOAT
    int32_t mantissa = (int32_t)(tempC * 100.0f);
    int8_t exponent = -2;

    uint8_t tempPayload[5];
    tempPayload[0] = 0x00;
    tempPayload[1] = (uint8_t)(mantissa & 0xFF);
    tempPayload[2] = (uint8_t)((mantissa >> 8) & 0xFF);
    tempPayload[3] = (uint8_t)((mantissa >> 16) & 0xFF);
    tempPayload[4] = (uint8_t)exponent;

    if (tempm.notify(tempPayload, sizeof(tempPayload))) {
      Serial.print("Temperature: ");
      Serial.print(tempC);
      Serial.println(" °C");
    } else {
      Serial.println("ERROR: Temp notify failed!");
    }
  }

  delay(1000);
}