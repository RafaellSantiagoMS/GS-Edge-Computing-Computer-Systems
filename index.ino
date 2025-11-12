// =================== BIBLIOTECAS ===================
#include <WiFi.h>              // Conexão Wi-Fi
#include <PubSubClient.h>      // Comunicação MQTT
#include <Wire.h>              // Comunicação I2C
#include <Adafruit_GFX.h>      // Biblioteca gráfica
#include <Adafruit_SSD1306.h>  // Controle do display OLED

// =================== CONFIGURAÇÕES WI-FI E MQTT ===================
const char* SSID = "Wokwi-GUEST";           // Nome da rede Wi-Fi
const char* PASSWORD = "";                  // Senha da rede (vazia no Wokwi)
const char* BROKER_MQTT = "18.228.235.47";  // IP da instância AWS (broker MQTT)
const int BROKER_PORT = 1883;               // Porta padrão MQTT
const char* TOPICO_PUBLISH = "/GS/jogo/attrs"; // Tópico para enviar dados
const char* ID_MQTT = "gs_gamificacao_01";  // Identificação única do cliente MQTT

// =================== DISPLAY OLED ===================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =================== PINOS ===================
#define LED_RED 23
#define LED_GREEN 22
#define LED_BLUE 21
#define PIR_PIN 33
#define BUTTON_PIN 12
#define LDR_PIN 34

// =================== VARIÁVEIS DO JOGO ===================
int pontos = 0;                   // Armazena a pontuação do jogador
bool jogoAtivo = false;           // Indica se o jogo está em andamento
unsigned long inicioTempo;        // Marca o início da rodada
unsigned long tempoLimite;        // Define tempo máximo de cada rodada
unsigned long tempoReacao = 0;    // Armazena o tempo de reação do jogador
bool movimentoDetectado = false;  // Flag de movimento detectado

// =================== OBJETOS DE REDE ===================
WiFiClient espClient;             // Cliente Wi-Fi
PubSubClient MQTT(espClient);     // Cliente MQTT

// =================== FUNÇÕES AUXILIARES ===================

// Conecta ao Wi-Fi
void conectarWiFi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// Garante conexão com o broker MQTT
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("* Tentando conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
      MQTT.publish(TOPICO_PUBLISH, "status:on");
    } else {
      Serial.println("Falha ao conectar. Nova tentativa em 2s...");
      delay(2000);
    }
  }
}

