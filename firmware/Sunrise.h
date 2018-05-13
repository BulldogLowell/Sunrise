#ifndef Sunrise_h
#define Sunrise_h

#include "Particle.h"

enum Twilight : uint8_t {
  ACTUAL,
  ASTRONOMICAL,
  CIVIL,
  NAUTICAL,
};

class Sunrise{
  protected:
    struct SunriseTime {
      uint8_t rs;
      uint8_t hour;
      uint8_t minute;
    };
    int Compute(unsigned char  month, unsigned char  day, SunriseTime& times);
    double _lat;
    double _lon;
    double _zenith;
    SunriseTime _sun_rise;
    SunriseTime _solar_noon;
    SunriseTime _sun_set;
    Twilight _twilight;

  private:
    static constexpr double RADIANS_TO_DEGREES(void) {
      return 57.295779513082322;
    }
    time_t tmConvert_t(uint16_t YYYY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss);


  public:
    Sunrise(double latitude, double longitude, Twilight tl = ACTUAL);
    void setTwilight(Twilight tl);
    Twilight getTwilight(void);
    void updateSolarTimes(void);
    void updateSolarTimes(Twilight tl);
    bool isDay(void);
    bool isDay(Twilight tl);
    bool isDay(Twilight riseTwilight, Twilight setTwilight);
    bool isNight(void);
    bool isNight(Twilight tl);
    uint8_t& sunRiseHour = _sun_rise.hour;
    uint8_t& sunRiseMinute = _sun_rise.minute;
    uint8_t& solarNoonHour = _solar_noon.hour;
    uint8_t& solarNoonMinute = _solar_noon.minute;
    uint8_t& sunSetHour = _sun_set.hour;
    uint8_t& sunSetMinute = _sun_set.minute;
};

#endif
