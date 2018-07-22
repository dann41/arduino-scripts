#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// #### PIN DEFINITIONS ####

// RFID
#define SS_PIN 10
#define RST_PIN 9

// SHIFT REGISTER (74HC595)
#define DATA_SR 5
#define LATCH_SR 6
#define CLOCK_SR 7

// 4 DIGITS 7 SEGMENTS
int DIGIT_PINS[] = {A3, A2, A1, A0};

// #### END PIN DEFINITIONS ####

// #### PROGRAM CONSTANTS DEFINITIONS ####

// USEFUL CONSTANTS
// 0, 1, 2, ..., A, ..., F, off
const unsigned char NUMBERS[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00};
const unsigned char ERROR = 0x79; // E (error)
const unsigned char EMPTY = 0x00; // Off
#define DELAY_DIGITS 5

byte EXPECTED_CARD[] = {0x43, 0xCC, 0x51, 0x21};
const int SECRET_CODE = 7526;
#define ERROR_DURATION 2000

// #### END PROGRAM CONSTANTS DEFINITIONS ####

// #### PROGRAM VARIABLE DEFINITIONS ####

bool secretCodeFound = false;

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// #### PROGRAM VARIABLE DEFINITIONS ####

void setup()
{
  Serial.begin(9600);
  pinMode(LATCH_SR, OUTPUT);
  pinMode(CLOCK_SR, OUTPUT);
  pinMode(DATA_SR, OUTPUT);

  for (int i = 0; i < 4; i++)
  {
    pinMode(DIGIT_PINS[i], OUTPUT);
  }

  SPI.begin();     // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

}

/**************************************
 *       Print utils functions
 **************************************/

void printHex(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

/*****************************************************
 *       Segment Display utils functions
 *****************************************************/
void Display(int num, int digit)
{
  digitalWrite(LATCH_SR, LOW);
  for (int i = 0; i < 4; i++)
  {
    if (i == digit)
    {
      digitalWrite(DIGIT_PINS[i], LOW);
    }
    else
    {
      digitalWrite(DIGIT_PINS[i], HIGH);
    }
  }
  shiftOut(DATA_SR, CLOCK_SR, MSBFIRST, num);
  digitalWrite(LATCH_SR, HIGH);
  delay(DELAY_DIGITS);
}

void Display(int num)
{
  digitalWrite(LATCH_SR, LOW);
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(DIGIT_PINS[i], LOW);
  }
  shiftOut(DATA_SR, CLOCK_SR, MSBFIRST, num);
  digitalWrite(LATCH_SR, HIGH);
  delay(DELAY_DIGITS);
}

void DisplayNumber(unsigned char num, int digit)
{
  Display(NUMBERS[num], digit);
}

void Display4DigitsNumber(int num)
{
  int digit1 = num % 10;
  int digit10 = (num / 10) % 10;
  int digit100 = (num / 100) % 10;
  int digit1000 = (num / 1000) % 10;

  DisplayNumber(digit1, 3);
  DisplayNumber(digit10, 2);
  DisplayNumber(digit100, 1);
  DisplayNumber(digit1000, 0);
}

void DisplayError()
{
  Serial.println("DisplayError");
  Display(ERROR);
}

void ClearDisplay()
{
  Display(EMPTY);
}

bool isMIFARECard(MFRC522::PICC_Type piccType)
{
  return piccType == MFRC522::PICC_TYPE_MIFARE_MINI ||
         piccType == MFRC522::PICC_TYPE_MIFARE_1K ||
         piccType == MFRC522::PICC_TYPE_MIFARE_4K;
}

bool isSameCard(byte *card1, byte *card2)
{
  return card1[0] == card2[0] &&
         card1[1] == card2[1] &&
         card1[2] == card2[2] &&
         card1[3] == card2[3];
}

bool isExpectedKey()
{
  return isSameCard(rfid.uid.uidByte, EXPECTED_CARD);
}

void loop()
{
  if (secretCodeFound)
  {
    Display4DigitsNumber(SECRET_CODE);
    return;
  }

  ClearDisplay();
  
  // Look for cards
  if (!rfid.PICC_IsNewCardPresent())
  {
    return;
  }

  // try to read card
  if (!rfid.PICC_ReadCardSerial())
  {
    Serial.print(F("Couldn't read card"));
    return;
  }

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  if (!isMIFARECard(piccType))
  {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  Serial.println(F("The NUID tag is:"));
  Serial.print(F("In hex: "));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  if (isExpectedKey())
  {
    Serial.println(F("Secret code found"));
    Display4DigitsNumber(SECRET_CODE);
    secretCodeFound = true;
    return;
  }
  else
  {
    Serial.println(F("Wrong card"));
    DisplayError();
    delay(ERROR_DURATION);
    ClearDisplay();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
