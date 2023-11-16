
#include <SoftwareSerial.h>
SoftwareSerial sim800L(8, 7); // sim800L(rxPin, txPin)

String buff;

// Ultrasonic sensor library
#include <NewPing.h>

// RTC
#include "RTClib.h"

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//sd card
#include <SPI.h>
#include <SD.h>

File dataFile;
const int chipSelect = 4; // chip select pin connected to arduino pin 4.

// Fan
#define fan A1 // Arduino pin tied to trigger pin on the fan.

// Hazard led
#define led 2

// Rat counter
int i = 1;

// Ultrasonic sensor
int h1; // Distance measured from sensor 1
int h2; // Distance measured from sensor 2

NewPing sonar1(3, 5, 10); // sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE)
NewPing sonar2(9, 6, 10); // sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE)

void setup() {
  // Initialize the serial monitor
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Begin serial communication with Arduino and SIM800L
  sim800L.begin(9600);

  Serial.println("Initializing SIM800L");

  sim800L.println("AT");
  waitForResponse();

  sim800L.println("ATE1");
  waitForResponse();

  sim800L.println("AT+CMGF=1");
  waitForResponse();

  sim800L.println("AT+CNMI=1,2,0,0,0");
  waitForResponse();

  // Fan
  pinMode(fan, OUTPUT);

  // Hazard led
  pinMode(led, OUTPUT);

  // RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    digitalWrite(led, HIGH);
    //send_sms("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  digitalWrite(led, LOW);
  Serial.println("RTC is present");
  //send_sms("RTC is present");

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    //send_sms("RTC is NOT running, let's set the time!");
    /* When time needs to be set on a new device, or after a power loss, the
       following line sets the RTC to the date & time this sketch was compiled */
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  Serial.println("RTC is running");
  //send_sms("RTC is running");

  /* When time needs to be re-set on a previously configured device, the
     following line sets the RTC to the date & time this sketch was compiled */
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // sd card
  Serial.println("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card failed, or not present");
    digitalWrite(led, HIGH);
    //send_sms("SD Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  digitalWrite(led, LOW); // Default state of hazard led
  Serial.println(" SD card initialized.");
  //send_sms("SD card initialized.");

  // Printing the header file
  dataFile = SD.open("Rat_Data.csv", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print("Date");
    dataFile.print(",");
    dataFile.print("Day");
    dataFile.print(",");
    dataFile.print("Time");
    dataFile.print(",");
    dataFile.println("Count");
    dataFile.close();
    // print to the serial port too:
    // Serial.println();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    digitalWrite(led, HIGH);
    //send_sms("error opening datalog.txt");
  }

}

void loop() {

  // RTC
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

  // Ultrasonic sensor
  h1 = sonar1.ping_cm();
  delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  h2 = sonar2.ping_cm();
  delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

  Serial.print("Sonar 1: ");
  Serial.print(h1);
  Serial.println("cm");

  Serial.print("Sonar 2: ");
  Serial.print(h2);
  Serial.println("cm\n\r");

    // SIM800L
  while (sim800L.available()) {
    buff = sim800L.readString();
    Serial.println(buff);
  }
  if (buff.indexOf("Now") >= 0) {
    /*char buffer[60];
    sprintf(buffer, "Height one is %d cm and height two is %d cm", h1, h2);
    Serial.println(buffer);
    String a = String(buffer);
    send_sms(a);*/
    send_sms("All units are in cm");
    //send_sms("Current data say that, height one is " + String(h1));
    //send_sms("And height two is " + String(h2));
    buff = "";
  }

  if (h1 < 7) {
    digitalWrite(fan, HIGH);
    send_sms("Rat");

    dataFile = SD.open("Rat_Data.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(now.year(), DEC);
      dataFile.print('/');
      dataFile.print(now.month(), DEC);
      dataFile.print('/');
      dataFile.print(now.day(), DEC);
      dataFile.print(",");
      dataFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
      dataFile.print(",");
      dataFile.print(now.hour(), DEC);
      dataFile.print(':');
      dataFile.print(now.minute(), DEC);
      dataFile.print(':');
      dataFile.print(now.second(), DEC);
      dataFile.print(",");
      dataFile.println(i);
      dataFile.close();
      // print to the serial port too:
      // Serial.println();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
      digitalWrite(led, HIGH);
    }
    i++;
  }
  else {
    digitalWrite(fan, LOW);
  }

  delay(1000);
}

void send_sms(String message) {
  sim800L.print("AT+CMGS=\"+255752002942\"\r");
  waitForResponse();

  sim800L.print(message);
  sim800L.write(0x1A);
  waitForResponse();
}

void waitForResponse() {
  delay(4000);
  while (sim800L.available()) {
    Serial.println(sim800L.readString());
  }
  sim800L.read();
}
