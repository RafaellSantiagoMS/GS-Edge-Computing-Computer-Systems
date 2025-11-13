// ======================================================
// BIBLIOTECAS NECESSÁRIAS
// ======================================================
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ======================================================
// CONFIGURAÇÕES DE REDE WI-FI E MQTT
// ======================================================
const char* SSID = "Wokwi-GUEST";
const char* PASSWORD = "";
const char* BROKER_MQTT = "18.228.235.47";
const int BROKER_PORT = 1883;
const char* TOPICO_PUBLISH = "/GS/jogo/attrs";
const char* ID_MQTT = "gs_gamificacao_01";

// ======================================================
// CONFIGURAÇÃO DO DISPLAY OLED SSD1306
// ======================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ======================================================
// DEFINIÇÃO DE PINOS
// ======================================================
#define LED_RED 23
#define LED_GREEN 22
#define LED_BLUE 21
#define PIR_PIN 33
#define BUTTON_PIN 12
#define LDR_PIN 34

// ======================================================
// VARIÁVEIS GLOBAIS DO JOGO
// ======================================================
int pontosRodada = 0;
bool jogoAtivo = false;
unsigned long inicioTempo = 0;
unsigned long tempoLimite = 0;
unsigned long tempoReacao = 0;
bool movimentoDetectado = false;
int ldrBase = 0;

// ======================================================
// CONTROLE DO BOTÃO (DEBOUNCE)
// ======================================================
unsigned long ultimoBotao = 0;
const unsigned long DEBOUNCE_DELAY = 500;

// ======================================================
// CONFIGURAÇÃO DE PONTUAÇÃO
// ======================================================
const int PONTOS_RAPIDO = 12;   // Resposta abaixo de 2 segundos
const int PONTOS_MEDIO = 8;     // Resposta abaixo de 4 segundos
const int PONTOS_LENTO = 4;     // Resposta acima de 4 segundos
const int BONUS_LUZ = 10;       // Bônus se a luminosidade aumentar
const int LIMITE_LUZ = 200;     // Diferença mínima de luz para ativar o bônus

// ======================================================
// LIMITES DE CLASSIFICAÇÃO
// ======================================================
const int LIMITE_BAIXO = 10;
const int LIMITE_MEDIO = 18;

// ======================================================
// OBJETOS DE REDE
// ======================================================
WiFiClient espClient;
PubSubClient MQTT(espClient);

// ======================================================
// FUNÇÃO: CONECTAR AO WI-FI
// ======================================================
void conectarWiFi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP atribuído: ");
  Serial.println(WiFi.localIP());
}

// ======================================================
// FUNÇÃO: CONECTAR AO BROKER MQTT
// ======================================================
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado com sucesso ao broker!");
      MQTT.publish(TOPICO_PUBLISH, "status:on");
    } else {
      Serial.println("Falha ao conectar. Tentando novamente em 2 segundos...");
      delay(2000);
    }
  }
}

// ======================================================
// FUNÇÃO: ENVIAR DADOS AO BROKER MQTT
// ======================================================
void enviarDadosMQTT(int pontos, unsigned long tempoReacao, String status) {
  String payload = "{";
  payload += "\"pontuacao\": " + String(pontos) + ",";
  payload += "\"tempo_reacao\": " + String(tempoReacao) + ",";
  payload += "\"status\": \"" + status + "\"";
  payload += "}";
  MQTT.publish(TOPICO_PUBLISH, payload.c_str());
  Serial.println("Enviado MQTT: " + payload);
}

// ======================================================
// FUNÇÃO: EXIBIR MENSAGEM NO DISPLAY OLED
// ======================================================
void mensagem(String texto) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(texto);
  display.display();
}

// ======================================================
// FUNÇÃO: CONTROLE DE LEDS
// ======================================================
void acenderLED(int cor) {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);

  if (cor == LED_RED || cor == LED_BLUE || cor == LED_GREEN) {
    digitalWrite(cor, HIGH);
  }
}

// ======================================================
// FUNÇÃO: MOSTRAR MENSAGEM DE NÍVEL
// ======================================================
void mostrarConhecimento(int nivel) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 5);

  if (nivel == 1) {
    display.println("Nivel 1 - Conectado");
    display.println("");
    display.println("Use tecnologia com");
    display.println("curiosidade e cuidado.");
    display.println("Aprender e o 1o passo!");
  } 
  else if (nivel == 2) {
    display.println("Nivel 2 - Criativo");
    display.println("");
    display.println("A IA e sua aliada!");
    display.println("Use-a para explorar");
    display.println("ideias e criar solucoes.");
  } 
  else {
    display.println("Nivel 3 - Consciente");
    display.println("");
    display.println("Voce domina a IA!");
    display.println("Use-a com etica e");
    display.println("responsabilidade.");
  }

  display.display();
}

// ======================================================
// FUNÇÃO: INICIAR UMA NOVA RODADA
// ======================================================
void iniciarJogo() {
  jogoAtivo = true;
  movimentoDetectado = false;
  pontosRodada = 0;
  tempoReacao = 0;
  tempoLimite = random(5000, 8000);
  inicioTempo = millis();

  // Mede a luz ambiente inicial (base)
  ldrBase = 0;
  for (int i = 0; i < 10; i++) {
    ldrBase += analogRead(LDR_PIN);
    delay(50);
  }
  ldrBase = ldrBase / 10;

  Serial.println("=== NOVA RODADA INICIADA ===");
  Serial.print("Tempo limite: "); Serial.println(tempoLimite);
  Serial.print("LDR base (media): "); Serial.println(ldrBase);

  mensagem("Rodada iniciada!\nMovimente-se rapido!\nUse uma lanterna!");
  acenderLED(LED_BLUE); 
  enviarDadosMQTT(pontosRodada, 0, "iniciado");
}

// ======================================================
// FUNÇÃO: ENCERRAR A RODADA (CORRIGIDA)
// ======================================================
void encerrarJogo() {
  jogoAtivo = false;
  acenderLED(-1);
  String status;

  if (!movimentoDetectado) {
    pontosRodada = 0;
    tempoReacao = 0;
    Serial.println("Nenhum movimento detectado - 0 pontos");
  }

  Serial.println("===== FIM DA RODADA =====");
  Serial.print("Pontuacao: "); Serial.println(pontosRodada);
  Serial.print("Tempo de reacao (ms): "); Serial.println(tempoReacao);

  // Classificação corrigida
  if (pontosRodada < LIMITE_BAIXO) {
    status = "baixo";
    mensagem("Pontuacao: " + String(pontosRodada) + "\nStatus: Baixo");
    mostrarConhecimento(1);
    acenderLED(LED_RED);
  } 
  else if (pontosRodada < LIMITE_MEDIO) {
    status = "medio";
    mensagem("Pontuacao: " + String(pontosRodada) + "\nStatus: Medio");
    mostrarConhecimento(2);
    acenderLED(LED_BLUE);
  } 
  else {
    status = "alto";
    mensagem("Pontuacao: " + String(pontosRodada) + "\nStatus: Alto");
    mostrarConhecimento(3);
    acenderLED(LED_GREEN);
  }

  Serial.print("STATUS FINAL: "); Serial.println(status);
  Serial.println("===========================");

  enviarDadosMQTT(pontosRodada, tempoReacao, status);

  delay(5000);
  mensagem("Pressione o botao\npara nova rodada!");
}

// ======================================================
// CONFIGURAÇÃO INICIAL (SETUP)
// ======================================================
void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Inicialização do display OLED
  Wire.begin(5, 4);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro ao inicializar o display OLED!");
    for (;;);
  }
  display.clearDisplay();
  display.display();

  // Animação de inicialização dos LEDs
  acenderLED(LED_RED); delay(400);
  acenderLED(LED_BLUE); delay(400);
  acenderLED(LED_GREEN); delay(400);
  acenderLED(-1);

  // Conexão Wi-Fi e MQTT
  conectarWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);

  mensagem("Pressione o botao\npara iniciar o jogo!");
  Serial.println("Sistema pronto - aguardando inicio do jogo");
}

// ======================================================
// LOOP PRINCIPAL
// ======================================================
void loop() {
  if (!MQTT.connected()) reconnectMQTT();
  MQTT.loop();

  // Início de rodada ao pressionar botão
  if (!jogoAtivo && digitalRead(BUTTON_PIN) == LOW && 
      (millis() - ultimoBotao) > DEBOUNCE_DELAY) {
    ultimoBotao = millis();
    iniciarJogo();
  }

  // Lógica principal do jogo
  if (jogoAtivo) {
    // Verifica movimento
    if (digitalRead(PIR_PIN) == HIGH && !movimentoDetectado) {
      movimentoDetectado = true;
      tempoReacao = millis() - inicioTempo;

      // Calcula variação de luz
      int ldrAtual = 0;
      for (int i = 0; i < 5; i++) {
        ldrAtual += analogRead(LDR_PIN);
        delay(10);
      }
      ldrAtual = ldrAtual / 5;

      int diferencaLuz = ldrAtual - ldrBase;

      // Pontos por tempo de reação
      int pontosTempo = 0;
      if (tempoReacao < 2000) {
        pontosTempo = PONTOS_RAPIDO;
        Serial.println("Reacao muito rapida! +12 pontos");
      } else if (tempoReacao < 4000) {
        pontosTempo = PONTOS_MEDIO;
        Serial.println("Reacao media! +8 pontos");
      } else {
        pontosTempo = PONTOS_LENTO;
        Serial.println("Reacao lenta! +4 pontos");
      }

      // Bônus de luz
      int pontosLuz = 0;
      if (diferencaLuz > LIMITE_LUZ) {
        pontosLuz = BONUS_LUZ;
        Serial.println("Bonus de luz ativado! +10 pontos");
      } else {
        Serial.println("Sem bonus de luz - use uma lanterna!");
      }

      // Soma total
      pontosRodada = pontosTempo + pontosLuz;

      // Feedback visual
      if (pontosLuz > 0) {
        mensagem("Bonus luz: SIM\nPontos: " + String(pontosRodada));
      } else {
        mensagem("Bonus luz: NAO\nPontos: " + String(pontosRodada));
      }

      enviarDadosMQTT(pontosRodada, tempoReacao, "reagiu");
      delay(100);
    }

    // Verifica se o tempo acabou
    if (millis() - inicioTempo > tempoLimite) {
      Serial.println("Tempo limite atingido.");
      encerrarJogo();
    }
  }
}
