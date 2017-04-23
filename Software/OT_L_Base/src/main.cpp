#include "Arduino.h"
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
#define left_Button 16
#define middle_Button 5
#define right_Button 4

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

void shift(int NixietodisplayUpdate [8]);
void shift(int NixietodisplayUpdate);
void displayUpdate (int duration);
void displayUpdate (void);
void displayUpdate (int numbers,int duration);
void GetTime();
void GetDate();

unsigned long delayTime;

int NixieArray [8];
int PreviousArray [8];
int Brightness = 10;
int transition = 16;

int clockPin = 13;
int BL = 15;
int dataPin = 14;
int latch = 12;

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
}


void loop()
{
  GetTime();
  displayUpdate();

  if (digitalRead(left_Button) == 1)
  {
    //see bottom of program for draft of chrono
  }

  if (digitalRead(middle_Button) == 1)
  {
    GetDate();
    displayUpdate (14);
    while (digitalRead(middle_Button) == 1)
    {
      displayUpdate(14);
    }
    GetTime();
    displayUpdate (14);
  }
  if (digitalRead(right_Button) == 1)
  {

    displayUpdate (14);
    while (digitalRead(middle_Button) == 1)
    {
      displayUpdate(14);
    }
    GetTime();
    displayUpdate (14);
  }
}

//=======================================FUNCTION==DISPLAYUPDATE================================
// Do a smooth transition between the previous array dipslayed and NiexieArray, for a given duration
void displayUpdate (int duration)
{
  int dots [8] = {10, 10, 10, 10, 0, 0, 0, 0};
  for (int i = 0; i < 4; ++i)
  {
    if ((PreviousArray [i]) != (NixieArray [i])) goto Change;
  }
  dots [4] = NixieArray [4];
  dots [5] = NixieArray [5];
  dots [6] = NixieArray [6];
  dots [7] = NixieArray [7];

  shift (NixieArray);
  delayMicroseconds (Brightness * 300);

  shift (dots);
  delayMicroseconds (Brightness * 50);

  if (Brightness != 10)
  {
    shift (10000);
    delayMicroseconds (3000 - Brightness * 300);
  }

  for (int i = 0; i < 8; ++i)
  {
    PreviousArray [i] = NixieArray [i];
  }
  return;

Change:
  dots [4] = NixieArray [4];
  dots [5] = NixieArray [5];
  dots [6] = NixieArray [6];
  dots [7] = NixieArray [7];

  int dots2 [8] = {10, 10, 10, 10, 0, 0, 0, 0};
  dots2 [4] = PreviousArray [4];
  dots2 [5] = PreviousArray [5];
  dots2 [6] = PreviousArray [6];
  dots2 [7] = PreviousArray [7];

  for (int i = 0; i < duration; i++)
  {
    shift (PreviousArray);
    delayMicroseconds ((duration - i) * 1000 * Brightness / 10);
    shift (dots2);
    delayMicroseconds ((duration - i) * 1000 * Brightness / 60);
    if (Brightness != 10)
    {
    shift (10000);
    delayMicroseconds ((duration - i) * 1000 * (10 - Brightness) / 10);
    }

    shift (NixieArray);
    delayMicroseconds (i * 1000 * Brightness / 10);
    shift (dots);
    delayMicroseconds (i * 1000 * Brightness / 60);
    if (Brightness != 10)
    {
    shift (10000);
    delayMicroseconds (i * 1000 * (10 - Brightness) / 10);
    }
  }
  for (int i = 0; i < 8; ++i)
  {
    PreviousArray [i] = NixieArray [i];
  }
}

void displayUpdate (void)
{
  displayUpdate(transition);
}

void displayUpdate (int numbers, int duration)
{
  NixieArray [0] = numbers / 1000;
  NixieArray [1] = (numbers / 100) % 10;
  NixieArray [2] = (numbers / 10) % 10;
  NixieArray [3] = numbers % 10;
  displayUpdate (duration);
}

//=======================================FUNCTION==SHIFT==============================
// displayUpdate numbers and dots, from an int array, left to right, 4 first are numbers, 4 last are dots
// If a number is not in the range 0-9, no numbers are lit up
// Dots: 0 is nothing, 1 is left dot, 2 is right dot, 3 is both dots
void shift(int NixietodisplayUpdate [8])
{
  int Nixiearray [10] = {8, 9, 0, 1, 2, 3, 4, 5, 6, 7};
  int j;

  for (int k = 3; k >= 0; k--)
  {
    digitalWrite(latch, LOW);
    digitalWrite(dataPin, HIGH);

    if ((NixietodisplayUpdate [k + 4] == 3) || (NixietodisplayUpdate [k + 4] == 1))   digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);

    if ((NixietodisplayUpdate [k + 4] == 3) || (NixietodisplayUpdate [k + 4] == 2))   digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);

    if ((NixietodisplayUpdate [k] > 9) || (NixietodisplayUpdate [k] < 0))
    {
      for (int i = 0; i < 10; i++)
      {
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
      }
    }
    else
    {
      j = Nixiearray [NixietodisplayUpdate [k]];
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

void shift(int NixietodisplayUpdate)
{
  int nixiebuffer [8];
  if (NixietodisplayUpdate == 10000)
  {
    nixiebuffer [0] = 10;
    nixiebuffer [1] = 10;
    nixiebuffer [2] = 10;
    nixiebuffer [3] = 10;
  }
  else
  {
    nixiebuffer [0] = NixietodisplayUpdate / 1000;
    nixiebuffer [1] = (NixietodisplayUpdate / 100) % 10;
    nixiebuffer [2] = (NixietodisplayUpdate / 10) % 10;
    nixiebuffer [3] = NixietodisplayUpdate % 10;
  }
  nixiebuffer [4] = 0;
  nixiebuffer [5] = 0;
  nixiebuffer [6] = 0;
  nixiebuffer [7] = 0;
  if (NixietodisplayUpdate == 10000)
  {
    nixiebuffer [0] = 10;
    nixiebuffer [1] = 10;
    nixiebuffer [2] = 10;
    nixiebuffer [3] = 10;
  }

  shift(nixiebuffer);
}

//=======================================FUNCTION==GETTIME================================
void GetTime()
{
  DateTime now = rtc.now();

  NixieArray [0] = now.hour() / 10;
  NixieArray [1] = now.hour() % 10;
  NixieArray [2] = now.minute() / 10;
  NixieArray [3] = now.minute() % 10;
  NixieArray [4] = 0;
  NixieArray [5] = 0;
  NixieArray [6] = 0;
  NixieArray [7] = 0;

  if ((now.second() % 2) == 1) NixieArray [5] = 2;
  else NixieArray [6] = 1;
}


//=======================================FUNCTION==GETDATE================================
void GetDate()
{
  DateTime now = rtc.now();

  NixieArray [0] = now.month() / 10;
  NixieArray [1] = now.month() % 10;
  NixieArray [2] = now.day() / 10;
  NixieArray [3] = now.day() % 10;
  NixieArray [4] = 0;
  NixieArray [5] = 2;
  NixieArray [6] = 0;
  NixieArray [7] = 0;
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

/*   for (int i = 0; i < 40; i++)
   {
  shift(random(0, 9), 0, 0);
  shift(random(0, 9), 0, 0);
  shift(random(0, 9), 0, 0);
  shift(random(0, 9), 0, 0);
  delay(30);
   }
*/


     /* unsigned long InitialTime = millis();
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
       chronos = int((RelativeTime - InitialTime) / 100);

       NixieArray [0] = chronos / 600;
       NixieArray [1] = (chronos / 100) % 6;
       NixieArray [2] = (chronos / 10) % 10;
       NixieArray [3] = chronos % 10;*/
