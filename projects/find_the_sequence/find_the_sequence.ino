//www.elegoo.com
//2016.12.08

#define GREEN_PIN 9
#define RED_PIN 8

#define BUTTON_0 2
#define BUTTON_1 3

int sequence[7] = {0, 0, 0, 1, 1, 0, 1};

void setup()
{
  Serial.begin(9600);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BUTTON_0, INPUT_PULLUP);
  pinMode(BUTTON_1, INPUT_PULLUP);

  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(RED_PIN, LOW);

  Serial.println("Start game");
}

int i = 0;
bool completed = false;

int previousButton0State = HIGH;
bool pressing0 = false;

int previousButton1State = HIGH;
bool pressing1 = false;

void loop()
{
  if (!completed) {
    int buttonPressed = getButtonPressed();
    if (buttonPressed != -1) {
      Serial.println("Button pressed " + String(buttonPressed));
      // hit
      if (sequence[i] == buttonPressed) {
        hit();
      } else {
        miss();
      }
    }
  }
}

int getButtonPressed() {
  int button0 = digitalRead(BUTTON_0);
  int button1 = digitalRead(BUTTON_1);

  if (button0 != previousButton0State) {
    if (button0 == LOW) {
      pressing0 = true;
    } else if (pressing0) {
      pressing0 = false;
      return 0;
    }
    previousButton0State = button0;
  } else if (button1 != previousButton1State) {
    if (button1 == LOW) {
      pressing1 = true;
    } else if (pressing1) {
      pressing1 = false;
      return 1;
    }
    previousButton1State = button1;
  }
  return -1;
}

bool isCompleted() {
  Serial.println(String(sizeof(sequence) / sizeof(int)));
  return i >= sizeof(sequence) / sizeof(int);
}

void hit() {
  i++;
  if (isCompleted()) {
    completed = true;
    Serial.println("Game completed!");
    for (int j = 0; j < 5; j++) {
      delay(200);
      digitalWrite(GREEN_PIN, HIGH);
    }
  } else {
    digitalWrite(GREEN_PIN, HIGH);
    delay(1000);
    digitalWrite(GREEN_PIN, LOW);
  }
}

void miss() {
  Serial.println("Start over");
  i = 0;
  digitalWrite(RED_PIN, HIGH);
  delay(1000);
  digitalWrite(RED_PIN, LOW);
}

