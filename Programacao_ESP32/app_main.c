//Usando o VS Code para programação da ESP32##
//Para conseguir usar o VS Code é necessário nele instalar uma extensão chamada ESPRESSIF IDF e a extensão da linguagem C.
//O código abaixo é um exemplo que faz a conexão da ESP32 em uma rede Wifi de 2.4GHz. Após conectada, é mostrado no terminal o IP obtido pela ESP e com este é possível acessar uma página web e enviar uma mensagem da web para o terminal.


#include <stdio.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_http_server.h>

#define WIFI_SSID "Fabio"       // Substitua pelo seu SSID
#define WIFI_PASS "F0023339"      // Substitua pela sua senha
static const char *TAG = "ESP32_WEB";

// Callback para o manipulador da página principal
esp_err_t root_handler(httpd_req_t *req) {
    const char *response = 
        "<!DOCTYPE html>"
        "<html>"
        "<head><title>ESP32 Web Server</title></head>"
        "<body>"
        "<h1>Controle ESP32</h1>"
        "<form action=\"/submit\" method=\"POST\">"
        "<label for=\"input\">Envie algo:</label>"
        "<input type=\"text\" id=\"input\" name=\"input\">"
        "<input type=\"submit\" value=\"Enviar\">"
        "</form>"
        "</body>"
        "</html>";
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Callback para o manipulador da rota de envio
esp_err_t submit_handler(httpd_req_t *req) {
    char buf[100];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret > 0) {
        buf[ret] = 0; // Finaliza a string
        ESP_LOGI(TAG, "Recebido: %s", buf);

        // Responde ao cliente
        httpd_resp_send(req, "Recebido com sucesso!", HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send_500(req);
    }
    return ESP_OK;
}

// Configura o servidor HTTP
httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    // Inicia o servidor
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root = {
            .uri      = "/",
            .method   = HTTP_GET,
            .handler  = root_handler
        };
        httpd_register_uri_handler(server, &root);

        httpd_uri_t submit = {
            .uri      = "/submit",
            .method   = HTTP_POST,
            .handler  = submit_handler
        };
        httpd_register_uri_handler(server, &submit);

        ESP_LOGI(TAG, "Servidor iniciado");
    }
    return server;
}

// Configura o Wi-Fi
void wifi_init_sta(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Conectando ao Wi-Fi...");
    esp_wifi_connect();
}

// Evento de conexão Wi-Fi
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "IP obtido: " IPSTR, IP2STR(&event->ip_info.ip));
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Reconectando...");
        esp_wifi_connect();
    }
}

void app_main(void) {
    // Inicializa NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // Configura Wi-Fi
    wifi_init_sta();

    // Inicia o servidor web
    start_webserver();
}
