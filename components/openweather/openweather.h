#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void openweather_start_request(const char *url);
void openweather_update_daily(void);
void openweather_update_current(void);


typedef struct {
    double temp;
    double moon_phase;
    double date_unix;
    int    valid;   // 1 = data available, 0 = not ready yet
} weather_data_t;

const weather_data_t* openweather_get_data(void);

#ifdef __cplusplus
}
#endif
