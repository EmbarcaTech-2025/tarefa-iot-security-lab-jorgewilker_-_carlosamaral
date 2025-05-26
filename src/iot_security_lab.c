// Bibliotecas necessárias
#include <string.h>                 // Para funções de string como strlen()
#include "pico/stdlib.h"            // Biblioteca padrão do Pico (GPIO, tempo, etc.)
#include "pico/cyw43_arch.h"        // Driver WiFi para Pico W
#include "../include/wifi_conn.h"   // Funções personalizadas de conexão WiFi
#include "../include/mqtt_comm.h"   // Funções personalizadas para MQTT
#include "../include/xor_cipher.h"  // Funções de cifra XOR
#include <time.h>                   // Para usar a função time() para timestamps
#include <stdlib.h>                 // Para usar atof()

int main() {
    // Inicializa todas as interfaces de I/O padrão (USB serial, etc.)
    stdio_init_all();
    sleep_ms(10000);  // Delay de 10 segundos antes de conectar
    
    // Conecta à rede WiFi
    // Parâmetros: Nome da rede (SSID) e senha
    connect_to_wifi("QUALICOR", "Jor405169");

    // Configura o cliente MQTT
    // Parâmetros: ID do cliente, IP do broker, usuário, senha
    mqtt_setup("bitdog1", "192.168.1.130", "aluno", "senha123");

    // Mensagem original a ser enviada (agora apenas como base para o valor)
    const char *mensagem = "26.5";
    // As variáveis criptografada e a chamada inicial xor_encrypt não são mais usadas
    // para criptografar apenas a mensagem original, pois agora criptografamos
    // a string JSON completa.

    // Buffer para a mensagem JSON formatada (temporariamente antes do XOR)
    char json_buffer[64]; // Tamanho suficiente para "{"valor":XX.X,"ts":XXXXXXXXXX}"
    // Buffer para armazenar a mensagem JSON criptografada com XOR
    uint8_t xor_encrypted_buffer[64]; // Mesmo tamanho do buffer JSON

    // Loop principal do programa
    while (true) {
        // Obtém o timestamp atual
        unsigned long current_timestamp = time(NULL);
        
        // Formata a mensagem como JSON com valor e timestamp no json_buffer
        sprintf(json_buffer, "{\"valor\":%.1f,\"ts\":%lu}", atof(mensagem), current_timestamp);

        // Aplica a criptografia XOR na string JSON formatada
        xor_encrypt((uint8_t *)json_buffer, xor_encrypted_buffer, strlen(json_buffer), 42); // Usando chave 42

        // === FINS DIDÁTICOS: Publica AMBAS as versões ===
        
        // 1. Publica a mensagem JSON ORIGINAL (descriptografada/legível) 
        mqtt_comm_publish("escola/sala1/temperatura", (uint8_t *)json_buffer, strlen(json_buffer));
        
        // 2. Publica a mensagem JSON CRIPTOGRAFADA com XOR
        mqtt_comm_publish("escola/sala1/temperatura_criptografada", xor_encrypted_buffer, strlen(json_buffer));

        // === Linhas antigas comentadas para referência ===
        // Publica a mensagem original (não criptografada) - COMENTADA
        // mqtt_comm_publish("escola/sala1/temperatura", (uint8_t *)mensagem, strlen(mensagem));
        
        // Alternativa: Publica a mensagem criptografada (XOR) da string original - COMENTADA
        // mqtt_comm_publish("escola/sala1/temperatura", criptografada, strlen(mensagem));
        
        // Aguarda 5 segundos antes da próxima publicação
        sleep_ms(5000);
    }
    return 0;
}

/* 
 * Comandos de terminal para testar o MQTT:
 * 
 * Inicia o broker MQTT com logs detalhados:
 * mosquitto -c mosquitto.conf -v
 * 
 * Assina o tópico de temperatura (recebe mensagens):
 * mosquitto_sub -h localhost -p 1883 -t "escola/sala1/temperatura" -u "aluno" -P "senha123"
 * 
 * Publica mensagem de teste no tópico de temperatura:
 * mosquitto_pub -h localhost -p 1883 -t "escola/sala1/temperatura" -u "aluno" -P "senha123" -m "26.6"
 */