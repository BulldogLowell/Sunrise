/*
 Adapted for Spark Core Feb-2015
 Jim Brower
 bulloglowell@gmail.com
*/
//
/*
 Short explanation
 Create an instance of the Sunrise class
 Pass month and day to Rise() or Set(), returning minutes since midnight of the event
 Read the hour and minute if desired
 See the website for more detailed instructions
 http://www.swfltek.com/arduino/sunrise
 */
#include "application.h"
#include "Sunrise/Sunrise.h"
#include "math.h"

// create a Sunrise object
Sunrise winterSunrise(40.3571,-74.6702, -5);//Princeton, New Jersey, USA - Latitude/Longitude and UTC offset
Sunrise summerSunrise(40.3571, -74.6702,-4);//Princeton, New Jersey, USA - Latitude/Longitude and UTC offset

void setup(){
  Serial.begin(9600);
  winterSunrise.Astronomical(); //Actual, Civil, Nautical, Astronomical
  summerSunrise.Astronomical();
}

void loop()
{
  bool daylightSavings = IsDST(Time.day(), Time.month(), Time.weekday());
  Time.zone(daylightSavings? -4 : -5);
  byte h,m;
  int t;
  // t= minutes past midnight of sunrise
  t = (daylightSavings? winterSunrise.Rise(Time.month(),Time.day()) : winterSunrise.Rise(Time.month(),Time.day())); // (month,day) - january=1
  if(t >= 0)
  {
    h=winterSunrise.sun_Hour();
    m=winterSunrise.sun_Minute();
    Serial.print("The sun rises Today at ");
    Serial.print(h, DEC);
    Serial.print(":");
    if(m<10) Serial.print("0");
    Serial.println(m,DEC);
  }
  // t= minutes past midnight of sunset
  t = (daylightSavings? winterSunrise.Set(Time.month(),Time.day()) : winterSunrise.Set(Time.month(),Time.day())); // (month,day) - january=1
  if(t >= 0)
  {
    h=winterSunrise.sun_Hour();
    m=winterSunrise.sun_Minute();
    Serial.print("The sun sets on Today at ");
    Serial.print(h, DEC);
    Serial.print(":");
    if(m<10) Serial.print("0");
    Serial.println(m,DEC);
  }
  // t= minutes past midnight of solar noon
  t = (daylightSavings? winterSunrise.Noon(Time.month(),Time.day()) : winterSunrise.Noon(Time.month(),Time.day())); // (month,day) - january=1
  if(t >= 0)
  {
    h=winterSunrise.sun_Hour();
    m=winterSunrise.sun_Minute();
    Serial.print("Solar Noon at ");
    Serial.print(h, DEC);
    Serial.print(":");
    if(m<10) Serial.print("0");
    Serial.println(m,DEC);
  }
  else
  {
    Serial.println("There are either penguins or polar bears around here!");
  }
  delay(5000);
}

bool IsDST(int dayOfMonth, int month, int dayOfWeek)  // North American Algorithm
{
  if (month < 3 || month > 11)
  {
    return false;
  }
  if (month > 3 && month < 11)
  {
    return true;
  }
  int previousSunday = dayOfMonth - dayOfWeek;
  //In march, we are DST if our previous sunday was on or after the 8th.
  if (month == 3)
  {
    return previousSunday >= 8;
  }
  //In november we must be before the first sunday to be dst.
  //That means the previous sunday must be before the 1st.
  return previousSunday <= 0;
}
