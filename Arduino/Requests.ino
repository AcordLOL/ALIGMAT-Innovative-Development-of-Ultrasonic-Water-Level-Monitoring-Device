void handleRequest(WiFiClient &serverClient, String &request) {
  if (request.startsWith("GET /waterLevel")) {
    StaticJsonDocument<256> doc;
    
    doc["waterLevel"] = waterLevel;

    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Content-type: application/json");
    serverClient.println("Connection: close");
    serverClient.println();

    serializeJson(doc, serverClient);
    serverClient.println();

  } else if (request.startsWith("GET /info")) {
    StaticJsonDocument<256> doc;

    for (int i = 0; i < 10; i++) {
      doc["numbers"][i] = phoneNumbers[i];
    }

    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Content-type: application/json");
    serverClient.println("Connection: close");
    serverClient.println();

    serializeJson(doc, serverClient);
    serverClient.println();

  } else if (request.startsWith("POST /addNumber")) {
  
    int numCoord = request.substring(25, 27).toInt();
    
    for (int i = 0; i < 9; i++) {
      EEPROM.write(numCoord*9 + 2 + i, request[16 + i]);
    }

    EEPROM.write(numCoord, 1);
    handleNumbers(numCoord);

    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Connection: close");
    serverClient.println();

  } else if (request.startsWith("POST /updateWifi")) {
    int ssidLen = request.substring(17, 19).toInt();
    int passLen = request.substring(19 + ssidLen, ssidLen+21).toInt();
    char _ssid[35];
    char _pass[66];

    EEPROM.write(120, ssidLen);
    EEPROM.write(121, passLen);

    for (int i = 0; i < ssidLen; i++) {
      const int value = request[19 + i];

      EEPROM.write(122 + i, value);
      _ssid[i] = value;
    }

    for (int i = 0; i < passLen; i++) {
      const int value = request[21 + ssidLen + i];

      EEPROM.write(122 + ssidLen + i, value);
      _pass[i] = value;
    }

    _ssid[ssidLen] = '\0';
    _pass[passLen] = '\0';

    strcpy(ssid, _ssid);
    strcpy(pass, _pass);

    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Connection: close");
    serverClient.println();

  } else if (request.startsWith("POST /deleteNumber")) {
    
    int numCoord = request.substring(19, 21).toInt();

    EEPROM.write(numCoord, 0);
    handleNumbers(numCoord);

    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Connection: close");
    serverClient.println();
    
  } else serverClient.println(configHTML);
}