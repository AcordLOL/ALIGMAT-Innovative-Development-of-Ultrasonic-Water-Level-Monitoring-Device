void handleError(const char* msg) {
  hasError = true;
  Serial.println(msg);

  while(true);
}

void handleMode() {
  // button toggle to switch modes
  config[2] = digitalRead(config[0]);
  if (config[2] != config[3]) {
    config[3] = config[2];

    if (config[2] == LOW) {
      isConfig = !isConfig;
      EEPROM.write(0, isConfig);
      
      once = true;
    }
  }

  if (isConfig) configMode();
  else passiveMode();
}

void handleNumbers(int i) {
  int hasNum = EEPROM.read(i);

  phoneNumbers[i-1][0] = hasNum;
  for (int n = 0; n < 9; n++) {
      phoneNumbers[i-1][n+1] = (hasNum) ? EEPROM.read(i*9 + 2 + n) : '0';
  }

  phoneNumbers[i-1][11] = '\0';
  Serial.println(phoneNumbers[i-1]);
}

void handleMessage() {
    mySerial.println("AT+CMGF=1");
    Serial.println(readSerial());
    for (int i = 0; i < 10; i++) {
      if (!phoneNumbers[i][0]) continue;

      char setNum[24];
      char num[10];
      for (int n = 0; n < 9; n++) {
        num[0] = phoneNumbers[i][n+1];
      }
      num[10] = '\0';

      sprintf(setNum, "AT+CMGS=\"+639%s\"", num);
      mySerial.println(setNum);
      Serial.println(readSerial());
      mySerial.println("ALIGMAT: Water Level Monitoring Device Warning!");
      mySerial.println((char)26);
    }
}

String readSerial() {
  delay(100);
  if (mySerial.available()) {
    return mySerial.readString();
  }
}