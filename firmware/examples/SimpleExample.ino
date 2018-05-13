/*
  Sunrise Library
  Adapted for Particle Devices May - 2018
  Copyright May, 2018
  Jim Brower
  bulloglowell@gmail.com

  Twilight definitions:
  ACTUAL is the moment the Sun sets completely below the horizon
  CIVIL is the moment that the center of the sun is 6° below the horizon
  NAUTICAL is the moment that the center of the sun is 12° below the horizon
  ASTRONOMICAL is the moment that the center of the sun is 18° below the horizon

  for more on CIVIL, NAUTICAL and ASTRONOMICAL twilight, see the following:
  https://en.wikipedia.org/wiki/Twilight

  check your times here:
  https://sunrise-sunset.org
  or as this example:
  https://sunrise-sunset.org/us/princeton-nj

  Originally Derived From:
  http://www.swfltek.com/arduino/sunrise
  (but seems to now be defunct...)
*/

#include "Sunrise.h"
#include "math.h"

//Sunrise sydney(-33.8688, 151.2093); //sydney, Australia
Sunrise princeton(40.3571, -74.6702);  // ACTUAL is default
// or
// Sunrise princeton(40.3571, -74.6702, ACTUAL);
// Sunrise princeton(40.3571, -74.6702, CIVIL);
// Sunrise princeton(40.3571, -74.6702, NAUTICAL);
// Sunrise princeton(40.3571, -74.6702, ASTRONOMICAL);

void setup() {
  Serial.begin(9600);
  pinMode(D7, OUTPUT);
  Time.zone(-4); // summer but this library works in UTC time as well!
  // princeton.setTwilight(ACTUAL);  // optional setter
}

void loop() {
  static uint32_t lastMillis = 0;
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    printSolarTimes();
  }

  static int lastCivilDayStatus = -1;
  int civilDayStatus = princeton.isDay(CIVIL)? 1 : 0;
  if (civilDayStatus != lastCivilDayStatus) {
    if(civilDayStatus == 1) {
      digitalWrite(D7, LOW);
    } else {
      digitalWrite(D7, HIGH);
    }
    lastCivilDayStatus = civilDayStatus;
  }
}

void printSolarTimes(void) {
  Serial.printf("Current Time:\t\t%02d:%02d\n", Time.hour(), Time.minute());

  princeton.updateSolarTimes();  // default
  Serial.printf("Actual Sunrise:\t\t%02d:%02d\n", princeton.sunRiseHour, princeton.sunRiseMinute);
  Serial.printf("Actual Solar Noon:\t%02d:%02d\n", princeton.solarNoonHour, princeton.solarNoonMinute);
  Serial.printf("Actual Sunset:\t\t%02d:%02d\n", princeton.sunSetHour, princeton.sunSetMinute);
  Serial.printf("It is currently %stime\n\n", princeton.isDay()? "day"  : "night");

  princeton.updateSolarTimes(CIVIL);
  Serial.printf("Civil Sunrise:\t\t%02d:%02d\n", princeton.sunRiseHour, princeton.sunRiseMinute);
  Serial.printf("Civil Sunset:\t\t%02d:%02d\n", princeton.sunSetHour, princeton.sunSetMinute);
  Serial.printf("It is currently Civil %stime\n\n", princeton.isDay(CIVIL)? "day"  : "night");

  princeton.updateSolarTimes(NAUTICAL);
  Serial.printf("Nautical Sunrise:\t%02d:%02d\n", princeton.sunRiseHour, princeton.sunRiseMinute);
  Serial.printf("Nautical Sunset:\t%02d:%02d\n", princeton.sunSetHour, princeton.sunSetMinute);
  Serial.printf("It is currently Nautical %stime\n\n", princeton.isDay(NAUTICAL)? "day"  : "night");

  princeton.updateSolarTimes(ASTRONOMICAL);
  Serial.printf("Astronomical Sunrise:\t%02d:%02d\n", princeton.sunRiseHour, princeton.sunRiseMinute);
  Serial.printf("Astronomical Sunset:\t%02d:%02d\n", princeton.sunSetHour, princeton.sunSetMinute);
  Serial.printf("It is currently Astronomical %stime\n\n", princeton.isDay(ASTRONOMICAL)? "day"  : "night");
}
