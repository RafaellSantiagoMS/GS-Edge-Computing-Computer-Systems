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

// Envia dados em formato JSON para o broker MQTT
void enviarDadosMQTT(int pontos, unsigned long tempoReacao, String status) {
  String payload = "{";
  payload += "\"pontuacao\": " + String(pontos) + ",";
  payload += "\"tempo_reacao\": " + String(tempoReacao) + ",";
  payload += "\"status\": \"" + status + "\"";
  payload += "}";

  MQTT.publish(TOPICO_PUBLISH, payload.c_str());
  Serial.println("Enviado MQTT: " + payload);
}

// Exibe mensagem simples no display
void mensagem(String texto) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print(texto);
  display.display();
}

// Acende um LED específico (vermelho, verde ou azul)
void acenderLED(int cor) {
  digitalWrite(LED_RED, cor == LED_RED);
  digitalWrite(LED_GREEN, cor == LED_GREEN);
  digitalWrite(LED_BLUE, cor == LED_BLUE);
}

// Mostra mensagem de incentivo conforme desempenho
void mostrarConhecimento(int nivel) {
  display.clearDisplay();
  display.setCursor(0, 10);
  if (nivel == 1) {
    display.print("Curioso! Parte 1:\nAprender IA\ncomeca na base.");
  } else if (nivel == 2) {
    display.print("Bom! Parte 2:\nA pratica leva\nao dominio.");
  } else {
    display.print("Excelente!\nIA transforma\nideias em solucoes!");
  }
  display.display();
}

// =================== LÓGICA PRINCIPAL ===================
void setup() {
  Serial.begin(115200);

  // Configuração dos pinos
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Inicializa o display OLED (pinos SDA = 5, SCL = 4)
  Wire.begin(5, 4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Conecta Wi-Fi e configura MQTT
  conectarWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);

  mensagem("Pressione o botao\npara iniciar!");
}

void loop() {
  // Mantém conexão MQTT ativa
  if (!MQTT.connected()) reconnectMQTT();
  MQTT.loop();

  // Inicia o jogo se o botão for pressionado
  if (!jogoAtivo && digitalRead(BUTTON_PIN) == LOW) {
    iniciarJogo();
  }

  // Enquanto o jogo estiver ativo
  if (jogoAtivo) {
    int ldrValor = analogRead(LDR_PIN);  // Lê luminosidade do ambiente

    // Se o sensor PIR detectar movimento
    if (digitalRead(PIR_PIN) == HIGH && !movimentoDetectado) {
      movimentoDetectado = true;
      tempoReacao = millis() - inicioTempo; // Calcula tempo de reação

      // Pontuação baseada no tempo de reação
      if (tempoReacao < 3000) pontos += 10;
      else if (tempoReacao < 6000) pontos += 7;
      else pontos += 4;

      // Bônus caso o ambiente esteja bem iluminado
      if (ldrValor > 2000) pontos += 5;
    }

    // Verifica se o tempo da rodada acabou
    if (millis() - inicioTempo > tempoLimite) {
      encerrarJogo();
    }
  }
}

