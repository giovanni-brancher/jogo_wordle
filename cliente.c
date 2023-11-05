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

struct Partida
{
    char nomeJogador[TAM_NOME_JOGADOR + 1];
    char palavraInformada[TAM_PALAVRA + 1];
    float tempo;
};

char *obterNomeJogador();
char *obterPalavraJogador();
void limparBuffers();
char *palavraEntrada;

int main(int argc, char const *argv[])
{
    int client_socket;
    struct sockaddr_in server_address;

    // Criar o soquete do cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Erro ao criar o soquete do cliente");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);                   // Porta do servidor
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Endereço IP do servidor

    // Conectar ao servidor
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Erro ao conectar ao servidor");
        exit(1);
    }

    printf("Conexão estabelecida com o servidor.\n");
    // char *nomeJogador, *palavraInformada; // depois apagar

    struct Partida partida;
    memset(&partida, 0, sizeof(partida));

    while (1)
    {
        strncpy(partida.nomeJogador, obterNomeJogador(), sizeof(partida.nomeJogador) - 1);
        partida.nomeJogador[sizeof(partida.nomeJogador) - 1] = '\0'; // Certifica-se de terminar a string
        strncpy(partida.palavraInformada, "teste", sizeof(partida.palavraInformada) - 1);
        partida.palavraInformada[sizeof(partida.palavraInformada) - 1] = '\0'; // Certifica-se de terminar a string
        partida.tempo = 50;

        printf("Nome obtido: %s\n", partida.nomeJogador);
        printf("Palavra informada: %s\n", partida.palavraInformada);
        printf("Tempo: %f\n", partida.tempo);

        printf("Solicitando servico ao servidor\n");
        int rval = send(client_socket, &partida, sizeof(struct Partida), 0);
    }

    close(client_socket);

    // char *palavraInformada, *nomeJogador;
    // char lstPalavras[2][TAM_PALAVRA + 1];
    // strcpy(lstPalavras[0], "abano");
    // strcpy(lstPalavras[1], "citar");

    // nomeJogador = obterNomeJogador();
    // printf("Nome obtido: %s\n", nomeJogador);

    // for (int tentativa = 0; tentativa < NUM_MAX_TENTATIVAS; tentativa++)
    // {
    //     palavraInformada = obterPalavraJogador();
    //     printf("Enviando para o servidor: %s\n", palavraInformada);
    //     // Envia para o servidor

    //     int posicaoValidada[TAM_PALAVRA];
    //     for (int i = 0; i < TAM_PALAVRA; i++) // Reseta verificações na palavra atual
    //     {
    //         posicaoValidada[i] = 0;
    //     }

    //     printf("Verificando letras posicionadas corretamente\n");
    //     for (int i = 0; i < strlen(palavraInformada); i++)
    //     {
    //         if (lstPalavras[0][i] == palavraInformada[i])
    //         {
    //             printf(">> Letra e posicao certas (%d, %c)\n", i, palavraInformada[i]);
    //             posicaoValidada[i] = 1;
    //         }
    //     }

    //     printf("Verificando letras corretas fora de posicao\n");
    //     for (int i = 0; i < strlen(palavraInformada); i++)
    //     {
    //         if (posicaoValidada[i] != 1) // Indica que pode ser a letra certa, mas a posição errada após o for anterior
    //         {
    //             for (int j = 0; j < strlen(lstPalavras[0]); j++)
    //             {
    //                 if (posicaoValidada[j] != 1 && palavraInformada[i] == lstPalavras[0][j])
    //                 {
    //                     printf(">> Letra certa mas posicao errada (%d, %c)\n", j, palavraInformada[i]);
    //                     posicaoValidada[i] = 2; // 2 é aviso
    //                 }
    //             }
    //         }
    //     }

    //     // somente para verificar
    //     printf("certa = %s, informado = %s\n", lstPalavras[0], palavraInformada);
    //     bool acertouTudo = true;
    //     for (int i = 0; i < TAM_PALAVRA; i++)
    //     {
    //         printf("%d ", posicaoValidada[i]);
    //         acertouTudo = acertouTudo && posicaoValidada[i] == 1; // and's para verificar
    //     }
    //     printf("\n");

    //     if (acertouTudo)
    //     {
    //         printf("Você venceu!\n");
    //         break;
    //     }

    //     printf("Voce possui %d tentativas ainda\n\n", NUM_MAX_TENTATIVAS - tentativa - 1);
    //     limparBuffers();
    // }

    // free(nomeJogador);
    // printf("Fim de jogo!\n");
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
