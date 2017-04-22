#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "RTClib.h"

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define Left_button 16
#define Middle_button 5
#define Right_button 4

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

unsigned long delayTime;

int clockPin = 13;
int BL = 15;
int dataPin = 14;
int latch = 12;
void Displaynumber(int Nixietodisplay [8]);
void Displaynumber(int Nixietodisplay);

void setup() {

  Serial.begin(9600);
  Wire.pins(0, 2); //SDA and SCL
  Serial.println(F("BME280 test"));

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(clockPin, OUTPUT);
  pinMode(BL, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latch, OUTPUT);

  pinMode(16, INPUT); // Left button
  pinMode(5, INPUT);  // Middle button
  pinMode(4, INPUT);  // Right button

  digitalWrite(BL, LOW);
  digitalWrite(latch, HIGH);

  digitalWrite(dataPin, HIGH);
  for (int i = 0; i < 50; i++)
  {
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }


  bool status;

  // default settings
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();
  Displaynumber (0000);
  delay(1000);
}


void loop() {
  int NixieArray [8];
  int NixieArray2 [8];
  int Multiplexing [8];

  Multiplexing [0] = 10;
  Multiplexing [1] = 10;
  Multiplexing [2] = 10;
  Multiplexing [3] = 10;

  while (1)
  {
    for (int i = 0; i < 8; ++i)
    {
      NixieArray2 [i] = NixieArray [i];
    }
    DateTime now = rtc.now();

    NixieArray [0] = now.hour() / 10;
    NixieArray [1] = now.hour() % 10;
    NixieArray [2] = now.minute() / 10;
    NixieArray [3] = now.minute() % 10;
    NixieArray [4] = 0;
    NixieArray [5] = 0;
    NixieArray [6] = 0;
    NixieArray [7] = 0;

    Multiplexing [5] = 0;
    Multiplexing [6] = 0;
    if ((now.second() % 2) == 1) Multiplexing [5] = 2;
    else Multiplexing [6] = 1;

    Transition(NixieArray2, NixieArray, 17);
    delay(3);

    Displaynumber(Multiplexing);
    delay (1);

/*int testa [2] = {1, 2};
    test (*testa);
    delay (1000);*/

    if (digitalRead(Left_button) == 1)
    {
      unsigned long InitialTime = millis();
      unsigned long RelativeTime;
      int chronos;
      bool ButtonReleased = 0;

      Multiplexing [4] = 2;
      Multiplexing [5] = 0;
      Multiplexing [6] = 2;
      Multiplexing [7] = 0;
      
      // Chronos, works up to 10 min so far
      while (digitalRead(Left_button) == 1 || ButtonReleased == 1)
      {
        RelativeTime = millis();
        chronos = int((RelativeTime - InitialTime) / 10);
        
        NixieArray [0] = chronos / 600;
        NixieArray [1] = (chronos/100)%6;
        NixieArray [2] = (chronos/10)%10;
        NixieArray [3] = chronos % 10;

        Displaynumber(NixieArray);
        delay(3);

        Displaynumber(Multiplexing);
        delay (1);
      }
    }

    if (digitalRead(Middle_button) == 1)
    {
      NixieArray2 [0] = now.month() / 10;
      NixieArray2 [1] = now.month() % 10;
      NixieArray2 [2] = now.day() / 10;
      NixieArray2 [3] = now.day() % 10;
      NixieArray2 [4] = 0;
      NixieArray2 [5] = 2;
      NixieArray2 [6] = 0;
      NixieArray2 [7] = 0;
      Transition (NixieArray, NixieArray2, 14);
      while (digitalRead(Middle_button) == 1)
      {
        delay(50);
      }
      Transition (NixieArray2, NixieArray, 14);
    }


    // Display Date when pressed
    if (digitalRead(Right_button) == 1)
    {
      NixieArray2 [0] = now.month() / 10;
      NixieArray2 [1] = now.month() % 10;
      NixieArray2 [2] = now.day() / 10;
      NixieArray2 [3] = now.day() % 10;
      NixieArray2 [4] = 0;
      NixieArray2 [5] = 2;
      NixieArray2 [6] = 0;
      NixieArray2 [7] = 0;
      Transition (NixieArray, NixieArray2, 20);
      while (digitalRead(Right_button) == 1)
      {
        delay(50);
      }
      Transition (NixieArray2, NixieArray, 20);
    }
  }
}

//=======================================FUNCTION==DISPLAYNUMBER==========================
// Display numbers and dots, from an int array, left to right, 4 first are numbers, 4 last are dots
// If a number is not in the range 0-9, no numbers are lit up
// Dots: 0 is nothing, 1 is left dot, 2 is right dot, 3 is both dots
void Displaynumber(int Nixietodisplay [8])
{
  int Nixiearray [10] = {8, 9, 0, 1, 2, 3, 4, 5, 6, 7};
  int j;

  for (int k = 3; k >= 0; k--)
  {
    digitalWrite(latch, LOW);
    digitalWrite(dataPin, HIGH);

    if ((Nixietodisplay [k + 4] == 3) || (Nixietodisplay [k + 4] == 1))   digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);

    if ((Nixietodisplay [k + 4] == 3) || (Nixietodisplay [k + 4] == 2))   digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);

    if ((Nixietodisplay [k] > 9) || (Nixietodisplay [k] < 0))
    {
      for (int i = 0; i < 10; i++)
      {
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
      }
    }
    else
    {
      j = Nixiearray [Nixietodisplay [k]];
      for (int i = 0; i < j; i++)
      {
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
      }
      digitalWrite(dataPin, LOW);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      digitalWrite(dataPin, HIGH);
      for (int i = 0; i < (9 - j); i++)
      {
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
      }
    }
    digitalWrite(latch, HIGH);
  }
}

void Displaynumber(int Nixietodisplay)
{
  int nixiebuffer [8];
  nixiebuffer [0] = Nixietodisplay / 1000;
  nixiebuffer [1] = (Nixietodisplay / 100) % 10;
  nixiebuffer [2] = (Nixietodisplay / 10) % 10;
  nixiebuffer [3] = Nixietodisplay % 10;
  nixiebuffer [4] = 0;
  nixiebuffer [5] = 0;
  nixiebuffer [6] = 0;
  nixiebuffer [7] = 0;
  if (Nixietodisplay == 10000)
  {
    nixiebuffer [0] = 10;
    nixiebuffer [1] = 10;
    nixiebuffer [2] = 10;
    nixiebuffer [3] = 10;
  }

  Displaynumber(nixiebuffer);
}

//=======================================FUNCTION==TRANSITION=============================
// Takes an array, and do a smooth transition from the before array to the after, for a given duration
void Transition (int before [8], int after [8], int duration)
{
  for (int i = 0; i < 8; ++i)
  {
    if ((before [i]) != (after [i])) goto Change;
  }
  Displaynumber (before);
  return;

Change: for (int i = 0; i < duration; i++)
  {
    Displaynumber (before);
    delay (duration - i);
    Displaynumber (after);
    delay (i);
  }
}

//=======================================FUNCTION==SERIAL==RTC============================
void printRTCValues() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.println();
  delay(delayTime);
}

//=======================================FUNCTION==SERIAL==TEMP===========================
void printBME280Values() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void test (int testarray [2])
{
  int i;
  i =  testarray [1] + testarray [0]*10;
  Displaynumber (i);
}

/*   for (int i = 0; i < 40; i++)
   {
  Displaynumber(random(0, 9), 0, 0);
  Displaynumber(random(0, 9), 0, 0);
  Displaynumber(random(0, 9), 0, 0);
  Displaynumber(random(0, 9), 0, 0);
  delay(30);
   }
*/


/*
  while (digitalRead(16)==1)
  {
    Displaynumber(1000);
    delay(50);
  }

  while (digitalRead(5)==1)
  {
    Displaynumber(100);
    delay(50);
  }

  while (digitalRead(4)==1)
  {
    Displaynumber(10);
    delay(50);
  }*/
