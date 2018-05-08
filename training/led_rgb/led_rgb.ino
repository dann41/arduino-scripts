#define BLUE_PIN 3
#define GREEN_PIN 5
#define RED_PIN 6

#define DELAY_SMOOTH 10

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);  

  // read something disconnected (unpredictable) to initialize randomseed
  randomSeed(analogRead(0));
}

int redValue = 0;
int greenValue = 0;
int blueValue = 0;

void loop() {
  int toRed = generateNewColor(redValue);
  int toGreen = generateNewColor(greenValue);
  int toBlue = generateNewColor(blueValue);

  Serial.println(String("RGB to :" + String(toRed) + " " + String(toGreen) + " " + String(toBlue)));
  
  while(redValue != toRed || greenValue != toGreen || blueValue != toBlue) {
    redValue = getNextValue(redValue, toRed);
    greenValue = getNextValue(greenValue, toGreen);
    blueValue = getNextValue(blueValue, toBlue);

    delay(DELAY_SMOOTH);
    analogWrite(RED_PIN, redValue);
    analogWrite(GREEN_PIN, greenValue);
    analogWrite(BLUE_PIN, blueValue);
  }
}

int generateNewColor(int previousValue) {
  // An attempt of generating a much different color
  int newSection = 0;
  int previousSection = previousValue / 85;
  do {
    newSection = random(0, 255) /  85; // 255/3
  } while (newSection == previousSection);

  return min(255, (newSection * 85) + random(0, 85));
  
  //return random(0, 255);
}

int getNextValue(int from, int to) {
  if (from < to) {
    return from + 1;
  } else if (from > to) {
    return from - 1;
  } else {
    return from;
  }
}

