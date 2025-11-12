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

