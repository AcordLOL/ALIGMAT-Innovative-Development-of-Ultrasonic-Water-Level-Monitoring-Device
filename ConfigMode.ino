int status = WL_IDLE_STATUS;

void configMode() {
  if (once) {
    status = WiFi.beginAP("ALIGMAT", "Password");
    if (status != WL_AP_LISTENING) {
      handleError("Creating Access Point Failed.");
    }

    delay(1000);
    server.begin();
    // digitalWrite(config[1], HIGH);
  }

  WiFiClient serverClient = server.available();
  if (serverClient) {
    String currentLine = "";
    String request = "";

    while (serverClient.connected()) {
      delayMicroseconds(10);
      if (!serverClient.available()) continue;

      char c = serverClient.read();
      if (c != '\n') {
        if (c != '\r') currentLine += c;
        continue;
      }

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