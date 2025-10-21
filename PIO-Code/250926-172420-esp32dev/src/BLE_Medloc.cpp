/*********************************************************************
 This is MEDLOC WEARABLE BLUETOOTH DEVICE, Enjoy!!
*********************************************************************/
#include <Arduino.h>
#include <LSM6DS3.h>
#include <Wire.h>
#include <bluefruit.h>
#include <Adafruit_MLX90614.h>

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

// identifier
char bname[] = "Medloc-Wearable";
char manu[] = "Mikrojos Corp.";
char model[] = "Medloc-Charlie";
char fwver[] = "1.2.0";

uint8_t bps = 0;
uint8_t heartRate = 75;     // initial HR
uint8_t batteryLevel = 100; // start full battery

void connect_callback(uint16_t conn_handle) {
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
  Serial.println("Advertising!");
}

void cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value) {
    // Display the raw request packet
    Serial.print("CCCD Updated: ");
    //Serial.printBuffer(request->data, request->len);
    Serial.print(cccd_value);
    Serial.println("");

    // Check the characteristic this CCCD update is associated with in case
    // this handler is used for multiple CCCD records.
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

void startAdv(void){  // Setup and start advertising
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include HRM Service UUID
  Bluefruit.Advertising.addService(hrms);

  // include HT Service UUID
  Bluefruit.Advertising.addService(hts);

  // Include Name
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void setupHRM(void){  // Configure the Heart Rate Monitor service
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.heart_rate.xml
  // Supported Characteristics:
  // Name                         UUID    Requirement Properties
  // ---------------------------- ------  ----------- ----------
  // Heart Rate Measurement       0x2A37  Mandatory   Notify
  // Body Sensor Location         0x2A38  Optional    Read
  // Heart Rate Control Point     0x2A39  Conditional Write       <-- Not used here
  hrms.begin();

  // Note: You must call .begin() on the BLEService before calling .begin() on
  // any characteristic(s) within that service definition.. Calling .begin() on
  // a BLECharacteristic will cause it to be added to the last BLEService that
  // was 'begin()'ed!

  // Configure the Heart Rate Measurement characteristic
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xml
  // Properties = Notify
  // Min Len    = 1
  // Max Len    = 8
  //    B0      = UINT8  - Flag (MANDATORY)
  //      b5:7  = Reserved
  //      b4    = RR-Internal (0 = Not present, 1 = Present)
  //      b3    = Energy expended status (0 = Not present, 1 = Present)
  //      b1:2  = Sensor contact status (0+1 = Not supported, 2 = Supported but contact not detected, 3 = Supported and detected)
  //      b0    = Value format (0 = UINT8, 1 = UINT16)
  //    B1      = UINT8  - 8-bit heart rate measurement value in BPM
  //    B2:3    = UINT16 - 16-bit heart rate measurement value in BPM
  //    B4:5    = UINT16 - Energy expended in joules
  //    B6:7    = UINT16 - RR Internal (1/1024 second resolution)
  hrmc.setProperties(CHR_PROPS_NOTIFY);
  hrmc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  hrmc.setFixedLen(2);
  hrmc.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  hrmc.begin();
  uint8_t hrmdata[2] = { 0b00000110, 0x40 }; // Set the characteristic to use 8-bit values, with the sensor connected and detected
  hrmc.write(hrmdata, 2);

  // Configure the Body Sensor Location characteristic
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.body_sensor_location.xml
  // Properties = Read
  // Min Len    = 1
  // Max Len    = 1
  //    B0      = UINT8 - Body Sensor Location
  //      0     = Other
  //      1     = Chest
  //      2     = Wrist
  //      3     = Finger
  //      4     = Hand
  //      5     = Ear Lobe
  //      6     = Foot
  //      7:255 = Reserved
  bslc.setProperties(CHR_PROPS_READ);
  bslc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  bslc.setFixedLen(1);
  bslc.begin();
  bslc.write8(2);    // Set the characteristic to 'Wrist' (2)
}

void setupHTS(void){  // Configure the Health Thermometer service
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.health_thermometer.xml
  // Supported Characteristics:
  // Name                         UUID    Requirement Properties
  // ---------------------------- ------  ----------- ----------
  // Temperature Measurement      0x2A1C  Mandatory   Indicate
  // Temperature Type             0x2A1D  Optional    Read
  hts.begin();

  // Configure the Temperature Measurement characteristic
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.temperature_measurement.xml
  // Properties = Indicate
  // Min Len    = 5
  // Max Len    = 25
  //    B0      = UINT8   - Flag (MANDATORY)
  //      b7    = Time Stamp Present (0 = Not present, 1 = Present)
  //      b6    = Temperature Type Present (0 = Not present, 1 = Present)
  //      b5    = Reserved
  //      b4    = Reserved
  //      b3    = Reserved
  //      b2    = Unit (0 = Celsius, 1 = Fahrenheit)
  //      b1    = Value Format (0 = FLOAT, 1 = SFLOAT)
  //      b0    = Temperature Measurement Value Present (Always 1)
  //    B1:B4   = FLOAT   - Temperature Measurement Value
  //    B5:B10  = UINT32  - Time Stamp (OPTIONAL)
  //    B11     = UINT8   - Temperature Type (OPTIONAL)
  tempm.setProperties(CHR_PROPS_NOTIFY);
  tempm.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  tempm.setFixedLen(5);
  tempm.begin();

  // Configure the Temperature Type characteristic
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.temperature_type.xml
  // Properties = Read
  // Min Len    = 1
  // Max Len    = 1
  //    B0      = UINT8 - Temperature Type
  //      0     = Armpit
  //      1     = Body (general)
  //      2
  temptype.setProperties(CHR_PROPS_READ);
  temptype.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  temptype.setFixedLen(1);
  temptype.begin();
  temptype.write8(2);    // Set the characteristic to 'Body (general)' (1)
}

void setup()
{
  Serial.begin(9600);
  while ( !Serial ) delay(10);   // for enabling when serial available

  // checking MLX90614 
  if ( !mlx.begin() ) {
    Serial.println("Error connecting to MLX90614 sensor. Check wiring.");
    while (1);
  }
  else {
    Serial.println("MLX90614 sensor connected.");
    Serial.print("Emissivity set to: "); Serial.println(mlx.readEmissivity());
  }
  Serial.println("-----------------------\n");

  Serial.println("Bluefruit52 Begin");
  Serial.println("-----------------------\n");

  // Initialise the Bluefruit module
  Serial.println("Initialise the Bluefruit nRF52 module");
  Bluefruit.begin();
  Bluefruit.setName(bname);

  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Configure and Start the Device Information Service
  Serial.println("Configuring the Device Information Service");
  bledis.setManufacturer(manu);
  bledis.setModel(model);
  bledis.setFirmwareRev(fwver);
  bledis.begin();

  // Start the BLE Battery Service and set it to 100%
  Serial.println("Configuring the Battery Service");
  blebas.begin();
  blebas.write(batteryLevel);

  // Setup the Heart Rate Monitor service using
  // BLEService and BLECharacteristic classes
  Serial.println("Configuring the Heart Rate Monitor Service");
  setupHRM();

  // Setup the Health Thermometer service using
  // BLEService and BLECharacteristic classes
  Serial.println("Configuring the Health Thermometer Service");
  setupHTS();

  // Setup the advertising packet(s)
  Serial.println("Setting up the advertising payload(s)");
  startAdv();

  Serial.println("Ready Player One!!!");
  Serial.println("\nAdvertising");
}

void loop()
{
  digitalToggle(LED_RED);
  
  if ( Bluefruit.connected() ) {
    bps += (int8_t)random(-2, 3);
    if (bps < 60) bps = 60;
    if (bps > 160) bps = 160;

    uint8_t hrmdata[2] = {0b00000110, bps};           // Sensor connected, increment BPS value
    
    // Note: We use .notify instead of .write!
    // If it is connected but CCCD is not enabled
    // The characteristic's value is still updated although notification is not sent
    if ( hrmc.notify(hrmdata, sizeof(hrmdata)) ){
      Serial.print("Heart Rate Measurement updated to: "); Serial.println(bps); 
    }else{
      Serial.println("ERROR: Notify not set in the CCCD or not connected!");
    }
    // Read temperature from MLX90614 sensor
    float tempC = mlx.readObjectTempC();
    // float tempC = 36.0 + (random(0, 150) / 100.0f); // e.g., 36.00 .. 37.49

    // Encode as IEEE-11073 FLOAT (32-bit): mantissa (24-bit), exponent (8-bit)
    // We'll use exponent = -2 so mantissa = temp * 100 (e.g., 3650 for 36.50)
    int32_t mantissa = (int32_t)(tempC * 100.0f);
    int8_t exponent = -2;

    uint8_t tempPayload[5];
    tempPayload[0] = 0x00; // Flags: 0 => Celsius, no time, no type
    tempPayload[1] = (uint8_t)(mantissa & 0xFF);
    tempPayload[2] = (uint8_t)((mantissa >> 8) & 0xFF);
    tempPayload[3] = (uint8_t)((mantissa >> 16) & 0xFF);
    tempPayload[4] = (uint8_t)exponent; // signed 8-bit exponent

    if ( tempm.notify(tempPayload, sizeof(tempPayload)) ) {
      Serial.print("Temperature notified: ");
      Serial.print(tempC);
      Serial.println(" C");
    } else {
      Serial.println("ERROR: Temperature notify failed (CCCD not enabled?)");
    }
  }

  // Only send update once per second
  delay(1000);
}