void handleRequest(WiFiClient &serverClient) {
  
  if (request.startsWith("GET /waterLevel")) {
    StaticJsonDocument<256> doc;
    
    doc["waterLevel"] = 50;

    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Content-type: application/json");
    serverClient.println("Connection: close");
    serverClient.println();

    serializeJson(doc, serverClient);
    serverClient.println();

  } else if (request.startsWith("GET /numbers")) {
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

    EEPROM.write(numCoord, '1');
    handleNumbers(numCoord);

    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Connection: close");
    serverClient.println();

  } else if (request.startsWith("POST /deleteNumber")) {
    
    int numCoord = request.substring(19, 21).toInt();

    EEPROM.write(numCoord, '0');
    handleNumbers(numCoord);

    serverClient.println("HTTP/1.1 200 OK");
    serverClient.println("Connection: close");
    serverClient.println();
    
  // } else if (request.startsWith("POST /message")) {
    
  //   handleMessage();

  //   serverClient.println("HTTP/1.1 200 OK");
  //   serverClient.println("Connection: close");
  //   serverClient.println();
    
  } else serverClient.println(configHTML);
}