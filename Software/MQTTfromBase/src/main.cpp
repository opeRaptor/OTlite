#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include "RTClib.h"
#include <PubSubClient.h>
#include <WiFiUdp.h>

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define left_Button 16
#define middle_Button 5
#define right_Button 4



void shift(int NixietodisplayUpdate [8]);
void shift(int NixietodisplayUpdate);
void displayUpdate (int duration);
void displayUpdate (void);
void displayUpdate (int numbers,int duration);
void GetTime();
void GetDate();

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

bool adjust(void); //NTP time adjust
void sendNTPpacket(IPAddress *address);

unsigned long delayTime;

//=======================================INCLUDE==NTP======================================
/// NTP Server address
const char* ntpServerName = "0.uk.pool.ntp.org";
const double timezone = 1.0;
// local port to listen for UDP packets
const unsigned int localPort = 2390;
/// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

/// Weekday strings.
//char daysOfTheWeek[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
/**
 * @brief NTP Server IP variable.
 *
 * Don't hardwire the IP address or we won't get the benefits of the pool.
 */
IPAddress timeServerIP;
/// NTP time stamp is in the first 48 bytes of the message
const int NTP_PACKET_SIZE = 48;
/// buffer to hold incoming and outgoing packets
byte packetBuffer[ NTP_PACKET_SIZE];
//===================================================================================

int NixieArray [8];
int PreviousArray [8];
int Brightness = 10;
int transition = 16;

int clockPin = 13;
int BL = 15;
int dataPin = 14;
int latch = 12;


const char* mqtt_server = "iot.eclipse.org";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {

  Serial.begin(9600);
  Wire.pins(0, 2); //SDA and SCL

  //MQTT setup
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Start UDP Server to receive the NTP response.
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.println("Initial RTC sync with NTP");
  while (!adjust()){
    adjust();
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

  if (!client.connected()) {
    reconnect();
  }

}


void loop()
{
  GetTime();
  displayUpdate();

  client.loop();

  DateTime now = rtc.now();
  int nowish = now.minute() / 10;

  if (nowish != lastMsg) {
    lastMsg = now.minute() / 10;
    ++value;
    snprintf (msg, 75, "x10min: %ld", value);
    //Serial.print("Publish message: ");
    //Serial.println(msg);
    client.publish("/OTL/batt", msg);
  }

  /*if (digitalRead(left_Button) == 1)
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
  }*/
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
//=======================================FUNCTION==MQTTCallback============================
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
//=======================================FUNCTION==MQTTreconnect============================
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
      client.publish("/OTL/batt", "beginBatt test");
      // ... and resubscribe
      client.subscribe("/OTL/incoming");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//=======================================NTP==FUNCTION============================================
/**
 * @brief Adjust the DS3231 with current date and time.
 *
 * <pre>
 * NTPv4 Basic Header Specification:
 *
 * Bytes  : Description
 * -------:----------------------
 *       0: LI(2), VN(3), Mode(3)
 *       1: Stratum
 *       2: Poll
 *       3: Precision
 *  4 -  7: Root Delay
 *  8 - 11: Root Dispersion
 * 12 - 15: Reference ID
 * 16 - 23: Reference Timestamp
 * 24 - 31: Origin Timestamp
 * 32 - 39: Receive Timestamp
 * 40 - 47: Transmit Timestamp
 *
 * Transmit Timestamp:
 * Time at the server when the response left for the client, in NTP timestamp format.
 *
 *
 * NTP timestamp format
 *
 * Bytes: Description
 * -----:-------------------
 * 0 - 3: Seconds since 1900
 * 4 - 7: Fraction of Second
 * </pre>
 *
 */
bool adjust(void)
{
  unsigned long mi;
  int cb = 0;

  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  // send an NTP packet to a time server
  sendNTPpacket(&timeServerIP);
  mi = millis();

  // wait to see if a reply is available
  while (millis() - mi < 2000 && !cb)
  {
    cb = udp.parsePacket();
    yield();
  }

  if (!cb)
  {
    Serial.println("[ERROR]: No packet available.");

    return 0;
  }

  //Serial.print("packet received, length=");
  //Serial.println(cb);
  // We've received a packet, read the data from it
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  //Serial.print("Seconds since Jan 1 1900 = " );
  //Serial.println(secsSince1900);

  // now convert NTP time into everyday time:
  Serial.print("Unix UTC time = ");
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  // print Unix time:
  Serial.println(epoch);

  // adjust to user timezone
  epoch += timezone * 3600;

  //rtc.setEpoch(epoch);
  rtc.adjust(epoch); //adjust RTC time to NTP time
  return 1;
}

void sendNTPpacket(IPAddress *address)
{
  Serial.println("sending NTP packet...");

  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(*address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
