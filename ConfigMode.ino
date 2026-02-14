int status = WL_IDLE_STATUS;
String currentLine = "";
String request = "";

void configMode() {
  if (once) {
    status = WiFi.beginAP("ALIGMAT", "Password");
    if (status != WL_AP_LISTENING) {
      handleError("Creating Access Point Failed.");
    }

    delay(1000);
    server.begin();
    digitalWrite(config[1], HIGH);
  }

  WiFiClient serverClient = server.available();
  if (serverClient) {
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
        handleRequest(serverClient);

        break;
      } else {
        currentLine = "";
      }
    }
    
    serverClient.stop();
  }
}