#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "esp_http_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"

#include "protocol_examples_common.h"
#include "protocol_examples_utils.h"

#include "lvgl.h"
#include "iot_button.h"

#include "wifi_sntp.h"
#include "esp_sparkbot_bsp.h"
#include "bsp_board_extra.h"

#include "cJSON.h"
#include "dirent.h"

#include "ui.h"
#include "app_imu.h"
#include "app_wifi.h"
#include "app_weather.h"
#include "app_animation.h"

#include "duer_audio_wrapper.h"
#include "dueros_app.h"

#include "audio_idf_version.h"
#include "app_power.h"

#include "esp_camera.h"

static const char *TAG = "main";
static void button_handler(touch_button_handle_t out_handle, touch_button_message_t *out_message, void *arg)
{
    (void) out_handle; //Unused
    lv_obj_t *current_screen = lv_disp_get_scr_act(NULL);
    int button = (int)arg;

    if (out_message->event == TOUCH_BUTTON_EVT_ON_PRESS) {
        ESP_LOGI(TAG, "Button[%d] Press", (int)arg);
        switch (button) {
        case 1:
            if (current_screen == ui_muyuplay) {
                voice_muyu_notify();
            }
            ui_send_sys_event(current_screen, LV_EVENT_PRESSED, NULL);
            break;
        case 2:
            ui_send_sys_event(current_screen, LV_EVENT_SCREEN_PRIVIOUS, NULL);
            break;
        case 3:
            ui_send_sys_event(current_screen, LV_EVENT_SCREEN_NEXT, NULL);
            break;
        default:
            break;
        }
    } else if (out_message->event == TOUCH_BUTTON_EVT_ON_RELEASE) {
        ESP_LOGI(TAG, "Button[%d] Release", (int)arg);
    } else if (out_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
        ESP_LOGI(TAG, "Button[%d] LongPress", (int)arg);
        switch (button) {
        case 1:
            ui_send_sys_event(current_screen, LV_EVENT_LONG_PRESSED, NULL);
            break;
        default:
            break;
        }
    }
}

static void button_long_press_cb(void *arg,void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_LONG_PRESS_START");
    // int32_t profile_state = duer_profile_certified();
    // bool wifi_configured = wifi_has_cfg_saved();
    // ESP_LOGI(TAG, "profile state %d, wifi_configured %d", (int)profile_state, (int)wifi_configured);
    // if (profile_state == 1 || profile_state == 2) {
    //     ESP_LOGW(TAG, "\nPlease fill ${ADF_PATH}/components/dueros_service/duer_profile and enable CONFIG_DUEROS_GEN_PROFILE & CONFIG_DUEROS_FLASH_PROFILE in sdkconfig\n");
    // }
    // if (!wifi_configured || profile_state == 3) {
    //     wifi_setting_flag = wifi_cfg_start() == ESP_OK ? true : false;
    // } else if (wifi_configured && profile_state == 0) {
    //     wifi_service_connect(wifi_serv);
    // }
}

static void button_short_press_cb(void *arg,void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_PRESS_UP");
    esp_restart();
}

#include "audio_mem.h"
#include "audio_sys.h"

void memory_monitor()
{
    static char buffer[128];    /* Make sure buffer is enough for `sprintf` */
    while (1) {
        AUDIO_MEM_SHOW(TAG);
        audio_sys_get_real_time_stats();
        vTaskDelay(5000);
    }
}

void app_main(void)
{
    /* Initialize NVS. */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    /* Create GPIO button to reset Wi-Fi */
    button_config_t gpio_btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
        .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
        .gpio_button_config = {
            .gpio_num = 0,
            .active_level = 0,
        },
    };
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);

    if( NULL == gpio_btn ) {
        ESP_LOGE(TAG, "Button create failed");
    }

    iot_button_register_cb(gpio_btn, BUTTON_LONG_PRESS_START, button_long_press_cb, NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, button_short_press_cb, NULL);

    /**
     * @brief Connect to the network
     */
    app_animation_start();

    // bsp_i2c_init();

    bsp_touch_button_create(button_handler);

    esp_netif_init();

    duer_app_init();

    // app_weather_start();

    // app_network_start();

    // send_network_event(NET_EVENT_WEATHER);

    wifi_sntp_start();

    bsp_i2c_init();

    // power_adc_init();
    app_imu_init();
    /* Monitor free heap */
    // memory_monitor();
}
