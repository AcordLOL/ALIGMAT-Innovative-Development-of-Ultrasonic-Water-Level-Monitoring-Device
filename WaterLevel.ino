const int requiredReadings = 50;
const int echoPin = 13;

int readings[requiredReadings][2];
int numReadings = 0;
int numUnique = 0;

int maxCount = 0;
int maxValue = 0;

long savedMill = 0; 

void getWaterLevel() {
  if (once) {
    pinMode(echoPin, INPUT);
  }

  long currMill = millis();
  if (currMill - savedMill < 100) return;
  savedMill = currMill;

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  // int distance = random(14, 18);

  numReadings++;
  for (int i = 0; i < numReadings; i++) {
    if (distance == readings[i][0]) {
      readings[i][1]++;

      if (readings[i][1] > maxCount) {
        maxCount = readings[i][1];
        maxValue = readings[i][0];
      }

    } else if (i+1 == numReadings) {
      numUnique++;

      readings[i][0] = distance;
      readings[i][1] = 1;
    }
  }

  if (maxCount > requiredReadings/numUnique || numReadings == requiredReadings) {
    disFromWLevel = maxValue;

    maxValue = 0;
    numReadings = 0;
    maxCount = 0;
  }
}