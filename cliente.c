/**
 * Comandos executados:
 * sudo apt-get install libncurses5-dev libncursesw5-dev // instala o curses.h
 * gcc cliente.c -o cliente
 */

#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "constants.h"

char *obterNomeJogador();
char *obterPalavraJogador();
void limparBuffers();
char *palavraEntrada;

int main(int argc, char const *argv[])
{
    int client_socket;
    struct sockaddr_in server_address;

    // Cria o socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Erro ao criar o soquete do cliente");
        exit(1);
    }

    // Parametros da conexao com o servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(IP_SERVIDOR);

    // Conecta socket ao servidor
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Erro ao conectar ao servidor");
        exit(1);
    }

    printf("Conexão estabelecida com o servidor\n");
    char *nomeJogador = obterNomeJogador();
    struct Partida partida;
    memset(&partida, 0, sizeof(partida));

    // Dados da partida
    char *palavraInformada;
    // char lstPalavras[1][TAM_PALAVRA + 1];
    // strcpy(lstPalavras[0], "abano");

    for (int tentativa = 0; tentativa < NUM_MAX_TENTATIVAS; tentativa++)
    {
        // Palavra enviada para conferencia
        palavraInformada = obterPalavraJogador();
        strncpy(partida.palavraInformada, palavraInformada, sizeof(partida.palavraInformada) - 1);
        partida.palavraInformada[sizeof(partida.palavraInformada) - 1] = '\0'; // Certifica-se de terminar a string

        // Nome do Jogador
        strncpy(partida.nomeJogador, nomeJogador, sizeof(partida.nomeJogador) - 1);
        partida.nomeJogador[sizeof(partida.nomeJogador) - 1] = '\0'; // Certifica-se de terminar a string

        // Tempo atual
        partida.tempo = 50;

        printf("Verificando palavra\n");
        int rval1 = send(client_socket, &partida, sizeof(struct Partida), 0);
        int rval2 = recv(client_socket, &partida, sizeof(struct Partida), 0);

        // Verifica se o jogador acertou todas as letras
        bool acertouTudo = true;

        for (int i = 0; i < TAM_PALAVRA; i++)
        {
            printf("%d ", partida.posicaoValidada[i]);
            acertouTudo = acertouTudo && partida.posicaoValidada[i] == 1; // and's para verificar
        }

        if (acertouTudo) {
            printf("\nVoce venceu!\n\n");
            break;
        }

        printf("\nVoce possui %d tentativas restantes\n\n", NUM_MAX_TENTATIVAS - tentativa - 1);
        limparBuffers();
    }

    close(client_socket);
    free(nomeJogador);
    printf("Fim de jogo!\n");

    return 0;
}

char *obterNomeJogador()
{
    char *nome = (char *)malloc((TAM_NOME_JOGADOR + 1) * sizeof(char));
    if (nome == NULL)
    {
        fprintf(stderr, "Erro na alocação de memória (Nome)\n");
        return NULL;
    }

    printf("Nome do Jogador: ");
    fgets(nome, TAM_NOME_JOGADOR + 1, stdin);

    // Remove a quebra de linha ocasionada pelo Enter
    size_t len = strlen(nome);
    if (len > 0 && nome[len - 1] == '\n')
    {
        nome[len - 1] = '\0';
    }

    return nome;
}

char *obterPalavraJogador()
{
    palavraEntrada = (char *)malloc((TAM_PALAVRA + 1) * sizeof(char));
    if (palavraEntrada == NULL)
        printf("Falha na relocação de memória.\n");

    while (true)
    {
        printf("Digite a palavra: ");
        fgets(palavraEntrada, (TAM_PALAVRA + 1), stdin);
        size_t len = strlen(palavraEntrada);
        if (len > 0 && palavraEntrada[len - 1] == '\n')
        {
            palavraEntrada[len - 1] = '\0'; // Remove a quebra de linha ocasionada pelo Enter
            printf("A palavra informada deve conter %d caracteres\n", TAM_PALAVRA);
            continue;
        }
        break;
    }

    return palavraEntrada;
}

void limparBuffers()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    free(palavraEntrada);
    fflush(stdin);
}
