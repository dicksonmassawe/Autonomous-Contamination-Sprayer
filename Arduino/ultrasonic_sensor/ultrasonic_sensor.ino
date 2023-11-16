// Ultrasonic sensor library
#include <NewPing.h>

// Ultrasonic sensor
#define TRIGGER_PIN_1  3  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_1     5  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PIN_2  9  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_2     6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
int h1; // Distance measured from sensor 1
int h2; // Distance measured from sensor 2

NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  // Initialize the serial monitor
  Serial.begin(9600);

}

void loop() {
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

  delay(1000);
}
