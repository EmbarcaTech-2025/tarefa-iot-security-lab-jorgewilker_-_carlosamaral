#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP
#include "../include/mqtt_comm.h" // Header file com as declarações locais
// Base: https://github.com/BitDogLab/BitDogLab-C/blob/main/wifi_button_and_led/lwipopts.h
#include "../include/lwipopts.h"  // Configurações customizadas do lwIP
#include "lwip/dns.h"             // Para resolução DNS
#include <string.h>               // Para strncpy e strlen

/* Variável global estática para armazenar a instância do cliente MQTT
 * 'static' limita o escopo deste arquivo */
static mqtt_client_t *client;

/* Variável para controlar o status da conexão MQTT */
static bool mqtt_connected = false;

/* Variáveis para armazenar informações de conexão durante resolução DNS */
static char stored_client_id[64];
static char stored_user[64]; 
static char stored_pass[64];

/* Callback de conexão MQTT - chamado quando o status da conexão muda
 * Parâmetros:
 *   - client: instância do cliente MQTT
 *   - arg: argumento opcional (não usado aqui)
 *   - status: resultado da tentativa de conexão */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT com sucesso!\n");
        mqtt_connected = true;
    } else {
        printf("Falha ao conectar ao broker, código: %d\n", status);
        mqtt_connected = false;
    }
}

/* Callback de resolução DNS - chamado quando o DNS é resolvido */
static void dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr != NULL) {
        printf("DNS resolvido: %s -> %s\n", name, ip4addr_ntoa(ipaddr));
        
        // Cria uma nova instância do cliente MQTT
        client = mqtt_client_new();
        if (client == NULL) {
            printf("Falha ao criar o cliente MQTT\n");
            return;
        }

        // Configura as informações de conexão do cliente
        struct mqtt_connect_client_info_t ci = {
            .client_id = stored_client_id,
            .client_user = (strlen(stored_user) > 0) ? stored_user : NULL,
            .client_pass = (strlen(stored_pass) > 0) ? stored_pass : NULL
        };

        // Inicia a conexão com o broker
        mqtt_client_connect(client, ipaddr, 1883, mqtt_connection_cb, NULL, &ci);
    } else {
        printf("Erro na resolução DNS para %s\n", name);
    }
}

/* Função para configurar e iniciar a conexão MQTT
 * Parâmetros:
 *   - client_id: identificador único para este cliente
 *   - broker_ip: endereço IP do broker como string ou nome de domínio
 *   - user: nome de usuário para autenticação (pode ser NULL)
 *   - pass: senha para autenticação (pode ser NULL) */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass) {
    ip_addr_t broker_addr;  // Estrutura para armazenar o IP do broker
    
    // Armazena as credenciais para uso posterior na resolução DNS
    strncpy(stored_client_id, client_id, sizeof(stored_client_id) - 1);
    stored_client_id[sizeof(stored_client_id) - 1] = '\0';
    
    if (user) {
        strncpy(stored_user, user, sizeof(stored_user) - 1);
        stored_user[sizeof(stored_user) - 1] = '\0';
    } else {
        stored_user[0] = '\0';
    }
    
    if (pass) {
        strncpy(stored_pass, pass, sizeof(stored_pass) - 1);
        stored_pass[sizeof(stored_pass) - 1] = '\0';
    } else {
        stored_pass[0] = '\0';
    }
    
    // Tenta converter diretamente se for um IP numérico
    if (ip4addr_aton(broker_ip, &broker_addr)) {
        printf("Usando IP direto: %s\n", broker_ip);
        
        // Cria uma nova instância do cliente MQTT
        client = mqtt_client_new();
        if (client == NULL) {
            printf("Falha ao criar o cliente MQTT\n");
            return;
        }

        // Configura as informações de conexão do cliente
        struct mqtt_connect_client_info_t ci = {
            .client_id = client_id,
            .client_user = (user && strlen(user) > 0) ? user : NULL,
            .client_pass = (pass && strlen(pass) > 0) ? pass : NULL
        };

        // Inicia a conexão com o broker
        mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
    } else {
        // É um nome de domínio, precisa resolver DNS
        printf("Resolvendo DNS para: %s\n", broker_ip);
        err_t err = dns_gethostbyname(broker_ip, &broker_addr, dns_found_cb, NULL);
        
        if (err == ERR_OK) {
            // DNS já estava em cache, chama callback diretamente
            dns_found_cb(broker_ip, &broker_addr, NULL);
        } else if (err != ERR_INPROGRESS) {
            printf("Erro na resolução DNS: %d\n", err);
        }
        // Se err == ERR_INPROGRESS, o callback será chamado quando o DNS for resolvido
    }
}

/* Callback de confirmação de publicação
 * Chamado quando o broker confirma recebimento da mensagem (para QoS > 0)
 * Parâmetros:
 *   - arg: argumento opcional
 *   - result: código de resultado da operação */
static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("Publicação MQTT enviada com sucesso!\n");
    } else {
        printf("Erro ao publicar via MQTT: %d\n", result);
    }
}

/* Função para verificar se o MQTT está conectado
 * Retorna: true se conectado, false caso contrário */
bool mqtt_is_connected(void) {
    return mqtt_connected;
}

/* Função para publicar dados em um tópico MQTT
 * Parâmetros:
 *   - topic: nome do tópico (ex: "sensor/temperatura")
 *   - data: payload da mensagem (bytes)
 *   - len: tamanho do payload */
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len) {
    if (client == NULL) {
        printf("Cliente MQTT não inicializado\n");
        return;
    }
    
    // Envia a mensagem MQTT
    err_t status = mqtt_publish(
        client,              // Instância do cliente
        topic,               // Tópico de publicação
        data,                // Dados a serem enviados
        len,                 // Tamanho dos dados
        0,                   // QoS 0 (nenhuma confirmação)
        0,                   // Não reter mensagem
        mqtt_pub_request_cb, // Callback de confirmação
        NULL                 // Argumento para o callback
    );

    if (status != ERR_OK) {
        printf("mqtt_publish falhou ao ser enviada: %d\n", status);
    }
}