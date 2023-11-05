/**
 * Comandos executados:
 * sudo apt-get install libncurses5-dev libncursesw5-dev // instala o curses.h
 * gcc servidor.c -o servidor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "constants.h"

struct Partida
{
    char nomeJogador[TAM_NOME_JOGADOR + 1];
    char palavraInformada[TAM_PALAVRA + 1];
    float tempo;
};

char **lerArquivoDeTexto(const char *nomeArquivo, int *numLinhas);
int obterIndicePalavraAleatoria(int qtdPalavras);

int main(int argc, char const *argv[])
{
    printf("Iniciando servidor\n");
    //     printf("Lendo banco de palavras\n");

    // #pragma region Leitura do banco de palavras
    //     int numLinhas;
    //     char **linhas = lerArquivoDeTexto(BANCO_PALAVRAS, &numLinhas);
    //     char lstPalavras[numLinhas][TAM_PALAVRA + 1];
    //     if (linhas != NULL)
    //     {
    //         for (int i = 0; i < numLinhas; i++)
    //         {
    //             strncpy(lstPalavras[i], linhas[i], sizeof(lstPalavras[i]) - 1);
    //             lstPalavras[i][sizeof(lstPalavras[i]) - 1] = '\0';
    //             free(linhas[i]);
    //         }
    //         free(linhas);
    //     }
    // #pragma endregion

    // printf("Número de palavras: %d\n", numLinhas);
    // printf("Banco de palavras lido\n");

    int server_socket, client_address, s0;
    struct sockaddr_in server_address;
    socklen_t client_address_len = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erro ao criar o soquete do servidor");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT); // Porta do servidor
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
    s0 = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
    //printf("Server received connections from %s  \n", inet_ntoa(server_address.sin_addr));
    if (s0 == -1) {
        perror("Erro ao aceitar a conexão do cliente");
        exit(1);
    }

    printf("Cliente conectado.\n");

    struct Partida partida;
    memset(&partida, 0, sizeof(partida));

    while (1)
    {
        int rval = recv(s0, &partida, sizeof(struct Partida), 0);
        printf("Nome do Jogador: %s\n", partida.nomeJogador);
        printf("Palavra do Jogador: %s\n", partida.palavraInformada);
        printf("Tempo do Jogador: %.2f\n", partida.tempo);
    }

    close(s0);
    return 0;

    // int indiceSorteado = obterIndicePalavraAleatoria(numLinhas);
    // printf("indiceSorteado=%d\n", indiceSorteado);
    // printf("%s\n", lstPalavras[indiceSorteado]);
    // return 0;
}

int obterIndicePalavraAleatoria(int qtdPalavras)
{
    srand(time(NULL));
    return rand() % (qtdPalavras);
}

char **lerArquivoDeTexto(const char *nomeArquivo, int *numLinhas)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return NULL;
    }

    char **linhas = NULL;
    char buffer[256]; // Tamanho máximo de uma linha
    int contador = 0;

    while (fgets(buffer, sizeof(buffer), arquivo) != NULL)
    {
        // Aloca memória para a nova linha
        char **temp = (char **)realloc(linhas, (contador + 1) * sizeof(char *));
        if (temp == NULL)
        {
            fprintf(stderr, "Erro na alocação de memória.\n");
            fclose(arquivo);
            for (int i = 0; i < contador; i++)
            {
                free(linhas[i]);
            }
            free(linhas);
            return NULL;
        }
        linhas = temp;

        // Aloca memória para armazenar a linha atual
        linhas[contador] = (char *)malloc(strlen(buffer) + 1);
        if (linhas[contador] == NULL)
        {
            fprintf(stderr, "Erro na alocação de memória.\n");
            fclose(arquivo);
            for (int i = 0; i <= contador; i++)
            {
                free(linhas[i]);
            }
            free(linhas);
            return NULL;
        }

        // Copia a linha do buffer para a matriz
        strcpy(linhas[contador], buffer);
        contador++;
    }

    fclose(arquivo);
    *numLinhas = contador;

    return linhas;
}