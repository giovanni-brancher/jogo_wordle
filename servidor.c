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
#include <pthread.h>
#include "constants.h"

pthread_t partidaThread[NUM_MAX_CONEXOES];
pthread_t esperaJogadores;
int server_socket, client_address, sock[NUM_MAX_CONEXOES];
struct sockaddr_in server_address;
struct PartidaInfo partidaInfo[NUM_MAX_CONEXOES];
struct RankingJogo rankingJogo[TAM_RANKING];

char **lerArquivoDeTexto(const char *nomeArquivo, int *numLinhas);
int obterIndicePalavraAleatoria(int qtdPalavras);

void resetRanking()
{
    char nomeJogador[TAM_NOME_JOGADOR + 1];
    char palavraSecreta[TAM_PALAVRA + 1];

    for (int i = 0; i < TAM_RANKING; i++)
    {
        strcpy(rankingJogo[i].nomeJogador, "");
        strcpy(rankingJogo[i].palavraSecreta, "");
        rankingJogo[i].tempo = 0;
        rankingJogo[i].tentativa = 0;
    }
}

void listarRanking()
{
    int numRanking = 1;

    for (int i = TAM_RANKING; i >= 0; i--)
    {
        if (rankingJogo[i].tentativa > 0)
        {
            printf("\nPosicao %d\n", numRanking);
            printf("nomeJogador: %s\n", rankingJogo[i].nomeJogador);
            printf("palavraSecreta: %s\n", rankingJogo[i].palavraSecreta);
            printf("tempo: %f\n", rankingJogo[i].tempo);
            printf("tentativa: %d\n", rankingJogo[i].tentativa);
            numRanking++;
        }
    }
}

// Função para trocar duas structs
void swap(struct RankingJogo *a, struct RankingJogo *b)
{
    struct RankingJogo temp = *a;
    *a = *b;
    *b = temp;
}

// Função para ordenar o array de structs
void bubbleSort(struct RankingJogo array[], int tamanho)
{
    int i, j;
    for (i = 0; i < tamanho - 1; i++)
    {
        for (j = 0; j < tamanho - i - 1; j++)
        {
            if (array[j].tempo < array[j + 1].tempo)
            {
                swap(&array[j], &array[j + 1]);
            }
        }
    }
}

void *jogarPartida(void *arg)
{
    struct PartidaInfo *info = (struct PartidaInfo *)arg;
    int contador = 0;
    printf("(Sala %d) Palavra secreta: %s\n", info->id, info->palavraSecreta);

    struct Partida partida;
    memset(&partida, 0, sizeof(partida));

    while (1)
    {
        int rval = recv(sock[info->id], &partida, sizeof(struct Partida), 0);

        if (strlen(partida.nomeJogador) == 0)
        {
            continue; // aguarda ate vir algo
        }

        printf("\nJogador: %s\n", partida.nomeJogador);
        printf("Palavra informada: %s\n", partida.palavraInformada);
        printf("Tempo de partida: %f ms\n", partida.tempo);
        printf("Tentativa: %d\n", partida.tentativa);

        int posicaoValidada[TAM_PALAVRA];
        for (int i = 0; i < TAM_PALAVRA; i++) // Reseta verificações na palavra atual
        {
            posicaoValidada[i] = 0;
        }

        printf("Verificando letras posicionadas corretamente\n");
        for (int i = 0; i < strlen(partida.palavraInformada); i++)
        {
            if (info->palavraSecreta[i] == partida.palavraInformada[i])
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
            printf("Jogador %s acertou a palavra (%s)!\n", partida.nomeJogador, info->palavraSecreta);

            // Retornando validacao (codigo repetido)
            int *posicaoValidadaPtr = partida.posicaoValidada;
            for (int i = 0; i < TAM_PALAVRA; i++)
            {
                posicaoValidadaPtr[i] = posicaoValidada[i];
            }

            rval = send(sock[info->id], &partida, sizeof(struct Partida), 0);
            // memset(&partida, 0, sizeof(partida)); // limpa memoria alocada
            break;
        }

        printf("Verificando letras corretas fora de posicao\n");
        for (int i = 0; i < strlen(partida.palavraInformada); i++)
        {
            if (posicaoValidada[i] != 1) // Indica que pode ser a letra certa, mas a posição errada após o for anterior
            {
                for (int j = 0; j < strlen(info->palavraSecreta); j++)
                {
                    if (posicaoValidada[j] != 1 && partida.palavraInformada[i] == info->palavraSecreta[j])
                    {
                        printf(">> Letra certa mas posicao errada (%d, %c)\n", j, partida.palavraInformada[i]);
                        posicaoValidada[i] = 2; // 2 é aviso
                    }
                }
            }
        }

        // somente para verificar, excluir depois
        printf("Espectativa = %s, Recebido = %s\n", info->palavraSecreta, partida.palavraInformada);

        // Retornando validacao (codigo repetido)
        int *posicaoValidadaPtr = partida.posicaoValidada;
        for (int i = 0; i < TAM_PALAVRA; i++)
        {
            posicaoValidadaPtr[i] = posicaoValidada[i];
        }

        // printf("Tempo decorrido: %.2f milissegundos\n", partida.tempo);

        rval = send(sock[info->id], &partida, sizeof(struct Partida), 0);
        memset(&partida, 0, sizeof(partida)); // limpa memoria alocada
        // rval = recv(sock[info->id], &partida, sizeof(struct Partida), 0);
        // break;

        if (partida.tentativa == 1)
        {
            printf("Fim do jogo\n");
            break;
        }
    }

    // teste
    // printf("1>>>>>>>>>>>>>>>> %s\n", partida.nomeJogador);

    // for (int i = 0; i < TAM_RANKING; i++)
    // {
    //     if (partida.tempo < rankingJogo[i].tempo && partida.tentativa < rankingJogo[i].tentativa) {
    //         struct RankingJogo temp;
    //         strcpy(temp.nomeJogador, rankingJogo[i].nomeJogador);
    //         strcpy(temp.palavraSecreta, rankingJogo[i].palavraSecreta);
    //         temp.tempo = rankingJogo[0].tempo;
    //         temp.tentativa = rankingJogo[0].tentativa;

    //         // salvo na posicao
    //         strcpy(rankingJogo[0].nomeJogador, partida.nomeJogador);
    //         strcpy(rankingJogo[0].palavraSecreta, info->palavraSecreta);
    //         rankingJogo[0].tempo = partida.tempo;
    //         rankingJogo[0].tentativa = partida.tentativa;

    //         for (int j = 0; i < count; i++)
    //         {
    //             /* code */
    //         }

    //     }
    // }

    strcpy(rankingJogo[5].nomeJogador, partida.nomeJogador);
    strcpy(rankingJogo[5].palavraSecreta, info->palavraSecreta);
    rankingJogo[5].tempo = partida.tempo;
    rankingJogo[5].tentativa = partida.tentativa;

    int tamanho = sizeof(rankingJogo) / sizeof(rankingJogo[0]);
    bubbleSort(rankingJogo, tamanho);
    listarRanking();

    close(sock[info->id]);
}

void *esperarNovosJogadores()
{
    int cont = 0;
    socklen_t client_address_len = sizeof(client_address);
    int idSocket = 0;

    while (1)
    {
        sock[idSocket] = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (sock[idSocket] == -1)
        {
            perror("Erro ao aceitar a conexão do cliente");
            exit(1);
        }
        printf("Cliente %d conectado.\n", idSocket);
        idSocket++;
    }
}

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

    resetRanking();

    printf("Criando salas...\n");
    for (int i = 0; i < NUM_MAX_CONEXOES; i++)
    {
        int indiceSorteado = obterIndicePalavraAleatoria(numLinhas);
        char palavraSecreta[TAM_PALAVRA + 1];
        // printf("indiceSorteado=%d\n", indiceSorteado);

        strcpy(palavraSecreta, lstPalavras[indiceSorteado]);
        // printf("Palavra secreta = %s\n", palavraSecreta);

        partidaInfo[i].id = i;
        strcpy(partidaInfo[i].palavraSecreta, palavraSecreta);

        pthread_create(&partidaThread[i], NULL, (void *)jogarPartida, &partidaInfo[i]);
        // printf("Sala %d criada!\n", i);
    }

    pthread_create(&esperaJogadores, NULL, (void *)esperarNovosJogadores, NULL);

    // printf("Criando salas...\n");
    for (int i = 0; i < NUM_MAX_CONEXOES; i++)
    {
        pthread_join(partidaThread[i], NULL);
        // printf("Sala %d disponível!\n", i);
    }

    pthread_join(esperaJogadores, NULL);
    printf("Aguardando jogadores...\n");

    return 0;

    // Aceitar uma conexão de cliente
    // s0 = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    // if (s0 == -1)
    // {
    //     perror("Erro ao aceitar a conexão do cliente");
    //     exit(1);
    // }
    // printf("Cliente s0 conectado.\n");

    // struct Partida partida;
    // memset(&partida, 0, sizeof(partida));

    // int indiceSorteado = obterIndicePalavraAleatoria(numLinhas);
    // printf("indiceSorteado=%d\n", indiceSorteado);
    // printf("%s\n", lstPalavras[indiceSorteado]);
    // strcpy(lstPalavras[0], lstPalavras[indiceSorteado]);

    // struct timeval tempo_inicial, tempo_final;
    // double tempo_milissegundos;

    // int rval = recv(s0, &partida, sizeof(struct Partida), 0);
    // gettimeofday(&tempo_inicial, NULL);

    // while (1)
    // {
    //     gettimeofday(&tempo_final, NULL);
    //     printf("\nJogador: %s\n", partida.nomeJogador);
    //     printf("Palavra informada: %s\n", partida.palavraInformada);
    //     printf("Tempo de partida: %.2f\n", partida.tempo);

    //     int posicaoValidada[TAM_PALAVRA];
    //     for (int i = 0; i < TAM_PALAVRA; i++) // Reseta verificações na palavra atual
    //     {
    //         posicaoValidada[i] = 0;
    //     }

    //     printf("Verificando letras posicionadas corretamente\n");
    //     for (int i = 0; i < strlen(partida.palavraInformada); i++)
    //     {
    //         if (lstPalavras[0][i] == partida.palavraInformada[i])
    //         {
    //             printf(">> Letra e posicao certas (%d, %c)\n", i, partida.palavraInformada[i]);
    //             posicaoValidada[i] = 1;
    //         }
    //     }

    //     // Verifica se o jogador acertou todas as letras
    //     bool acertouTudo = true;
    //     for (int i = 0; i < TAM_PALAVRA; i++)
    //     {
    //         acertouTudo = acertouTudo && posicaoValidada[i] == 1; // and's para verificar
    //     }

    //     if (acertouTudo)
    //     {
    //         printf("Jogador %s acertou a palavra (%s)!\n", partida.nomeJogador, lstPalavras[0]);

    //         // Retornando validacao (codigo repetido)
    //         int *posicaoValidadaPtr = partida.posicaoValidada;
    //         for (int i = 0; i < TAM_PALAVRA; i++)
    //         {
    //             posicaoValidadaPtr[i] = posicaoValidada[i];
    //         }

    //         partida.tempo = (tempo_final.tv_sec - tempo_inicial.tv_sec) * 1000.0 + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000.0;
    //         printf("Tempo decorrido: %.2f milissegundos\n", partida.tempo);

    //         rval = send(s0, &partida, sizeof(struct Partida), 0);
    //         memset(&partida, 0, sizeof(partida)); // limpa memoria alocada
    //         break;
    //     }

    //     printf("Verificando letras corretas fora de posicao\n");
    //     for (int i = 0; i < strlen(partida.palavraInformada); i++)
    //     {
    //         if (posicaoValidada[i] != 1) // Indica que pode ser a letra certa, mas a posição errada após o for anterior
    //         {
    //             for (int j = 0; j < strlen(lstPalavras[0]); j++)
    //             {
    //                 if (posicaoValidada[j] != 1 && partida.palavraInformada[i] == lstPalavras[0][j])
    //                 {
    //                     printf(">> Letra certa mas posicao errada (%d, %c)\n", j, partida.palavraInformada[i]);
    //                     posicaoValidada[i] = 2; // 2 é aviso
    //                 }
    //             }
    //         }
    //     }

    //     // somente para verificar, excluir depois
    //     printf("Espectativa = %s, Recebido = %s\n", lstPalavras[0], partida.palavraInformada);

    //     // Retornando validacao (codigo repetido)
    //     int *posicaoValidadaPtr = partida.posicaoValidada;
    //     for (int i = 0; i < TAM_PALAVRA; i++)
    //     {
    //         posicaoValidadaPtr[i] = posicaoValidada[i];
    //     }

    //     partida.tempo = (tempo_final.tv_sec - tempo_inicial.tv_sec) * 1000.0 + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000.0;
    //     printf("Tempo decorrido: %.2f milissegundos\n", partida.tempo);

    //     rval = send(s0, &partida, sizeof(struct Partida), 0);
    //     memset(&partida, 0, sizeof(partida)); // limpa memoria alocada
    //     int rval = recv(s0, &partida, sizeof(struct Partida), 0);
    //     // break;
    // }

    // close(s0);
    // return 0;
}

int obterIndicePalavraAleatoria(int qtdPalavras)
{
    static int ultimoIndice = -1; // Inicializado com um valor impossível
    srand(time(NULL));

    int novoIndice;
    do
    {
        novoIndice = rand() % qtdPalavras;
        sleep(1); // para dar tempo de sortear um diferente
    } while (novoIndice == ultimoIndice);

    ultimoIndice = novoIndice; // Armazenar o último índice gerado

    return novoIndice;
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