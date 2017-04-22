#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "RTClib.h"

RTC_DS3231 rtc;
/// network
const char ssid[] = "WongKei_FreeDialUp";
const char pass[] = "elpsycongroo";

const char* host = "api.0x52.eu";
const int httpPort = 80;
char inString[32]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?

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

Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    //digitalWrite(clicker,HIGH);
    delay(50);
    //digitalWrite(clicker,LOW);
  }
  Serial.println(WiFi.localIP());


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
  int NixieArray [8];
  Displaynumber(0000);
  delay(10);

}

void loop() {
  char inString[32]; // string for incoming serial data

  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  String url = "/hd/?project=20439-obsolete-time-lite-a-minty-nixie-clock";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    char c = client.read();

      if (c == '<' ) { //'<' is our begining character
        startRead = true; //Ready to start reading the part 
      }else if(startRead){

        if(c != '>'){ //'>' is our ending character
          inString[stringPos] = c;
          stringPos ++;
        }else{
          //got what we need here! We can disconnect now
          startRead = false;
          stringPos = 0; //reset position inString
          client.stop();
          client.flush();
          Serial.println("disconnecting.");

        }
  }
  }
  Serial.print(inString);
  Serial.println();
  Serial.println("closing connection");
  
  Displaynumber(atoi(inString));
  delay(10);
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
