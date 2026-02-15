#include <SoftwareSerial.h>
#include <R4HttpClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include "Website.h"

WiFiServer server(80);
SoftwareSerial mySerial(10, 11);

// btn, led, state, lstState
// int config[4] = {7, 6, LOW, LOW};
// bool isConfig = true;

bool once = true;
bool hasError = false;

int disFromWLevel = 100;

const int baudRate = 9600;

char phoneNumbers[10][11];

void setup() {
  // pinMode(config[0], INPUT_PULLUP);
  // pinMode(config[1], OUTPUT);

  Serial.begin(baudRate);
  mySerial.begin(baudRate);
  while (!Serial);

  if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
    
  if (WiFi.status() == WL_NO_MODULE) {
    handleError("Communication with WiFi module failed!");
  }

  // isConfig = EEPROM.read(0);
  for (int i = 0; i < 10; i++) {
    handleNumbers(i+1);
  }
}

void loop() {
  // handleMode();
  configMode();
  getWaterLevel();
  alarmSystem();

  Serial.println(disFromWLevel);
  once = false;
}