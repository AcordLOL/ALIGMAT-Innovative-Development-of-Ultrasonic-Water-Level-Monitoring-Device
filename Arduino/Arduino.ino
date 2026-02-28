// Libraries and Other Resources
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "WiFiS3.h"
#include "Website.h"

// Initializing Servers
WiFiServer server(80);
WiFiClient client;

// Defining Pin Connections
const int baudRate = 9600;
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
const int bottom = 38;
int disFromWLevel = 100;

// Saved Info Storage Variables
char phoneNumbers[10][11];
char ssid[35] = "wifi";
char pass[66] = "password";


// States and Cooldowns
int lstMuteState = 0;
int lstMode = 1; 
long cooldown = 0L;
bool mute = false;
bool once = true;

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
  for (int i = 0; i < 10; i++) {
    handleNumbers(i+1);
  }
}

void loop() {
  int currMode = digitalRead(switchPin);

  if (lastMode != currMode) {
    lastMode = currMode;

    Serial.println("Switching Mode...");
    WiFi.disconnect();
    delay(1000);

    if (currMode) {
      // Login to Wifi
      WiFi.begin(ssid, pass);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      Serial.println("Connected To WiFi");
      delay(1000);

    } else {
      // Host Wifi
      WiFi.beginAP("ALIGMAT", "Password");

      delay(5000);
      server.begin();

      Serial.println("Access Point Set-up");
    }
  }

  if (currMode) {

  } else {
    WiFiClient serverClient = server.available();

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

        // Define request type
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
      disFromWLevel = maxValue + 3; // Updates Current the Sensor's Distance from the Water Level
      Serial.println(disFromWLevel);

      // Reset all parameters to prepare for next set
      maxValue = 0;
      numReadings = 0;
      maxCount = 0;
    }
  };

  // Alarm System
  // disFromWLevel = 5;
  if ((38 - disFromWLevel) > threshold) {
    // if (!mute) tone(speakerPin, 3000); // Speaker Turns On

    Serial.println("sending");
      if (now - cooldown > 5000) {
        StaticJsonDocument<512> doc;
        doc["numbers"] = JsonArray();
        doc["numbers"].add("09916965106");
        doc["numbers"].add("09916965107");
        doc["numbers"].add("09916965108");

        String requestBody;
        serializeJson(doc, requestBody);

        if (client.connect(serverAddress, 3000)) {
          client.println("POST /send-sms HTTP/1.1");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();
          client.println(requestBody);
        }


      } else {
        Serial.println("on cooldown");
      }

  } else {
    noTone(speakerPin);
  }

  // Mute Button
  const int muteState = digitalRead(mutePin);
  const long now = millis();

  if (muteState != lstMuteState) {
    lstMuteState = muteState;

    if (muteState == LOW) {
      noTone(speakerPin);
      cooldown = now;
      mute = true;
    }
  }

  // Turn off mute after 50 seconds
  if (now - cooldown > 50000) {
    mute = false;
  }
}

// Updates numbers
void handleNumbers(int coordinate) {
  int hasNum = EEPROM.read(coordinate); // Check if number exists at EEPROM coordinate
  if (!hasNum) return; // Returns if no number at coordinate

  phoneNumbers[9][0] = hasNum; // Cache Number State
  for (int digit = 0; digit < 9; digit++) {

      // Writes Every Digit if Number Exists and 0 if it Doesn't
      phoneNumbers[coordinate-1][digit+1] = (hasNum) ? EEPROM.read(coordinate*9 + digit + 2) : '0';
  }

  // Adds the Terminator at the End (Essential for Every String)
  phoneNumbers[coordinate-1][11] = '\0';

  Serial.println(phoneNumbers[coordinate-1]);
}

// Error Handler
void handleError(const char* msg) {
  Serial.println(msg);

  while(true);
}