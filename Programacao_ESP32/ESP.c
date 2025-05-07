#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// Configurações do DHT22
#define DHTPIN 4       // GPIO do sensor
#define DHTTYPE DHT22  // Tipo de sensor
DHT dht(DHTPIN, DHTTYPE);

// Criar um objeto para o ADS1115
Adafruit_ADS1115 ads;

// Pinos para controle
#define RELAY_PIN 13  // Pino do relé (lâmpada)
#define LED_AC_ON 12  // LED indicando AC ligado
#define LED_AC_OFF 14 // LED indicando AC desligado

// Configurações de WiFi
const char* ssid = "Fabio";
const char* password = "F0023339";

// Configurações do MQTT (Mosquitto)
const char* mqttServer = "56.124.54.49";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* mqttTempTopic = "sensor_temp";
const char* mqttHumTopic = "sensor_umid";
const char* mqttLdrTopic = "sensor_luz";
const char* mqttRelayTopic = "lampada/comando";
const char* mqttAc1StatusTopic = "arcondicionado_1/status";
const char* mqttAc2StatusTopic = "arcondicionado_2/status";

// Variáveis globais
WiFiClient espClient;
PubSubClient mqttClient(espClient);
float temperature = 0.0;
float humidity = 0.0;
int16_t ldrValue = 0;

// Controle de tempo
unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 2000;

void connectToWiFi() {
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConectado ao WiFi!");
}

void connectToMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (mqttClient.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado!");
      mqttClient.subscribe(mqttRelayTopic);
    } else {
      Serial.print("Falha na conexão, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Mensagem recebida no tópico ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (String(topic) == mqttRelayTopic) {
    if (message == "ON") {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Lâmpada ligada");
    } else if (message == "OFF") {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Lâmpada desligada");
    }
  }
}

void publishToMQTT() {
  char message[50];

  snprintf(message, sizeof(message), "%.2f", temperature);
  mqttClient.publish(mqttTempTopic, message);

  snprintf(message, sizeof(message), "%.2f", humidity);
  mqttClient.publish(mqttHumTopic, message);

  snprintf(message, sizeof(message), "{\"ldrStatus\":\"%s\"}", (ldrValue > 14500) ? "Lâmpada APAGADA" : "Lâmpada ACESA");
  mqttClient.publish(mqttLdrTopic, message);

  snprintf(message, sizeof(message), "{\"arcondicionado1\":\"%s\"}", digitalRead(LED_AC_ON) == HIGH ? "Ligado" : "Desligado");
  mqttClient.publish(mqttAc1StatusTopic, message);

  snprintf(message, sizeof(message), "{\"arcondicionado2\":\"%s\"}", digitalRead(LED_AC_ON) == HIGH ? "Ligado" : "Desligado");
  mqttClient.publish(mqttAc2StatusTopic, message);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(21, 22);
  if (!ads.begin(0x49)) {
    Serial.println("Falha ao inicializar o ADS1115!");
    while (1);
  }

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_AC_ON, INPUT);
  pinMode(LED_AC_OFF, INPUT);
  
  connectToWiFi();
  mqttClient.setCallback(mqttCallback);
  connectToMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();

  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= publishInterval) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    ldrValue = ads.readADC_SingleEnded(3);
    
    if (!isnan(temperature) && !isnan(humidity)) {
      publishToMQTT();
    }

    lastPublishTime = currentTime;
  }
  delay(100);
}
