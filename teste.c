#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Criar o soquete do servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erro ao criar o soquete do servidor");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // Porta do servidor
    server_address.sin_addr.s_addr = INADDR_ANY; // Aceita conexões de qualquer endereço IP

    // Vincular o soquete a um endereço e porta
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Erro ao vincular o soquete");
        exit(1);
    }

    // Colocar o soquete em modo de escuta
    if (listen(server_socket, 5) == -1) {
        perror("Erro ao colocar o soquete em modo de escuta");
        exit(1);
    }

    printf("Aguardando conexões de clientes...\n");

    // Aceitar uma conexão de cliente
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
    if (client_socket == -1) {
        perror("Erro ao aceitar a conexão do cliente");
        exit(1);
    }

    printf("Cliente conectado.\n");

    // Receber dados do cliente
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        printf("Dados recebidos do cliente: %s\n", buffer);
    }

    // Enviar resposta ao cliente
    char response[] = "Olá, cliente!";
    send(client_socket, response, sizeof(response), 0);

    // Fechar o soquete do cliente e do servidor
    close(client_socket);
    close(server_socket);

    return 0;
}
