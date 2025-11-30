#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_manager_connect(const char *ssid, const char *pass);

#ifdef __cplusplus
}
#endif

#endif // WIFI_MANAGER_H
