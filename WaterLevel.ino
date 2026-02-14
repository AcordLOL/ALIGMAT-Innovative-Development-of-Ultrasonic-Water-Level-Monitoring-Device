const int requiredReadings = 50;
int readings[requiredReadings][2];
int numReadings = 0;

int maxCount = 0;
int maxValue = 0;

void getWaterLevel() {
  // long duration = pulseIn(echoPin, HIGH);
  // int distance = duration * 0.034 / 2;
  int distance = random(14, 18);
  Serial.print(distance);
  Serial.print(" ");

  numReadings++;
  for (int i = 0; i < numReadings; i++) {
    if (distance == readings[i][0]) {
      readings[i][1]++;

      if (readings[i][1] > maxCount) {
        maxCount = readings[i][1];
        maxValue = readings[i][0];
        
        Serial.print("- ");
        Serial.print(maxCount);
      }

    } else if (i == numReadings-1) {
      readings[i][0] = distance;
      readings[i][1] = 1;
    }
  }

  Serial.println();

  if (maxCount > requiredReadings/3 || numReadings == requiredReadings) {
    Serial.print("Done - ");
    Serial.println(maxCount);

    numReadings = 0;
    maxCount = 0;
    maxValue = 0;
  }
} 