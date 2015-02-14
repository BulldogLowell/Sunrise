/* Short explanation
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
Sunrise winterSunrise(40.3571,-74.6702,-5);//Princeton, New Jersey, USA - Latitude/Longitude and Timezone 	38.79/-9.12, 0
Sunrise winterSunrise(40.3571,-74.6702,-4);//Princeton, New Jersey, USA - Latitude/Longitude and Timezone 	38.79/-9.12, 0

void setup(){
  Serial.begin(9600);
  winterSunrise.Astronomical(); //Actual, Civil, Nautical, Astronomical
}

void loop()
{
  byte h,m;
  int t;
  // t= minutes past midnight of sunrise (6 am would be 360)
  t = winterSunrise.Rise(Time.month(),Time.day()); // (month,day) - january=1
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
  else
  { //
    Serial.println("There are either penguins or polar bears around here!");
  }
  // t= minutes past midnight of sunrise (6 am would be 360)
  t=winterSunrise.Set(Time.month(),Time.day()); // (month,day) - january=1
  if(t>=0)
  {
    h=winterSunrise.sun_Hour();
    m=winterSunrise.sun_Minute();
    Serial.print("The sun sets on Today at ");
    Serial.print(h, DEC);
    Serial.print(":");
    if(m<10) Serial.print("0");
    Serial.println(m,DEC);
  }
  else
  {
    Serial.println("There are either penguins or polar bears around here!");
  }
  // t= minutes past midnight of sunrise (6 am would be 360)
  t=winterSunrise.Noon(Time.month(),Time.day()); // (month,day) - january=1
  if(t >= 0)
  {
    h=winterSunrise.sun_Hour();
    m=winterSunrise.sun_Minute();
    Serial.print("Solar Noon at ");
    Serial.print(h, DEC);
    Serial.print(":");
    if(m<10) Serial.print("0");
    Serial.print(m,DEC);
    Serial.print("   T:");
    //Serial.println(sunNoon, DEC);
  }
  else
  {
    Serial.println("There are either penguins or polar bears around here!");
  }
  delay(5000);
}
