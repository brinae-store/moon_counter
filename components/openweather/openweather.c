#include "openweather.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>
#include "esp_crt_bundle.h"

static const char *TAG = "openweather";
static weather_data_t g_weather = {0};

#define URL_DAILY "https://api.openweathermap.org/data/3.0/onecall?lat=41.88&lon=-87.63&units=imperial&exclude=hourly,minutely&APPID=a937fd4fd5d0d701861163607edda571"
#define URL_CURRENT "https://api.openweathermap.org/data/3.0/onecall?lat=41.88&lon=-87.63&units=imperial&exclude=hourly,minutely&APPID=a937fd4fd5d0d701861163607edda571"




/*
 * Internal function to perform HTTP GET + JSON parse
 */
static esp_err_t my_api_perform_get_request_internal(const char *url)
{
    esp_http_client_config_t config = {
        .url = url,
        .timeout_ms = 5000,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
    .skip_cert_common_name_check = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        return ESP_FAIL;
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return err;
    }

    int content_length = esp_http_client_fetch_headers(client);
    if (content_length <= 0) {
        content_length = 8192; // fallback for chunked
    }

    char *buffer = malloc(content_length + 1);
    if (!buffer) {
        ESP_LOGE(TAG, "OOM allocating buffer");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_ERR_NO_MEM;
    }

    int total_read = 0;
    while (total_read < content_length) {
        int read = esp_http_client_read(client, buffer + total_read, content_length - total_read);
        if (read <= 0) break;
        total_read += read;
    }
    buffer[total_read] = '\0';

    ESP_LOGI(TAG, "Received JSON (%d bytes): %s", total_read, buffer);

    // ---- JSON parse ----
    cJSON *root = cJSON_Parse(buffer);
    if (!root) {
        ESP_LOGE(TAG, "cJSON Parse error");
        free(buffer);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    cJSON *current = cJSON_GetObjectItem(root, "current");
    double current_temp = cJSON_GetObjectItem(current, "temp")->valuedouble;

    cJSON *today = cJSON_GetArrayItem(cJSON_GetObjectItem(root, "daily"), 0);
    double date_unix = cJSON_GetObjectItem(today, "dt")->valuedouble;
    double moon_phase = cJSON_GetObjectItem(today, "moon_phase")->valuedouble;

    g_weather.temp = current_temp;
    g_weather.moon_phase = moon_phase;
    g_weather.date_unix = date_unix;
    g_weather.valid = 1;

    ESP_LOGI(TAG, "Weather Updated | Temp: %.1f°F | Moon Phase: %.2f",
            g_weather.temp, g_weather.moon_phase);


    cJSON_Delete(root);
    free(buffer);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    return ESP_OK;
}

const weather_data_t* openweather_get_data(void)
{
    return &g_weather;

}

void openweather_update_daily(void)
{
    my_api_perform_get_request_internal(URL_DAILY);
}

void openweather_update_current(void)
{
    my_api_perform_get_request_internal(URL_CURRENT);
}

/*
 * FreeRTOS Task Wrapper
 */
static void openweather_task(void *arg)
{
    const char *url = (const char *)arg;
    my_api_perform_get_request_internal(url);
    vTaskDelete(NULL);
}

/*
 * Public function called from app_main()
 */
void openweather_start_request(const char *url)
{
    xTaskCreate(
        openweather_task,
        "openweather_task",
        8192,      // safe stack size
        (void*)url,
        5,         // priority
        NULL
    );
}
