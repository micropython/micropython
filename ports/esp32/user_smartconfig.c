
#include "user_smartconfig.h"

#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"

char WIFI_AP_SSID[32 + 1] = "bit";

static wifi_config_t wifi_sta_config;

static bool smartconfig_mode = false;
static EventGroupHandle_t wifi_event_group;

static const int ESPTOUCH_DONE_BIT = BIT0;
static const int SMARTCONFIG_DONE_BIT = BIT1;
static const char *TAG = "user_smartconfig";

void smartconfig_task(void *parm);

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {

    case SYSTEM_EVENT_STA_START: // wifi STA 开始
        ESP_LOGD(TAG, "[WIFI AP+STA] STA start event!\r\n");
        if (smartconfig_mode)
        {
            xTaskCreate(smartconfig_task, "smartconfig_task", 2048, NULL, 3, NULL);
        }
        else
        {
            ESP_ERROR_CHECK(esp_wifi_connect()); // 连接wifi
        }
        break;
    case SYSTEM_EVENT_STA_GOT_IP: // 分配到了IP
        ESP_LOGD(TAG, "[WIFI AP+STA] Get IP event!\r\n");
        ESP_LOGW(TAG, "[WIFI AP+STA] ESP32 IP: %s !\r\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_CONNECTED: // 连接上了wifi
    {
        wifi_ap_record_t my_wifi_info;
        ESP_LOGD(TAG, "[WIFI AP+STA] Wifi STA connect event!\r\n");
        esp_wifi_sta_get_ap_info(&my_wifi_info); // STA模式下，获取模块连接上的wifi热点的信息
        ESP_LOGW(TAG, "[WIFI AP+STA] Connect to : %s!\r\n", my_wifi_info.ssid);
        break;
    }
    case SYSTEM_EVENT_STA_DISCONNECTED: // 断开了与wifi的连接
        ESP_LOGD(TAG, "[WIFI AP+STA] Wifi STA disconnect event, reconnect!\r\n");
        ESP_ERROR_CHECK(esp_wifi_connect()); // 重新连接wifi
        break;
    case SYSTEM_EVENT_AP_START: // wifi AP 开始
        ESP_LOGD(TAG, "[WIFI AP+STA] AP start event!\r\n");
        break;
    case SYSTEM_EVENT_AP_STACONNECTED: // 有站点（STA）连接上ESP32的AP
        ESP_LOGD(TAG, "[WIFI AP+STA] A station connected to ESP32 soft-AP!\r\n");
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED: // 有站点（STA）与ESP32的AP断开连接
        ESP_LOGD(TAG, "[WIFI AP+STA] A station disconnected from ESP32 soft-AP!\r\n");
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void sc_callback(smartconfig_status_t status, void *pdata)
{
    switch (status)
    {
    case SC_STATUS_WAIT:
        ESP_LOGD(TAG, "SC_STATUS_WAIT");
        break;
    case SC_STATUS_FIND_CHANNEL:
        ESP_LOGD(TAG, "SC_STATUS_FINDING_CHANNEL");
        break;
    case SC_STATUS_GETTING_SSID_PSWD:
        ESP_LOGD(TAG, "SC_STATUS_GETTING_SSID_PSWD");
        break;
    case SC_STATUS_LINK:
        ESP_LOGD(TAG, "SC_STATUS_LINK");
        wifi_config_t *wifi_config = pdata;
        ESP_LOGD(TAG, "SSID:%s", wifi_config->sta.ssid);
        ESP_LOGD(TAG, "PASSWORD:%s", wifi_config->sta.password);
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());
        wifi_sta_config = *wifi_config;
        break;
    case SC_STATUS_LINK_OVER:
        ESP_LOGD(TAG, "SC_STATUS_LINK_OVER");
        if (pdata != NULL)
        {
            uint8_t phone_ip[4] = {0};
            memcpy(phone_ip, (uint8_t *)pdata, 4);
            ESP_LOGD(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
        }
        xEventGroupSetBits(wifi_event_group, ESPTOUCH_DONE_BIT);
        break;
    default:
        break;
    }
}

void smartconfig_task(void *parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    ESP_ERROR_CHECK(esp_smartconfig_start(sc_callback));
    while (true)
    {
        uxBits = xEventGroupWaitBits(wifi_event_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);

        if (uxBits & ESPTOUCH_DONE_BIT)
        {
            ESP_LOGD(TAG, "smartconfig over");
            esp_smartconfig_stop();
            xEventGroupClearBits(wifi_event_group, ESPTOUCH_DONE_BIT);
            xEventGroupSetBits(wifi_event_group, SMARTCONFIG_DONE_BIT);
            vTaskDelete(NULL);
        }
    }
}

#include "esp_spiffs.h"
#include "esp_wifi_types.h"

bool wifi_config_file_init()
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 3,
        .format_if_mount_failed = true};
    if(ESP_OK == esp_vfs_spiffs_register(&conf)) return true;
    esp_spiffs_format(NULL);
    return false;
}

void wifi_config_file_exit()
{
    esp_vfs_spiffs_unregister(NULL);
}

bool wifi_config_file_read(wifi_config_t *config)
{
    bool res = false;
    FILE *f = fopen(SMART_CONFIG_FILE, "rb");
    if (f)
    {
        // ESP_LOGD(TAG, "fopen");
        if (1 == fread(config, sizeof(*config), 1, f))
        {
            // ESP_LOGD(TAG, "fread");
            res = true;
        }
        fclose(f);
    }
    return res;
}

bool wifi_config_file_write(wifi_config_t *config)
{
    bool res = false;
    FILE *f = fopen(SMART_CONFIG_FILE, "wb");
    if (f)
    {
        // ESP_LOGD(TAG, "fopen");
        if (1 == fwrite(config, sizeof(*config), 1, f))
        {
            // ESP_LOGD(TAG, "fwrite");
            res = true;
        }
        fclose(f);
    }
    return res;
}

bool config_default_wifi(void)
{
    bool result = false;
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    /*
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
    wifi_config_t wifi_ap_config = {0};
    wifi_ap_config.ap.ssid_len = strlen(WIFI_AP_SSID);
    memcpy(wifi_ap_config.ap.ssid, WIFI_AP_SSID, strlen(WIFI_AP_SSID));
    wifi_ap_config.ap.max_connection = 1;                                  // 能连接的设备数
    wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;                           //WIFI_AUTH_WPA_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_ap_config)); // AP配置
    */

    bool spiffs = wifi_config_file_init(), config = false;
    ESP_LOGD(TAG, "spiffs wifi_config_file_init result:%d\n", spiffs);

    // exist config not need default smartconfig
    if (spiffs && true == (config = wifi_config_file_read(&wifi_sta_config)))
    {
        ESP_LOGI(TAG, "exist smartconfig config\n");
        wifi_config_t *wifi_config = &wifi_sta_config;
        ESP_LOGD(TAG, "SSID:%s", wifi_config->sta.ssid);
        ESP_LOGD(TAG, "PASSWORD:%s", wifi_config->sta.password);
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
        smartconfig_mode = false;
    }
    else
    {
        ESP_LOGI(TAG, "default smartconfig config\n");
        wifi_config_t sta_config = {
            .sta = {
                .ssid = "tgoffice",
                .password = "tu9u2017",
                .bssid_set = false}};

        // memcpy(sta_config.sta.ssid, WIFI_AP_SSID, strlen(WIFI_AP_SSID));
        // ESP_LOGD(TAG, "SSID:%s", WIFI_AP_SSID);
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    }

    // but user hope changed config
    gpio_set_direction(SMART_CONFIG_KEY, GPIO_MODE_INPUT);
    if (1 == gpio_get_level(SMART_CONFIG_KEY))
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            vTaskDelay(200 / portTICK_PERIOD_MS);
            if (0 == gpio_get_level(SMART_CONFIG_KEY))
            {
                smartconfig_mode = true;
                break;
            }
        }
    }

    ESP_ERROR_CHECK(esp_wifi_start());
    if (smartconfig_mode)
    {
        gpio_set_direction(SMART_CONFIG_LED, GPIO_MODE_OUTPUT);
        gpio_set_level(SMART_CONFIG_LED, 1);

        smartconfig_mode = false;
        ESP_LOGD(TAG, "wait SMARTCONFIG_DONE_BIT");

        EventBits_t uxBits = xEventGroupWaitBits(wifi_event_group, SMARTCONFIG_DONE_BIT, true, false, portMAX_DELAY);

        if (uxBits & SMARTCONFIG_DONE_BIT)
        {
            result = true;
            ESP_LOGD(TAG, "SMARTCONFIG_DONE_BIT");
            if (false == wifi_config_file_write(&wifi_sta_config))
            {
                ESP_LOGD(TAG, "wifi_config_file_write");
                remove(SMART_CONFIG_FILE);
                result = false;
            }
            // esp_restart();
        }
    }

    if (spiffs)
    {
        wifi_config_file_exit();
        ESP_LOGD(TAG, "wifi_config_exit");
    }
    else
    {
        ESP_LOGD(TAG, "spiffs error! need to erase");
    }
    vEventGroupDelete(wifi_event_group);

    gpio_set_level(SMART_CONFIG_LED, 0);
    gpio_reset_pin(SMART_CONFIG_LED);
    return result;
}
