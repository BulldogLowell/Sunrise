#include "Particle.h"
#include "Sunrise.h"
#include "math.h"

Sunrise::Sunrise(double latitude, double longitude, Twilight tl) {
  _lat = latitude / RADIANS_TO_DEGREES();
  _lon = (0 - longitude) / RADIANS_TO_DEGREES();
  setTwilight(tl);
  _sun_rise.rs = 1;
  _solar_noon.rs = 2;
  _sun_set.rs = 0;
}

Twilight Sunrise::getTwilight(void) {
  return _twilight;
}

void Sunrise::setTwilight(Twilight twilight) {
  _twilight = twilight;
  switch (_twilight) {
    case ACTUAL:
      _zenith = 1.579522973054868;
      break;
    case ASTRONOMICAL:
      _zenith = 1.884955592153876;
      break;
    case CIVIL:
      _zenith = 1.675516081914556;
      break;
    case NAUTICAL:
      _zenith = 1.780235837034216;
      break;
  };
}

void Sunrise::updateSolarTimes(void) {
  (void) Compute(Time.month(), Time.day(), _sun_rise);  // we are not using the return from Compute
  (void) Compute(Time.month(), Time.day(), _solar_noon);
  (void) Compute(Time.month(), Time.day(), _sun_set);
}

void Sunrise::updateSolarTimes(Twilight tl) {
  Twilight savedTwilight = _twilight;
  setTwilight(tl);
  updateSolarTimes();
  _twilight = savedTwilight;
  setTwilight(_twilight);
}

int Sunrise::Compute(unsigned char  month, unsigned char  day, SunriseTime& eventTime) {
  double y;
  double decl;
  double eqt;
  double ha;
  unsigned char a;
  int doy;
  int minutes;

  // approximate hour;
  a = 6;
  if (eventTime.rs) {
    a = 18;
  }

  // approximate day of year
  doy = ((month - 1) * 30.3) + day - 1;

  // compute fractional year
  y = 1.721420632104e-02 * (doy + (a / 24.0));

  // compute equation of time
  eqt = 229.18 * ((0.000075 + (0.001868 * cos(y)))  - (0.032077 * sin(y)) - (0.014615 * cos(y * 2)) - (0.040849 * sin(y * 2)) );

  // compute solar declination
  decl = 0.006918 - (0.399912 * cos(y)) + (0.070257 * sin(y)) - (0.006758 * cos(y * 2)) + (0.000907 * sin(y * 2)) - (0.002697 * cos(y * 3) + .00148 * sin(y * 3));

  //compute hour angle
  ha = ((cos(_zenith) / (cos(_lat) * cos(decl))) - (tan(_lat) * tan(decl)));
  if (fabs(ha) > 1) {// we're in the (ant)arctic and there is no rise(or set) today!
    eventTime.hour = 255;
    return - 1;
  }

  ha = acos(ha);
  if (eventTime.rs == 0) {
    ha = (-ha);
  }

  if (eventTime.rs < 2) {
    // computes minutes into the day of the event
    minutes = 720 + (4 * (_lon - ha) * RADIANS_TO_DEGREES()) - eqt;

    // convert from UTC back to our timezone
    minutes += ((Time.local() - Time.now()) / 3600) * 60;
    if (minutes < 0) {
      minutes += 1440;
    }
    minutes %= 1440;

    // stuff hour into month, minute into day
    eventTime.hour = minutes / 60;
    eventTime.minute = minutes - (eventTime.hour * 60);

    return minutes;
  } else if (eventTime.rs == 2) {
    // computes minutes into the day of the event
    minutes = 720 + (4 * _lon * RADIANS_TO_DEGREES()) - eqt;

    // convert from UTC back to our timezone
    minutes += ((Time.local() - Time.now()) / 3600) * 60;
    if (minutes < 0) {
      minutes += 1440;
    }
    minutes %= 1440;

    // stuff hour into month, minute into day
    eventTime.hour = minutes / 60;
    eventTime.minute = minutes - (eventTime.hour * 60);

    return minutes;
  }
}

bool Sunrise::isDay(void) {
  time_t now_time = tmConvert_t(Time.year(), Time.month(), Time.day(), Time.hour(), Time.minute(), Time.second());
  time_t rise_time = tmConvert_t(Time.year(), Time.month(), Time.day(), _sun_rise.hour, _sun_rise.minute, 0);
  time_t set_time = tmConvert_t(Time.year(), Time.month(), Time.day(), _sun_set.hour, _sun_set.minute, 0);
  // this is being done to acomodate UTC
  if (rise_time < set_time) {
    return (now_time > rise_time and now_time < set_time);
  } else if (set_time < rise_time) {
    return (now_time > rise_time or now_time < set_time);
  }
  return false;
}

bool Sunrise::isDay(Twilight tl) {
  Twilight savedTwilight = _twilight;
  setTwilight(tl);
  bool result = isDay();
  _twilight = savedTwilight;
  setTwilight(_twilight);
  return result;
}

// bool isDay(Twilight riseTwilight, Twilight setTwilight) {
//   bool riseDay = isDay(riseTwilight);
//   bool setDay = isDay(setTwilight);
//   return false;
// }

bool Sunrise::isNight(void) {
  return !isDay();
}
bool Sunrise::isNight(Twilight tl) {
  return !isDay(tl);
}

time_t Sunrise::tmConvert_t(uint16_t YYYY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss) {
  struct tm t;
  t.tm_year = YYYY-1900;
  t.tm_mon = MM - 1;
  t.tm_mday = DD;
  t.tm_hour = hh;
  t.tm_min = mm;
  t.tm_sec = ss;
  t.tm_isdst = 0;
  time_t t_of_day = mktime(&t);
  return t_of_day;
}

