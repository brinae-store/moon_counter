#include "moon_phase.h"

/*
 * OpenWeather moon_phase value:
 * 0.0 = New Moon
 * 0.5 = Full Moon
 * 1.0 = New Moon (cycle restart)
 */

const char* moon_phase_get_image(float m)
{
    // Normalize boundaries (just in case)
    if (m < 0.0f) m = 0.0f;
    if (m > 1.0f) m = 1.0f;

    if (m < 0.0625f) return "/spiffs/new_moon.png";
    if (m < 0.1875f) return "/spiffs/waxing_crescent.png";
    if (m < 0.3125f) return "/spiffs/first_quarter.png";
    if (m < 0.4375f) return "/spiffs/waxing_gibbous.png";
    if (m < 0.5625f) return "/spiffs/full_moon.png";
    if (m < 0.6875f) return "/spiffs/waning_gibbous.png";
    if (m < 0.8125f) return "/spiffs/last_quarter.png";
    return "/spiffs/waning_crescent.png";
}
