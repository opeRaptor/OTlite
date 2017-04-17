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
}


void loop() {
  int NixieArray [8];

  DateTime now = rtc.now();
  
  NixieArray [0] = now.hour()/10;
  NixieArray [1] = now.hour()%10;
  NixieArray [2] = now.minute()/10;
  NixieArray [3] = now.minute()%10;
  NixieArray [4] = 0;
  NixieArray [5] = 0;
  NixieArray [6] = 0;
  NixieArray [7] = 0;
  Displaynumber(NixieArray);

  delay(15);
  if ((now.second()%2)==1)NixieArray [5] = 2;
  else NixieArray [6] = 1;

  NixieArray [0] = 10;
  NixieArray [1] = 10;
  NixieArray [2] = 10;
  NixieArray [3] = 10;
  Displaynumber(NixieArray);
  delay (5);

// Display temperature when left button is pressed
  /*while (digitalRead(16)==1)   
  {
    int a = int(bme.readTemperature());
    NixieArray [2] = (a-23)/10;
    NixieArray [3] = (a-23)%10;
    NixieArray [0] = 10;
    NixieArray [1] = 10;
    NixieArray [4] = 0;
    NixieArray [5] = 0;
    NixieArray [6] = 0;
    NixieArray [7] = 0;
    Displaynumber(NixieArray);
    delay(50);
  }*/
  
// Display Date when pressed
  if (digitalRead(5)==1)
  {
    Transition ((now.hour()*100 + now.minute()), (now.month()*100 + now.day()), 17);
    while (digitalRead(5)==1)
    {
      NixieArray [0] = now.month()/10;
      NixieArray [1] = now.month()%10;
      NixieArray [2] = now.day()/10;
      NixieArray [3] = now.day()%10;
      NixieArray [4] = 0;
      NixieArray [5] = 2;
      NixieArray [6] = 0;
      NixieArray [7] = 0;
      Displaynumber(NixieArray);
      delay(50);
    }
    Transition ((now.month()*100 + now.day()), (now.hour()*100 + now.minute()), 17);
  }  
}

void Displaynumber(int Nixietodisplay [8])
{
  int Nixiearray [11] = {8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 10};
  int j;

  for (int k = 3; k >= 0; k--)
  {
    digitalWrite(dataPin, HIGH);
    j = Nixiearray [Nixietodisplay [k]];
    if ((Nixietodisplay [k + 4] == 3) || (Nixietodisplay [k + 4] == 1))   digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);

    if ((Nixietodisplay [k + 4] == 3) || (Nixietodisplay [k + 4] == 2))   digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);

    for (int i = 0; i < j; i++)
    {
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }
    if (j != 10)
    {
      digitalWrite(dataPin, LOW);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }

    digitalWrite(dataPin, HIGH);
    for (int i = 0; i < (9 - j); i++)
    {
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }
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
  if(Nixietodisplay == 10000)
  {
    nixiebuffer [0] = 10;
    nixiebuffer [1] = 10;
    nixiebuffer [2] = 10;
    nixiebuffer [3] = 10;
  }

  Displaynumber(nixiebuffer);
}

void Transition (int before, int after, int duration)
{
  for (int i = 0; i < duration; i++)
  {
    Displaynumber (before);
    delay (duration - i);
    Displaynumber (after);
    delay (i);
  }
}

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
