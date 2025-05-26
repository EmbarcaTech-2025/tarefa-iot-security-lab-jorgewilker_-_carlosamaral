# 📋 Relatório de Implementações - IoT Security Lab

## 🎯 Objetivo do Projeto
Implementar um sistema de comunicação IoT seguro utilizando Raspberry Pi Pico W (BitDogLab) com proteção contra ataques de replay e criptografia XOR.

---

## 🔧 Implementações Realizadas (Ordem Cronológica)

### 1. **Configuração de Rede Personalizada**
**O que foi modificado:**
- Atualização das credenciais WiFi para rede local
- Configuração do broker MQTT com IP específico (192.168.1.130)
- Manutenção das credenciais de autenticação MQTT

**Código modificado:**
```c
connect_to_wifi("QUALICOR", "Jor405169");
mqtt_setup("bitdog1", "192.168.1.130", "aluno", "senha123");
```

**Benefício:** Estabelecimento da comunicação com o broker MQTT local.

---

### 2. **Criptografia XOR Aplicada ao JSON**
**O que foi implementado:**
- Criptografia XOR aplicada à mensagem JSON completa (incluindo timestamp)
- Uso de chave fixa (42) para demonstração
- Buffers separados para dados originais e criptografados

**Código adicionado:**
```c
// Buffer para a mensagem JSON formatada (antes do XOR)
char json_buffer[64];
// Buffer para armazenar a mensagem JSON criptografada com XOR
uint8_t xor_encrypted_buffer[64];

// Aplica a criptografia XOR na string JSON formatada
xor_encrypt((uint8_t *)json_buffer, xor_encrypted_buffer, strlen(json_buffer), 42);
```

**Benefício:** Proteção de confidencialidade dos dados transmitidos.

---

### 3. **Proteção contra Replay Attack (Etapa 6)**
**O que foi implementado:**
- Adição de timestamp nas mensagens MQTT
- Formatação de dados em JSON com valor e timestamp
- Estrutura de dados: `{"valor":26.5,"ts":1678886400}`

**Código adicionado:**
```c
#include <time.h>                   // Para usar a função time() para timestamps
#include <stdlib.h>                 // Para usar atof()

// Obtém o timestamp atual
unsigned long current_timestamp = time(NULL);

// Formata a mensagem como JSON com valor e timestamp
sprintf(json_buffer, "{\"valor\":%.1f,\"ts\":%lu}", atof(mensagem), current_timestamp);
```

**Benefício:** Permite identificar e descartar mensagens antigas ou repetidas.

---

### 4. **Publicação Dual para Fins Didáticos**
**O que foi implementado:**
- Publicação simultânea de dados originais e criptografados
- Uso de tópicos MQTT distintos para comparação
- Manutenção de compatibilidade com configurações existentes

**Código adicionado:**
```c
// 1. Publica a mensagem JSON ORIGINAL (legível) 
mqtt_comm_publish("escola/sala1/temperatura", (uint8_t *)json_buffer, strlen(json_buffer));

// 2. Publica a mensagem JSON CRIPTOGRAFADA com XOR
mqtt_comm_publish("escola/sala1/temperatura_criptografada", xor_encrypted_buffer, strlen(json_buffer));
```

**Benefício:** Permite visualização comparativa entre dados originais e criptografados.

---

## 🆚 Diferenças em Relação à Tarefa Inicial

### **Antes (Código Original):**
- Publicação de mensagem simples: `"26.5"`
- Sem proteção contra replay
- Criptografia XOR opcional e comentada
- Dados sem estrutura temporal

### **Depois (Implementação Atual):**
- Publicação de JSON estruturado: `{"valor":26.5,"ts":1678886400}`
- Proteção contra replay com timestamp
- Criptografia XOR ativa aplicada ao JSON completo
- Publicação dual (original + criptografado) para fins didáticos
- Estrutura de dados temporal para validação

---

## 📊 Estrutura de Dados Implementada

### **Mensagem Original (Legível):**
```json
{"valor":26.5,"ts":1678886400}
```

### **Mensagem Criptografada:**
```
Bytes não legíveis resultantes da aplicação XOR com chave 42
```

---

## 🔒 Mecanismos de Segurança Implementados

1. **Confidencialidade:** Criptografia XOR dos dados
2. **Integridade Temporal:** Timestamp para ordenação de mensagens
3. **Proteção contra Replay:** Validação de timestamp no subscriber
4. **Autenticação:** Credenciais MQTT (usuário/senha)

---

## 🎓 Valor Didático

A implementação permite:
- **Visualização clara** da diferença entre dados originais e criptografados
- **Compreensão prática** de proteção contra replay attacks
- **Demonstração real** de comunicação IoT segura
- **Comparação lado a lado** de dados protegidos e não protegidos

---

## 📝 Conclusão

O projeto evoluiu de uma simples transmissão de dados para um sistema de comunicação IoT com múltiplas camadas de segurança, mantendo a simplicidade didática e permitindo a visualização clara dos conceitos de segurança implementados.

**Data do Relatório:** 25/05/2025  
**Autor:** Jorge Wilker  
**Projeto:** EmbarcaTech 2025 - IoT Security Lab 