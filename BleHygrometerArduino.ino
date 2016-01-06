#include <SPI.h>
#include <BLEPeripheral.h>
#include <Wire.h>
#include <HIH6130.h>

// define pins
#define BLE_REQ 8
#define BLE_RDY 7
#define BLE_RST 6

// prepare BLE objects
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);
BLEService service = BLEService("fff0");
BLEFloatCharacteristic characteristic = BLEFloatCharacteristic("fff1", BLENotify);
BLEDescriptor descriptor = BLEDescriptor("2901", "Humidity percent");

// init hygrometer object with I2C address
HIH6130 rht(0x27);

void setup() {

  blePeripheral.setLocalName("Arduino");
  blePeripheral.setAdvertisedServiceUuid(service.uuid());
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(characteristic);
  blePeripheral.addAttribute(descriptor);

  rht.begin();
  readHumidity();
  blePeripheral.begin();

}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    unsigned long updatedAt = millis();
    while (central.connected()) {
      if (millis() >= updatedAt + 300) {
        readHumidity();
        updatedAt = millis();
      }
    }
  }
}

void readHumidity() {
  rht.readRHT();
  characteristic.setValue((float)rht.humidity);
}

