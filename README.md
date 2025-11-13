# GS-Edge-Computing-Computer-Systems
## 🧩 Gamificação e Educação Tecnológica com IoT

## 👥 Integrantes
- **Rafaell Santiago Martins da Silva** – RM: 563486  
- **Enzo Augusto Lima da Silva** – RM: 562249
- **Gustavo Neres dos Santos** – RM: 561785  

---

## 🎯 Objetivo

Este projeto tem como objetivo combater a deficiência do ensino sobre tecnologia e inteligência artificial, por meio de uma solução gamificada com IoT, que estimula o aprendizado de competências digitais de forma interativa e prática.

A proposta utiliza o **ESP32** conectado ao **Node-RED (AWS EC2)** via **MQTT**, permitindo que estudantes participem de desafios físicos com feedback em tempo real (via LEDs e display OLED), enquanto desbloqueiam conteúdos educativos conforme sua pontuação.  

O projeto visa integrar educação, tecnologia e gamificação, preparando os alunos para o que podera encontrar no futuro do trabalho.

---

## ⚙️ Arquitetura da Solução

### **Componentes Principais**
- **ESP32 (simulado no Wokwi)** → coleta e envia dados dos sensores.  
- **Sensores simulados:**  
  - **PIR** → Detecta movimento.  
  - **LDR** → Mede luminosidade e influencia na pontuação.  
  - **Botão** → Inicia o jogo.  
- **Atuadores:**  
  - **LEDs RGB** → Fornecem feedback visual (vermelho, azul, verde).  
  - **Display OLED** → Mostra mensagens e frases educativas.  
- **Broker MQTT (Mosquitto)** → Responsável pela troca de mensagens entre o ESP32 e o Node-RED.  
- **Node-RED Dashboard** → Exibe dados em tempo real e gerencia feedbacks.  
- **AWS EC2 (Ubuntu Server)** → Hospeda o ambiente Node-RED e o broker MQTT.

---

## ☁️ Criação da Instância EC2 e Instalação do Ambiente

### 1. Criar a Instância EC2
1. Acesse [AWS EC2](https://aws.amazon.com/ec2/) → **Launch Instance**  
2. Selecione **Ubuntu Server 24.04 LTS**  
3. Tipo de instância: `t2.micro` (Free Tier)  
4. Crie ou selecione um **Par de Chaves (.pem)** e baixe-o  
5. Configure o grupo de segurança com as portas abertas:  
   - 22 → SSH  
   - 1883 → MQTT Broker  
   - 1880 → Node-RED  
   - 1026 → Orion  
   - 4041 → IoT Agent MQTT  
   - 8666 → STH-Comet  
   - 27017 → MongoDB  

---

### 2. Acessar a Instância via SSH

Após criar e iniciar a instância, conecte-se a partir de qualquer computador que possua o par de chaves:

```bash
1. Primeiro ir até o sua maquina virtual no teminal de comando

- ssh -i nome-da-chave.pem ubuntu@<IP-PUBLICO-DA-EC2>

2. Instalar FIWARE
  - git clone https://github.com/fabiocabrini/fiware
  - sudo apt update
  - sudo apt install docker.io 
  - sudo apt install docker-compose
  - ls fiware
  - cd fiware
  - sudo docker-compose up -d
```

### 3. Instalar o Node-RED
```bash
1. Após ir até o sua maquina virtual (ssh -i nome-da-chave.pem ubuntu@<IP-PUBLICO-DA-EC2>)

Execute esses comandos : 
    - sudo npm install -g --unsafe-perm node-red
    - node-red

Depois é so acessar pelo navegadoer:

    - http://<IP_PUBLICO>:1880

```

## 🧠 Detalhes da Implementação

O **ESP32** coleta dados dos sensores (**PIR** e **LDR**) e envia via **MQTT** para o **Broker Mosquitto**.  
O **Node-RED** consome esses dados e exibe no **Dashboard**, com indicadores de pontuação e tempo de reação.  
O **Display OLED** fornece feedback textual e educativo, conforme o desempenho.  
O **FIWARE** (opcional) pode ser utilizado para armazenar os dados históricos e análises avançadas.

---

## 📡 Comunicação MQTT

| Propriedade | Valor |
|--------------|--------|
| **Broker** | `18.228.235.47` |
| **Porta** | `1883` |
| **Tópico** | `/GS/jogo/attrs` |

**Exemplo de mensagem publicada pelo serial monitor do Wokwi:**

```json
{
  "pontuacao": 10,
  "tempo_reacao": 3800,
  "status": "medio"
}
```
---

## 🔗 Simulação ESP32 (Wokwi)

[Link do Projeto Wokwi](https://wokwi.com/projects/447347122209616897)

## 📸 Prints

| **Descrição** | **Imagem** |
|---------------|-----------:|
| Conexão do **ESP32** com o **Broker MQTT (AWS)** | ![ESP32 conectado ao broker](./assets/images/Captura%20de%20tela%202025-11-12%20104635.png) |
| Recebimento dos dados no **Node-RED Dashboard** | ![Dados recebidos Node-RED](./assets/images/Captura%20de%20tela%202025-11-12%20113013.png) |
| Feedback visual no **Display OLED** e **LEDs** | ![Feedback OLED e LEDs](./assets/images/Captura%20de%20tela%202025-11-12%20113422.png) |
| **Dashboard** mostrando pontuação e tempo de reação | ![Dashboard completo](./assets/images/Captura%20de%20tela%202025-11-12%20113527.png) |

---

## 📽️ Vídeo Explicativo (para avaliação)

[Link do Vídeo](https://youtu.be/LDEHrYvmR9Q)

---

## 🏁 Conclusão

Este projeto demonstrou como a IoT e a gamificação podem ser aplicadas na educação tecnológica, promovendo aprendizado prático, engajamento e estimulando a curiosidade científica.

Com o uso de sensores e feedback em tempo real, os estudantes desenvolvem habilidades digitais e aprendem conceitos de Inteligência Artificial de maneira divertida e alinhada ao futuro do trabalho.
