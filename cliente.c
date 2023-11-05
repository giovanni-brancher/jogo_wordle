/**
 * Comandos executados:
 * sudo apt-get install libncurses5-dev libncursesw5-dev // instala o curses.h
 * gcc cliente.c -o cliente
 */

#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"

char *obterNomeJogador();
char *obterPalavraJogador();
void limparBuffers();
char *palavraEntrada;

int main(int argc, char const *argv[])
{
    char *palavraInformada, *nomeJogador;
    char lstPalavras[2][TAM_PALAVRA + 1];
    strcpy(lstPalavras[0], "abano");
    strcpy(lstPalavras[1], "citar");

    nomeJogador = obterNomeJogador();
    printf("Nome obtido: %s\n", nomeJogador);

    for (int tentativa = 0; tentativa < NUM_MAX_TENTATIVAS; tentativa++)
    {
        palavraInformada = obterPalavraJogador();
        printf("Enviando para o servidor: %s\n", palavraInformada);
        // Envia para o servidor

        int posicaoValidada[TAM_PALAVRA];
        for (int i = 0; i < TAM_PALAVRA; i++) // Reseta verificações na palavra atual
        {
            posicaoValidada[i] = 0;
        }

        printf("Verificando letras posicionadas corretamente\n");
        for (int i = 0; i < strlen(palavraInformada); i++)
        {
            if (lstPalavras[0][i] == palavraInformada[i])
            {
                printf(">> Letra e posicao certas (%d, %c)\n", i, palavraInformada[i]);
                posicaoValidada[i] = 1;
            }
        }

        printf("Verificando letras corretas fora de posicao\n");
        for (int i = 0; i < strlen(palavraInformada); i++)
        {
            if (posicaoValidada[i] != 1) // Indica que pode ser a letra certa, mas a posição errada após o for anterior
            {
                for (int j = 0; j < strlen(lstPalavras[0]); j++)
                {
                    if (posicaoValidada[j] != 1 && palavraInformada[i] == lstPalavras[0][j])
                    {
                        printf(">> Letra certa mas posicao errada (%d, %c)\n", j, palavraInformada[i]);
                        posicaoValidada[i] = 2; // 2 é aviso
                    }
                }
            }
        }

        // somente para verificar
        printf("certa = %s, informado = %s\n", lstPalavras[0], palavraInformada);
        bool acertouTudo = true;
        for (int i = 0; i < TAM_PALAVRA; i++)
        {
            printf("%d ", posicaoValidada[i]);
            acertouTudo = acertouTudo && posicaoValidada[i] == 1; // and's para verificar
        }
        printf("\n");

        if (acertouTudo)
        {
            printf("Você venceu!\n");
            break;
        }

        printf("Voce possui %d tentativas ainda\n\n", NUM_MAX_TENTATIVAS - tentativa - 1);
        limparBuffers();
    }

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
    while ((c = getchar()) != '\n' && c != EOF);
    free(palavraEntrada);
    fflush(stdin);
}
