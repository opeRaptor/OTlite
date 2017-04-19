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
//=======================================INCLUDE==NTP======================================
/// NTP Server address
const char* ntpServerName = "0.uk.pool.ntp.org";
const double timezone = 1.0;
// local port to listen for UDP packets
const unsigned int localPort = 2390;
/// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

/// Weekday strings.
char daysOfTheWeek[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
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
bool adjust(void);
void sendNTPpacket(IPAddress *address);


void setup() {

  Serial.begin(9600);
  Wire.pins(0, 2); //SDA and SCL


  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
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

}

void loop() {
  int NixieArray [8];
    Displaynumber(0000);
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
