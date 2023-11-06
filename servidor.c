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
#include <curses.h>
#include "constants.h"

char **lerArquivoDeTexto(const char *nomeArquivo, int *numLinhas);
int obterIndicePalavraAleatoria(int qtdPalavras);

int main(int argc, char const *argv[])
{
    printf("Iniciando servidor\n");
    printf("Lendo banco de palavras\n");

    // Popula o banco de palavras
    int numLinhas;
    char **linhas = lerArquivoDeTexto(BANCO_PALAVRAS, &numLinhas);
    char lstPalavras[numLinhas][TAM_PALAVRA + 1];

    if (linhas != NULL)
    {
        for (int i = 0; i < numLinhas; i++)
        {
            strncpy(lstPalavras[i], linhas[i], sizeof(lstPalavras[i]) - 1);
            lstPalavras[i][sizeof(lstPalavras[i]) - 1] = '\0';
            free(linhas[i]);
        }
        free(linhas);
    }
    
    printf("Número de palavras: %d\n", numLinhas);
    printf("Banco de palavras lido\n");

    printf("Criando socket do servidor\n");
    int server_socket, client_address, s0;
    struct sockaddr_in server_address;
    socklen_t client_address_len = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Erro ao criar o socket do servidor\n");
        exit(1);
    }

    printf("Configurando parametros do socket\n");
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY; // Aceita conexões de qualquer endereço IP

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Erro ao vincular o socket\n");
        exit(1);
    }

    // Coloca o socket em modo de escuta
    if (listen(server_socket, NUM_MAX_CONEXOES) == -1)
    {
        perror("Erro ao colocar o socket em modo de escuta\n");
        exit(1);
    }

    printf("Aguardando jogadores...\n");

    // Aceitar uma conexão de cliente
    s0 = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (s0 == -1)
    {
        perror("Erro ao aceitar a conexão do cliente");
        exit(1);
    }
    printf("Cliente s0 conectado.\n");

    struct Partida partida;
    memset(&partida, 0, sizeof(partida));

    int indiceSorteado = obterIndicePalavraAleatoria(numLinhas);
    printf("indiceSorteado=%d\n", indiceSorteado);
    printf("%s\n", lstPalavras[indiceSorteado]);
    strcpy(lstPalavras[0], lstPalavras[indiceSorteado]);

    while (1)
    {
        int rval = recv(s0, &partida, sizeof(struct Partida), 0);
        printf("\nJogador: %s\n", partida.nomeJogador);
        printf("Palavra informada: %s\n", partida.palavraInformada);
        printf("Tempo de partida: %.2f\n", partida.tempo);

        int posicaoValidada[TAM_PALAVRA];
        for (int i = 0; i < TAM_PALAVRA; i++) // Reseta verificações na palavra atual
        {
            posicaoValidada[i] = 0;
        }

        printf("Verificando letras posicionadas corretamente\n");
        for (int i = 0; i < strlen(partida.palavraInformada); i++)
        {
            if (lstPalavras[0][i] == partida.palavraInformada[i])
            {
                printf(">> Letra e posicao certas (%d, %c)\n", i, partida.palavraInformada[i]);
                posicaoValidada[i] = 1;
            }
        }

        // Verifica se o jogador acertou todas as letras
        bool acertouTudo = true;
        for (int i = 0; i < TAM_PALAVRA; i++)
        {
            acertouTudo = acertouTudo && posicaoValidada[i] == 1; // and's para verificar
        }

        if (acertouTudo)
        {
            printf("Jogador %s acertou a palavra (%s)!\n", partida.nomeJogador, lstPalavras[0]);

            // Retornando validacao (codigo repetido)
            int *posicaoValidadaPtr = partida.posicaoValidada;
            for (int i = 0; i < TAM_PALAVRA; i++)
            {
                posicaoValidadaPtr[i] = posicaoValidada[i];
            }

            rval = send(s0, &partida, sizeof(struct Partida), 0);
            memset(&partida, 0, sizeof(partida)); // limpa memoria alocada
            break;
        }

        printf("Verificando letras corretas fora de posicao\n");
        for (int i = 0; i < strlen(partida.palavraInformada); i++)
        {
            if (posicaoValidada[i] != 1) // Indica que pode ser a letra certa, mas a posição errada após o for anterior
            {
                for (int j = 0; j < strlen(lstPalavras[0]); j++)
                {
                    if (posicaoValidada[j] != 1 && partida.palavraInformada[i] == lstPalavras[0][j])
                    {
                        printf(">> Letra certa mas posicao errada (%d, %c)\n", j, partida.palavraInformada[i]);
                        posicaoValidada[i] = 2; // 2 é aviso
                    }
                }
            }
        }

        // somente para verificar, excluir depois
        printf("certa = %s, informado = %s\n", lstPalavras[0], partida.palavraInformada);

        // Retornando validacao (codigo repetido)
        int *posicaoValidadaPtr = partida.posicaoValidada;
        for (int i = 0; i < TAM_PALAVRA; i++)
        {
            posicaoValidadaPtr[i] = posicaoValidada[i];
        }

        rval = send(s0, &partida, sizeof(struct Partida), 0);
        memset(&partida, 0, sizeof(partida)); // limpa memoria alocada
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