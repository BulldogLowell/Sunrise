
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

//********************** utility functions **********************//
constexpr uint32_t SECONDS_TO_MILLIS(uint32_t seconds) {
  return seconds * 1000UL;
}

class MillisIntervalCallback {
  using funcPtr = void(*)(void);
  public:
    MillisIntervalCallback(funcPtr func){
      _callback = func;
    }
    void operator () (uint32_t interval) {
      if (millis() - _lastMillis > interval) {
        if (_callback) {
          _callback();
        }
        _lastMillis = millis();
      }
    }
    void operator ()() {
      if (_callback) {
        _callback();
      }
      _lastMillis = millis();
    }
  private:
    funcPtr _callback;
    uint32_t _lastMillis;
};
//******************** end utility functions ********************//

//******************** function declarations ********************//
void updateLocalTime(void);
void updateSolarVariables(void);
void updateDST(void);
void printSolarTimes(void);
bool IsDST(int dayOfMonth, int month, int dayOfWeek);
void manageLED(void);
int handleTwilightChange(String command);
//****************** end function declarations ******************//

MillisIntervalCallback syncronizeRTC([&](){Particle.syncTime();});
MillisIntervalCallback timeCheck(updateLocalTime);
MillisIntervalCallback dstCheck(updateDST);
MillisIntervalCallback solarTimeCheck(updateSolarVariables);
MillisIntervalCallback printTimes(printSolarTimes);
MillisIntervalCallback ledCheck(manageLED);

//Sunrise sydney(-33.8688, 151.2093); //sydney, Australia
Sunrise princeton(40.3571, -74.6702);  // ACTUAL is default
// or
// Sunrise princeton(40.3571, -74.6702, ACTUAL);
// Sunrise princeton(40.3571, -74.6702, CIVIL);
// Sunrise princeton(40.3571, -74.6702, NAUTICAL);
// Sunrise princeton(40.3571, -74.6702, ASTRONOMICAL);

char currentTime[6] = "N/A";
char sunrise[6] = "N/A";
char noon[6] = "N/A";
char sunset[6] = "N/A";
char civilSunrise[6] = "N/A";
char civilSunset[6] = "N/A";
char nauticalSunrise[6] = "N/A";
char nauticalSunset[6] = "N/A";
char astronomicalSunrise[6] = "N/A";
char astronomicalSunset[6] = "N/A";
char daytimeStr[6] = "N/A";
char twilightStr[16] = "N/A";

double currentOffset = -99.0;
char daylightSavingsStr[6] = "FALSE";

Twilight ledTwilight = ACTUAL;

void setup() {
  Serial.begin(9600);
  pinMode(D7, OUTPUT);

  Particle.variable("time", currentTime);
  Particle.variable("ActualRise", sunrise);
  Particle.variable("SolarNoon", noon);
  Particle.variable("ActualSet", sunset);
  Particle.variable("CivilRise", civilSunrise);
  Particle.variable("CivilSet", civilSunset);
  Particle.variable("NauticalRise", nauticalSunrise);
  Particle.variable("NauticalSet", nauticalSunset);
  Particle.variable("AstroRise", astronomicalSunrise);
  Particle.variable("AstroSet", astronomicalSunset);
  Particle.variable("DST", daylightSavingsStr);
  Particle.variable("Offset", currentOffset);
  Particle.variable("IsDaytime", daytimeStr);
  Particle.variable("Twilight", twilightStr);

  Particle.function("SetTwilight", handleTwilightChange);

  EEPROM.get(0, ledTwilight);
  waitFor(Time.isValid, SECONDS_TO_MILLIS(15));
  // princeton.setTwilight(ACTUAL);  // optional setter
  Time.zone(-5);
  dstCheck();
  timeCheck();
  solarTimeCheck();
  ledCheck();
}

void loop() {
  timeCheck(SECONDS_TO_MILLIS(1));            // every second
  solarTimeCheck(SECONDS_TO_MILLIS(1));       // every second
  ledCheck(SECONDS_TO_MILLIS(1));             // every second
  printTimes(SECONDS_TO_MILLIS(10));          // every 10 seconds
  dstCheck(SECONDS_TO_MILLIS(3600));          // once an hour
  syncronizeRTC(SECONDS_TO_MILLIS(3600 * 4)); //every few hours
}

void manageLED(void) {
  static bool lastDayCheck = false;
  static bool startup = true;
  bool nowDayCheck = princeton.isDay(ledTwilight);  // passing argument means isDay() includes passed Twilight
  if (lastDayCheck != nowDayCheck or startup) {
    startup = false;
    char pubString[64] = "";
    if (nowDayCheck) {
      digitalWrite(D7, LOW);
      strcpy(daytimeStr, "TRUE");
      sprintf(pubString, "Sunrise: LED is off at %02d:%02d", Time.hour(), Time.minute());
      Particle.publish("pushover", pubString);
    } else {
      digitalWrite(D7, HIGH);
      strcpy(daytimeStr, "FALSE");
      sprintf(pubString, "Sunset: LED is on at %02d:%02d", Time.hour(), Time.minute());
      Particle.publish("pushover", pubString);
    }
    lastDayCheck = nowDayCheck;
  }
}

int handleTwilightChange(String command) {
  if (command == "ACTUAL") {
    ledTwilight = ACTUAL;
    EEPROM.put(0, ledTwilight);
    return static_cast<int>(ACTUAL);
  } else if (command == "CIVIL") {
    ledTwilight = CIVIL;
    EEPROM.put(0, ledTwilight);
    return static_cast<int>(CIVIL);
  } else if (command == "NAUTICAL") {
    ledTwilight = NAUTICAL;
    EEPROM.put(0, ledTwilight);
    return static_cast<int>(NAUTICAL);
  } else if (command == "ASTRONOMICAL") {
    ledTwilight = ASTRONOMICAL;
    EEPROM.put(0, ledTwilight);
    return static_cast<int>(ASTRONOMICAL);
  }
  return -1;
}

void updateLocalTime(void) {
  currentOffset = (Time.local() - Time.now()) / 3600.0;  // debug to make sure that DST offset is correct
  Serial.printf("Current Time:%02d:%02d\n",Time.hour(), Time.minute());
  sprintf(currentTime, "%02d:%02d", Time.hour(), Time.minute());
  char currentTwilight[16] = "";
  switch (ledTwilight) {
    case ACTUAL:
      sprintf(twilightStr,"ACTUAL");
      break;
    case CIVIL:
      sprintf(twilightStr, "CIVIL");
      break;
    case NAUTICAL:
      sprintf(twilightStr, "NAUTICAL");
      break;
    case ASTRONOMICAL:
      sprintf(twilightStr,"ASTRONOMICAL");
      break;
  }
}

void updateSolarVariables(void) {
  princeton.updateSolarTimes();  // default as in your constructor or setter
  sprintf(sunrise, "%02d:%02d", princeton.sunRiseHour, princeton.sunRiseMinute);
  sprintf(noon, "%02d:%02d", princeton.solarNoonHour, princeton.solarNoonMinute);
  sprintf(sunset, "%02d:%02d", princeton.sunSetHour, princeton.sunSetMinute);

  princeton.updateSolarTimes(CIVIL);  // or plug your Twilight
  sprintf(civilSunrise, "%02d:%02d", princeton.sunRiseHour, princeton.sunRiseMinute);
  sprintf(civilSunset, "%02d:%02d", princeton.sunSetHour, princeton.sunSetMinute);

  princeton.updateSolarTimes(NAUTICAL);
  sprintf(nauticalSunrise, "%02d:%02d", princeton.sunRiseHour, princeton.sunRiseMinute);
  sprintf(nauticalSunset, "%02d:%02d", princeton.sunSetHour, princeton.sunSetMinute);

  princeton.updateSolarTimes(ASTRONOMICAL);
  sprintf(astronomicalSunrise, "%02d:%02d", princeton.sunRiseHour, princeton.sunRiseMinute);
  sprintf(astronomicalSunset, "%02d:%02d", princeton.sunSetHour, princeton.sunSetMinute);
}

void printSolarTimes(void) {
  princeton.updateSolarTimes(ACTUAL);
  Serial.printf("Actual Sunrise:\t%02d:%02d\n", princeton.sunRiseHour, princeton.sunRiseMinute);
  Serial.printf("Actual Solar Noon:\t%02d:%02d\n", princeton.solarNoonHour, princeton.solarNoonMinute);
  Serial.printf("Actual Sunset:\t%02d:%02d\n\n", princeton.sunSetHour, princeton.sunSetMinute);

  princeton.updateSolarTimes(CIVIL);
  Serial.printf("Civil Sunrise:\t%02d:%02d\n", princeton.sunRiseHour, princeton.sunRiseMinute);
  Serial.printf("Civil Sunset:\t%02d:%02d\n\n", princeton.sunSetHour, princeton.sunSetMinute);

  princeton.updateSolarTimes(NAUTICAL);
  Serial.printf("Nautical Sunrise:\t%02d:%02d\n", princeton.sunRiseHour, princeton.sunRiseMinute);
  Serial.printf("Nautical Sunset:\t%02d:%02d\n\n", princeton.sunSetHour, princeton.sunSetMinute);

  princeton.updateSolarTimes(ASTRONOMICAL);
  Serial.printf("Astronomical Sunrise:\t%02d:%02d\n", princeton.sunRiseHour, princeton.sunRiseMinute);
  Serial.printf("Astronomical Sunset:\t%02d:%02d\n\n", princeton.sunSetHour, princeton.sunSetMinute);
}

void updateDST(void) {
  static bool isDST = false;
  bool daylightSavings = IsDST(Time.day(), Time.month(), Time.weekday());
  if (isDST != daylightSavings) {
    if (daylightSavings) {
      Time.beginDST();
      sprintf(daylightSavingsStr, "%s", "TRUE");
    } else {
      Time.endDST();
      sprintf(daylightSavingsStr, "%s", "FALSE");
    }
    isDST = daylightSavings;
  }
}

bool IsDST(int dayOfMonth, int month, int dayOfWeek)  // US Algorithm
{
  if (month < 3 || month > 11) {
    return false;
  }
  if (month > 3 && month < 11) {
    return true;
  }
  int previousSunday = dayOfMonth - dayOfWeek;
  if (month == 3) {
    return previousSunday >= 8;
  }
  return previousSunday <= 0;
}
