// Libraries and Other Resources
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Wire.h>
#include "WiFiS3.h"
#include "Website.h"

// Initializing Servers
WiFiServer accessPoint(80);
WiFiClient client;

const int baudRate = 9600;
const char server[] = "10.132.185.14";
const int port = 3000;

// States and Cooldowns
int lstMuteState = 0;
int lstMode = 2; 
long muteCooldown = 0;
long smsCooldown = 0;
long wlvlCooldown = 0;
long now = 0;
bool mute = false;
bool onCooldown = false;
bool once = true;

// Defining Pin Connections
const int speakerPin = 8;
const int mutePin = 7;
const int echoPin = 13;
const int switchPin = 2;

// Water Level Mode Parameters
const int requiredReadings = 50;
int readings[requiredReadings][2];
int numReadings = 0;
int numUnique = 0;

int maxCount = 0;
int maxValue = 0;

long savedMill = 0; 

// Distance of Sensor from the Water Level
const int threshold = 12;
const int bottom = 54;
int waterLevel = 0;

// Saved Info Storage Variables
char phoneNumbers[10][11] = {
  "0000000000",
  "0000000000",
  "0000000000",
  "0000000000",
  "0000000000",
  "0000000000",
  "0000000000",
  "0000000000",
  "0000000000",
  "0000000000"
};
char ssid[33] = "";
char pass[64] = "";


void setup() {
  pinMode(echoPin, INPUT);
  pinMode(switchPin, INPUT);
  pinMode(mutePin, INPUT);

  // Serial Monitor Set-up for Depugging
  Serial.begin(baudRate);
  while (!Serial);


  // WiFi Integration Checks
  if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  if (WiFi.status() == WL_NO_MODULE) {
    handleError("Communication with WiFi module failed!");
  }

  // Loads all Saved Information on Storage Variables
  for (int i = 1; i < 11; i++) {
    handleNumbers(i);
  }
}

void loop() {
  int currMode = digitalRead(switchPin);

  if (lstMode != currMode) {
    lstMode = currMode;

    Serial.println("Switching Mode...");
    WiFi.disconnect();
    delay(1000);

    if (currMode) {
      while(WiFi.begin(ssid, pass) != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      Serial.println("Connected To WiFi");
      delay(1000);

    } else {
      // Host Wifi
      WiFi.beginAP("ALIGMAT", "Password");

      delay(5000);
      accessPoint.begin();

      Serial.println("Access Point Set-up");
    }
  }

  now = millis();
  if (currMode && now - wlvlCooldown > 2000) {
    if (client.connect(server, port)) {
      wlvlCooldown = now;
      String JsonPackage = " { \"water_level\": ";
      JsonPackage += String(waterLevel);
      JsonPackage += " }";

      client.println("POST /water-level HTTP/1.1");
      client.print("HOST: ");
      client.println(server);
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(JsonPackage.length());
      client.println("Connection: close");
      client.println();
      client.println(JsonPackage);

      client.stop();
    } else {
      Serial.println("Connection Failed!");
    }
  } else if (!currMode) {
    WiFiClient serverClient = accessPoint.available();

    // Check for Server Client
    if (serverClient) {
      // Parsed Data Placeholders
      String currentLine = "";
      String request = "";

      // Loop when Client is connected
      while (serverClient.connected()) {
        delayMicroseconds(10); // 10 Microsecond buffer
        if (!serverClient.available()) continue; // Continue to next loop when Server Client is Unavailable

        // Collect Sent-Over Data Characters
        char c = serverClient.read();
        if (c != '\n') {
          if (c != '\r') currentLine += c;
          continue;
        }

        // Define Request Type
        if (currentLine.startsWith("GET") || currentLine.startsWith("POST"))
          request = currentLine;

        if (currentLine.length() == 0) {
          handleRequest(serverClient, request);
    
          break;
        } else {
          currentLine = "";
        }
      }

      serverClient.stop();
    }
  }

  // Water Level Detection Runs Every 100 Milliseconds
  long currMill = millis();
  if (currMill - savedMill > 100) {
    savedMill = currMill;
    const int distance = pulseIn(echoPin, HIGH) * 0.034 / 2;
    
    // Retrieves Modal Value
    numReadings++;
    for (int i = 0; i < numReadings; i++ && false) {
      // If Value is Already Registered in Current Set of Readings
      if (distance == readings[i][0]) {
        readings[i][1]++;

        // Replaces Max with Current Reading if it is Higher
        if (readings[i][1] > maxCount) {
          maxCount = readings[i][1];
          maxValue = readings[i][0];
        }

      // Else if Value is Unique
      } else if (i+1 == numReadings) {
        numUnique++;

        readings[i][0] = distance;
        readings[i][1] = 1;
      }
    }

    // When Mode is found or if Readings Reached Max
    if (maxCount > requiredReadings/numUnique || numReadings == requiredReadings) {
      waterLevel = maxValue + 3; // Updates Current the Sensor's Distance from the Water Level
      waterLevel = bottom - waterLevel;
      if (waterLevel < 0) waterLevel = 0;

      // Reset all parameters to prepare for next set
      maxValue = 0;
      numReadings = 0;
      maxCount = 0;
    }
  };

  // Alarm System
  now = millis();
  if (onCooldown && now - smsCooldown > 60000) {
    onCooldown = false;
  }

  if (waterLevel > threshold) {
    if (!mute) tone(speakerPin, 3000); // Speaker Turns On

    if (currMode && !onCooldown) {
      if (client.connect(server, port)) {
        onCooldown = true;
        smsCooldown = now;

        String JsonPackage = "{ \"numbers\": [ ";
        for (int i = 0; i < 10; i++) {
          if (phoneNumbers[i][0] == '0') continue;
          
          // char num[12];

          // for (int n = 0; n < 9; n++) {
          //   num[n+3] = phoneNumbers[i][n+1];
          // }

          // num[0] = '6';
          // num[1] = '3';
          // num[2] = '9';
          // num[12] = '\0';

          JsonPackage += phoneNumbers[i];
          JsonPackage += ", ";
        }
        JsonPackage.remove(JsonPackage.length() - 2);
        JsonPackage += " ], \"water_level\": ";
        JsonPackage += String(waterLevel);
        JsonPackage += " }";

        client.println("POST /send-sms HTTP/1.1");
        client.print("HOST: ");
        client.println(server);
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(JsonPackage.length());
        client.println("Connection: close");
        client.println();
        client.println(JsonPackage);

        client.stop();
      } else {
        Serial.println("Connection Failed!");
      }
    }
  } else noTone(speakerPin);

  // Mute Button
  const int muteState = digitalRead(mutePin);
  if (muteState != lstMuteState) {
    lstMuteState = muteState;

    if (muteState == LOW) {
      noTone(speakerPin);
      muteCooldown = now;
      mute = true;
    }
  }

  // Turn off mute after 50 seconds
  now = millis();
  if (mute && now - muteCooldown > 50000) {
    mute = false;
  }
}

// Updates numbers
void handleNumbers(int coordinate) {
  int hasNum = EEPROM.read(coordinate); // Check if number exists at EEPROM coordinate
  if (!hasNum) {
    phoneNumbers[coordinate-1][0] = '0'; // Returns if no number at coordinate
    return;
  }
  
  phoneNumbers[coordinate-1][0] = '1'; // Cache Number State
  for (int digit = 0; digit < 9; digit++) {
      // Writes Every Digit if Number Exists and 0 if it Doesn't
      phoneNumbers[coordinate-1][digit+1] = EEPROM.read(coordinate*9 + digit + 2);
  }

  // Adds the Terminator at the End (Essential for Every String)
  phoneNumbers[coordinate-1][11] = '\0';

  // Log Changed Numbers
  Serial.println(phoneNumbers[coordinate-1]);
}

void handleWiFi() {
  char _ssid[33];
  char _pass[64];

  int ssidLen = EEPROM.read(120);
  int passLen = EEPROM.read(121);

  for (int i = 0; i < ssidLen; i++) {
    _ssid[i] = EEPROM.read(122 + i);
  }

  for (int i = 0; i < passLen; i++) {
    _pass[i] = EEPROM.read(122 + ssidLen + i);
  }

  _ssid[ssidLen] = '\0';
  _pass[passLen] = '\0';

  strcpy(ssid, _ssid);
  strcpy(pass, _pass);
}

void handleError(const char* msg) {
  Serial.println(msg);

  while(true);
}