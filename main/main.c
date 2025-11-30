#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "st7789.h"
#include "fontx.h"
#include "decode_png.h"
#include "openweather.h"
#include "wifi_manager.h"
#include "moon_phase.h"
#include "time.h"
#include "esp_sntp.h"
#include "freertos/semphr.h"
SemaphoreHandle_t lcd_mutex;

#define TAG "MOON_APP"
#define SCREEN_W CONFIG_WIDTH
#define SCREEN_H CONFIG_HEIGHT

// ---------- FONT HELPERS ----------
int get_string_width(FontxFile *fx, char *str) {
    int width = 0;
    uint8_t glyphBuf[FontxGlyphBufSize];
    uint8_t w, h;
    for (int i = 0; str[i] != '\0'; i++) {
        if (GetFontx(fx, (uint8_t)str[i], glyphBuf, &w, &h)) width += w;
        else width += 8;
    }
    return width;
}

// ---------- DAYS COLOR RAINBOW ----------
static uint16_t moon_countdown_color(int days)
{
    if (days >= 25) return PURPLE;
    if (days >= 20) return BLUE;
    if (days >= 15) return GREEN;
    if (days >= 10) return YELLOW;
    if (days >= 5)  return ORANGE;
    return RED;
}

// ---------- MAIN UI DRAW FUNCTION ----------
static void draw_ui(TFT_t *dev, FontxFile *fx_big, FontxFile *fx_small)
{
	xSemaphoreTake(lcd_mutex, portMAX_DELAY);
    const weather_data_t *wx = openweather_get_data();
    if (!wx->valid) return;

    // Clear screen
    lcdFillScreen(dev, BLACK);

    // ----- DATE -----
    time_t raw = (time_t)wx->date_unix;
    struct tm ts;
    localtime_r(&raw, &ts);

    char date_msg[32];
    strftime(date_msg, sizeof(date_msg), "%B %d, %Y", &ts);

    int date_w = get_string_width(fx_big, date_msg);
    lcdDrawString(dev, fx_big, (SCREEN_W - date_w) / 2, 35, (uint8_t*)date_msg, WHITE);

    // ----- MOON IMAGE -----
    float moon_phase_value = wx->moon_phase;
    const char *moon_img = moon_phase_get_image(moon_phase_value);

    pngle_t *png = pngle_new(SCREEN_W, SCREEN_H);
    FILE *fp = fopen(moon_img, "rb");
    if (fp) {
        pngle_set_init_callback(png, png_init);
        pngle_set_draw_callback(png, png_draw);

        uint8_t buf[1024];
        int len;
        while ((len = fread(buf, 1, sizeof(buf), fp)) > 0) pngle_feed(png, buf, len);
        fclose(fp);

        float scale = 0.70f;
        int new_w = png->imageWidth * scale;
        int new_h = png->imageHeight * scale;
        int img_x = (SCREEN_W - new_w) / 2;
        int img_y = (SCREEN_H - new_h) / 2 - 20;

        for (int y = 0; y < new_h; y++)
            for (int x = 0; x < new_w; x++)
                lcdDrawPixel(dev, img_x + x, img_y + y,
                    png->pixels[(int)(y/scale)][(int)(x/scale)]);

        pngle_destroy(png, SCREEN_W, SCREEN_H);

        // ----- COUNTDOWN -----
        float lunar_cycle = 29.53f;
        float dist = fmodf((0.5f - moon_phase_value + 1.0f), 1.0f);
        int days = (int)roundf(dist * lunar_cycle);

        char num_str[8];
        snprintf(num_str, sizeof(num_str), "%d", days);

        uint16_t countdown_color = moon_countdown_color(days);

        int num_y = img_y + new_h + 30;
        int num_w = get_string_width(fx_big, num_str);
        lcdDrawString(dev, fx_big, (SCREEN_W - num_w) / 2, num_y, (uint8_t*)num_str, countdown_color);

        char text_str[32];
        if (days == 0) strcpy(text_str, "FULL MOON TODAY");
        else if (days == 1) strcpy(text_str, "day until Full Moon");
        else strcpy(text_str, "days until Full Moon");

        int text_w = get_string_width(fx_small, text_str);
        lcdDrawString(dev, fx_small, (SCREEN_W - text_w) / 2, num_y + 20, (uint8_t*)text_str, WHITE);
    }

    // ----- TEMP -----
    char temp_msg[16];
    snprintf(temp_msg, sizeof(temp_msg), "%.0f F", wx->temp);
    lcdDrawString(dev, fx_small, 10, SCREEN_H - 20, (uint8_t*)temp_msg, APRICOT);

    // ----- TIME -----
    time_t now = time(NULL);
    struct tm ts_now;
    localtime_r(&now, &ts_now);
    char time_msg[16];
    strftime(time_msg, sizeof(time_msg), "%I:%M %p", &ts_now);
    int time_w = get_string_width(fx_small, time_msg);
    lcdDrawString(dev, fx_small, SCREEN_W - time_w - 10, SCREEN_H - 20, (uint8_t*)time_msg, MOONLIGHT_GRAY);

	xSemaphoreGive(lcd_mutex);
}

// ------ draw temp and time ------
static void draw_temp_and_time(TFT_t *dev, FontxFile *fx_small)
{
	xSemaphoreTake(lcd_mutex, portMAX_DELAY);

    const weather_data_t *wx = openweather_get_data();
    if (!wx->valid) return;

    // Clear only the bottom bar region (adjust height as needed)
    int bar_y = SCREEN_H - 35;
    lcdDrawFillRect(dev, 0, bar_y, SCREEN_W - 1, SCREEN_H - 1, BLACK);
;

    // ----- TEMP -----
    char temp_msg[16];
    snprintf(temp_msg, sizeof(temp_msg), "%.0f F", wx->temp);
    lcdDrawString(dev, fx_small, 10, SCREEN_H - 20, (uint8_t*)temp_msg, APRICOT);

    // ----- TIME -----
    time_t now = time(NULL);
    struct tm ts_now;
    localtime_r(&now, &ts_now);
    char time_msg[16];
    strftime(time_msg, sizeof(time_msg), "%I:%M %p", &ts_now);

    int time_w = get_string_width(fx_small, time_msg);
    lcdDrawString(dev, fx_small, SCREEN_W - time_w - 10, SCREEN_H - 20,
                  (uint8_t*)time_msg, MOONLIGHT_GRAY);

	xSemaphoreGive(lcd_mutex);			  
}


// ---------- TASKS ----------
void update_daily_task(void *arg)
{
    TFT_t *dev = (TFT_t*)arg;

    FontxFile fx_big[2];
    FontxFile fx_small[2];
    InitFontx(fx_big, "/spiffs/ILGH24XB.FNT", "");
    InitFontx(fx_small, "/spiffs/ILGH16XB.FNT", "");

    while (1) {
        // fetch new moon phase + date
        openweather_update_daily();

        // redraw full screen once per day
        draw_ui(dev, fx_big, fx_small);

        // calculate next midnight
        time_t now = time(NULL);
        struct tm tm;
        localtime_r(&now, &tm);
        tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 5;
        time_t next_midnight = mktime(&tm) + 24 * 3600;

        vTaskDelay(pdMS_TO_TICKS((next_midnight - now) * 1000));
    }
}

void update_current_task(void *arg)
{
	vTaskDelay(pdMS_TO_TICKS(3000)); // prevent boot storm
    while (1) {
        openweather_update_current();
        vTaskDelay(pdMS_TO_TICKS(5 * 60 * 1000));
    }
}

void ui_update_task(void *arg)
{
    TFT_t *dev = (TFT_t*)arg;
    FontxFile fx_big[2];
    FontxFile fx_small[2];
    InitFontx(fx_big, "/spiffs/ILGH24XB.FNT", "");
    InitFontx(fx_small, "/spiffs/ILGH16XB.FNT", "");

    // Draw full UI ONCE at boot
    draw_ui(dev, fx_big, fx_small);

    while (1) {
        // Only redraw temp + time, NOT the moon image or background
        draw_temp_and_time(dev, fx_small);
        vTaskDelay(pdMS_TO_TICKS(60 * 1000)); // update every minute
    }
}

// ---------- MAIN ----------
void app_main(void) {
    ESP_LOGI(TAG, "Starting...");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 10,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    wifi_manager_connect("{{SSID}}", "{{WIFI-PASSWORD}}");
	// ✅ Network is now up — now sync time (needed for HTTPS)
	ESP_LOGI(TAG, "⏳ Syncing time...");
	esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
	esp_sntp_setservername(0, "pool.ntp.org");
	esp_sntp_init();

	while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
		vTaskDelay(pdMS_TO_TICKS(250));
	}

	ESP_LOGI(TAG, "✅ Time synced. Fetching initial weather...");

	// Immediately fetch both weather sets on boot
	openweather_update_daily();
	openweather_update_current();

	// Wait for system time to sync (handled automatically by WiFi & SNTP)
	setenv("TZ", "CST6CDT,M3.2.0,M11.1.0", 1);
	tzset();
	ESP_LOGI(TAG, "⏳ Waiting for time sync...");
	time_t now = 0;
	struct tm timeinfo = {0};
	while (timeinfo.tm_year < (2016 - 1900)) {  // year still invalid
		time(&now);
		localtime_r(&now, &timeinfo);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	ESP_LOGI(TAG, "✅ Time synced.");


    static TFT_t dev;
    spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
    lcdInit(&dev, SCREEN_W, SCREEN_H, CONFIG_OFFSETX, CONFIG_OFFSETY);

	lcd_mutex = xSemaphoreCreateMutex();

	xTaskCreate(update_daily_task, "update_daily_task", 4096, &dev, 5, NULL);
	xTaskCreate(update_current_task, "update_current_task", 4096, NULL, 5, NULL);
	xTaskCreate(ui_update_task, "ui_update_task", 8192, &dev, 5, NULL);

}
