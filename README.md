[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/G8V_0Zaq)

# 🔒 IoT Security Lab - Segurança em Comunicação MQTT

**Autores:** Jorge Wilker Mamede de Andrade e Carlos Amaral  
**Curso:** Residência Tecnológica em Sistemas Embarcados  
**Instituição:** EmbarcaTech - HBr  
**Campinas, 02 de junho de 2025**

---

## 🎯 Objetivo

Este projeto implementa um sistema de comunicação IoT seguro utilizando Raspberry Pi Pico W (BitDogLab) com proteção contra ataques de replay e criptografia XOR. O sistema demonstra conceitos fundamentais de segurança em IoT, incluindo confidencialidade de dados, proteção temporal e autenticação via MQTT.

## Descrição
Um servidor mosquitto, instalado em um computador na rede local, recebe um broker com dois tópicos. Um criptografado e outro sem criptografia. A uma das placas BitDogLab registrará a temperatura de uma sala, e enviará esta informação ao broker, diretamente, e criptografada. A outra placa assinará o canal (tópico) e exibirá no display OLED da placa as duas mensagens enviadas após a descriptografia. 

### Tópicos MQTT
| Tópico | Descrição |
|--------|-----------|
| `escola/sala1/temperatura` | Mensagens JSON originais (legíveis) |
| `escola/sala1/temperatura_criptografada` | Timestamp criptografado em formato hexadecimal |

## 🔧 Componentes Usados

### Hardware
- **Raspberry Pi Pico W** (2x) (BitDogLab)
- **Display OLED SSD1306** 128x64 (I2C)
- **Rede WiFi** para conectividade Wifi
- **Computador** atuando como broker MQTT

### Software
- **Pico SDK** (versão 2.1.1)
- **CMake** para compilação
- **Mosquitto MQTT Broker** (servidor local)
- **lwIP** (Lightweight IP stack)
- **Bibliotecas:**
  - `wifi_conn.c/h` - Gerenciamento de conexão WiFi
  - `mqtt_comm.c/h` - Comunicação MQTT
  - `xor_cipher.c/h` - Criptografia XOR
  - `ssd1306_i2c.c/h` - Driver para display OLED SSD1306

## 📡 Primeira BitDogLab - Dispositivo Transmissor (Publisher)

A **primeira BitDogLab** atua como **dispositivo transmissor (publisher)** no sistema de comunicação IoT. Esta implementação utiliza o **Pico SDK** nativo com **CMake** para máxima performance e controle de hardware.

### 📋 Características da Primeira BitDogLab

#### **Funcionalidades Principais:**
- **Publisher MQTT:** Publica mensagens simultaneamente em dois tópicos (original e criptografado)
- **Criptografia XOR:** Aplica criptografia XOR à mensagem JSON completa usando chave simétrica (42)
- **Geração de Timestamp:** Cria timestamp Unix para proteção contra replay attacks
- **Simulação de Sensor:** Gera dados de temperatura simulados (26.5°C) para demonstração
- **Interface Visual Completa:** Display OLED com status detalhado do sistema

#### **Especificações Técnicas:**
- **Plataforma:** Raspberry Pi Pico W
- **Framework:** Pico SDK (nativo)
- **Sistema de Build:** CMake
- **Comunicação:** lwIP stack para WiFi e MQTT
- **Display:** SSD1306 OLED 128x64 via I2C
- **Pinos I2C:** SDA (GPIO 14), SCL (GPIO 15)
- **Endereço I2C:** 0x3C
- **Intervalo de Publicação:** 5 segundos

#### **Interface Visual no OLED:**
```
┌─────────────────────────────┐
│   IOT SECURITY LAB          │
│ WIFI: OK    MQTT: OK        │
│─────────────────────────────│
│ TEMP: 26.5 C                │
│ TS: 1735123456    4A2E      │
│ XOR ATIVO                   │
│                             │
└─────────────────────────────┘
```

#### **Processo de Publicação:**
1. **Geração de Dados:** Cria timestamp Unix atual e formata JSON
2. **Criptografia:** Aplica XOR à mensagem JSON completa
3. **Publicação Dual:** 
   - Tópico original: `{"valor":26.5,"ts":1735123456}`
   - Tópico criptografado: dados binários criptografados
4. **Feedback Visual:** Atualiza display com status e dados em tempo real


## 🔄 Segunda BitDogLab - Dispositivo Receptor (secondary_device)


A pasta `secondary_device` contém o código para a **segunda BitDogLab**, que atua como um **dispositivo receptor (subscriber)** no sistema de comunicação IoT. Esta implementação utiliza **PlatformIO** com o framework **Arduino** para o Raspberry Pi Pico W.

### 📋 Características da Segunda BitDogLab

#### **Funcionalidades Principais:**
- **Subscriber MQTT:** Recebe mensagens de ambos os tópicos (`escola/sala1/temperatura` e `escola/sala1/temperatura_criptografada`)
- **Descriptografia XOR:** Decodifica automaticamente mensagens criptografadas usando chave simétrica (42)
- **Display OLED Comparativo:** Exibe simultaneamente temperaturas originárias de ambos os canais
- **Processamento JSON:** Deserializa mensagens JSON para extrair valores de temperatura
- **Reconexão Automática:** Sistema robusto de reconexão WiFi e MQTT

#### **Especificações Técnicas:**
- **Plataforma:** Raspberry Pi Pico W
- **Framework:** Arduino (Arduino-Pico core)
- **Bibliotecas:**
  - `PubSubClient` - Cliente MQTT
  - `ArduinoJson` - Processamento de dados JSON
  - `OLED.h` - Driver personalizado para display SSD1306
- **Velocidade Serial:** 115200 baud
- **Porta MQTT:** 1883

#### **Interface Visual no OLED:**
```
┌─────────────────────────────┐
│  Connecting (WiFi)          │ ← Estado de conexão
│  Conectado ao WiFi          │ ← Confirmação WiFi
│ MQTT Connecting...          │ ← Estado MQTT
│─────────────────────────────│
│     25.7 oC                 │ ← Temperatura original (linha 3)
│                             │
│     25.7 oC                 │ ← Temperatura descriptografada (linha 6)
│                             │
└─────────────────────────────┘
```

#### **Processo de Descriptografia:**
1. **Recepção:** Captura mensagens do tópico criptografado
2. **Descriptografia XOR:** Aplica algoritmo XOR simétrico (chave 42)
3. **Deserialização JSON:** Converte dados descriptografados em estrutura legível
4. **Exibição:** Mostra temperatura na linha 6 do OLED


## 📈 Resultados Esperados

### Mensagens Originais (Legíveis)
```json
{"valor":26.5,"ts":1735123456}

```## 📂 Arquivos e Estrutura

```
tarefa-iot-security-lab-jorgewilker_-_carlosamaral/
├── src/
│   ├── iot_security_lab.c      # Arquivo principal da aplicação
│   ├── wifi_conn.c             # Implementação da conexão WiFi
│   ├── mqtt_comm.c             # Comunicação MQTT com lwIP
│   ├── xor_cipher.c            # Algoritmo de criptografia XOR
│   └── ssd1306_i2c.c           # Driver para display OLED SSD1306
├── include/
│   ├── wifi_conn.h             # Cabeçalho da conexão WiFi
│   ├── mqtt_comm.h             # Cabeçalho da comunicação MQTT
│   ├── xor_cipher.h            # Cabeçalho da criptografia XOR
│   ├── ssd1306_i2c.h           # Cabeçalho do driver OLED
│   └── lwipopts.h              # Configurações do lwIP
├── secondary_device/           # Segunda BitDogLab (Receptor/Subscriber)
│   ├── src/
│   │   ├── main.cpp            # Código principal do receptor MQTT
│   │   ├── OLED.c              # Driver OLED customizado
│   │   └── ssd1306_i2c.c       # Implementação I2C para OLED
│   ├── include/
│   │   └── OLED.h              # Cabeçalho do driver OLED
│   ├── .vscode/                # Configurações do VSCode
│   ├── platformio.ini          # Configuração do PlatformIO
│   ├── mosquitto_install_info.txt # Guia de instalação do Mosquitto
│   └── .gitignore              # Exclusões Git específicas
├── build/                      # Arquivos de compilação (gerado)
├── CMakeLists.txt              # Configuração de compilação
├── pico_sdk_import.cmake       # Importação do Pico SDK
├── README.md                   # Este arquivo
├── LICENSE                     # Licença GPL-3.0
└── .gitignore                  # Arquivos ignorados pelo Git
```

## 🔒 Mecanismos de Segurança Implementados

1. **Confidencialidade:** Criptografia XOR aplicada ao timestamp
2. **Integridade Temporal:** Timestamp para ordenação de mensagens
3. **Proteção contra Replay:** Validação de timestamp no subscriber
4. **Autenticação:** Credenciais MQTT (usuário/senha)
5. **Interface Visual:** Display OLED com status em tempo real e comparação visual dos dados


## 📜 Licença

Este projeto é licenciado sob GNU General Public License v3.0 (GPL-3.0).
Para mais detalhes, consulte o arquivo [LICENSE](LICENSE) ou visite 
<https://www.gnu.org/licenses/gpl-3.0.html>.

---

**Projeto desenvolvido como parte da Residência Tecnológica em Sistemas Embarcados - EmbarcaTech 2025**
