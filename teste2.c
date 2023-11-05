#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    // Criar o soquete do cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Erro ao criar o soquete do cliente");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // Porta do servidor
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Endereço IP do servidor

    // Conectar ao servidor
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Erro ao conectar ao servidor");
        exit(1);
    }

    printf("Conexão estabelecida com o servidor.\n");

    // Enviar dados para o servidor
    char message[] = "Olá, servidor!";
    send(client_socket, message, sizeof(message), 0);

    // Receber resposta do servidor
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        printf("Resposta do servidor: %s\n", buffer);
    }

    // Fechar o soquete do cliente
    close(client_socket);

    return 0;
}