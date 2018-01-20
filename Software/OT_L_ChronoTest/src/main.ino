#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "RTClib.h"
#include <PubSubClient.h>

void Displaynumber(int Nixietodisplay [8]);
void Displaynumber(int Nixietodisplay);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

RTC_DS3231 rtc;
/// network
const char ssid[] = "WongKei_FreeDialUp";
const char pass[] = "elpsycongroo";
const char* mqtt_server = "iot.eclipse.org";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

unsigned long delayTime;

int clockPin = 13;
int BL = 15;
int dataPin = 14;
int latch = 12;



void setup() {

  Serial.begin(9600);
  Wire.pins(0, 2); //SDA and SCL

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

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

  if (!client.connected()) {
    reconnect();
  }
  //client.loop();

for (int i = 0; i < 10000; i++) {
  Displaynumber(i);
  delay(50);
  client.loop();
}
  /*long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "kek world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }*/
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

void callback(char* topic, byte* payload, unsigned int length) {
  char character[4];

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < 4; i++) {
    Serial.print((char)payload[i]);
    Serial.println();
    character[i] = (char)payload[i];
  }
  Displaynumber(atoi(character));
  Serial.println();
  delay(5000);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
