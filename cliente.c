#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"

char *obterNomeJogador();
char *obterPalavraJogador();

int main(int argc, char const *argv[])
{
    // char* nomeJogador = obterNomeJogador();
    // printf("Nome armazenado: %s\n", nomeJogador);

    // Cliente
    char *palavra = obterPalavraJogador();
    printf("%s\n", palavra);
    // Envia para o servidor

    // Servidor recebe
    char lstPalavras[2][5 + 1];
    strcpy(lstPalavras[0], "abano");
    strcpy(lstPalavras[1], "citar");
    // printf("%s\n", lstPalavras[0]); // Servidor tem definido previamente
    // printf("%s\n", lstPalavras[1]); // Servidor tem definido previamente

    // Verifica quais letras estao dentro

    int posicaoValidada[5] = {0, 0, 0, 0, 0};

    // Procura posicao e letra certa
    for (int i = 0; i < strlen(palavra); i++)
    {
        if (lstPalavras[0][i] == palavra[i])
        {
            printf("Letra e posicao certas (%d, %c)\n", i, palavra[i]);
            posicaoValidada[i] = 1;
        }
    }

    // Procura letra certa e posicao errada nas não validadas ainda
    printf("Procurando por letras em posicoes erradas\n");
    for (int i = 0; i < strlen(palavra); i++)
    {
        if (posicaoValidada[i] == 0) // Indica que pode ser a letra certa, mas a posição errada após o for anterior
        {
            printf("Possivel candidato (%d, %c)\n", i, palavra[i]);
            for (int j = 0; j < strlen(lstPalavras[0]); j++)
            {
                printf("Comparando (%c) == (%c)\n", palavra[i], lstPalavras[0][j]);
                if (posicaoValidada[j] != 1 && palavra[i] == lstPalavras[0][j])
                {
                    printf("Letra certa, posicao errada (j=%d, %c, %c)\n", j, lstPalavras[0][j], palavra[i]);
                    posicaoValidada[i] = 2; // 2 é aviso
                }
            }
        }
    }

    // somente para verificar
    printf("certa = %s, informado = %s\n", lstPalavras[0], palavra);
    for (int i = 0; i < 5; i++)
    {
        printf("%d ", posicaoValidada[i]);
    }
    printf("\n");    

    // for (int i = 0; i < strlen(palavra); i++)
    // {

    //     if (lstPalavras[0][i] == palavra[i])
    //     {
    //         printf("Letra e posicao certas (%c)\n", palavra[i]);
    //         continue; // pula pra prox letra
    //     }

    //     bool existeLetraPalavra = false;
    //     // printf("i=%d\n", i);

    //     for (int j = i; j < strlen(lstPalavras[0]); j++)
    //     {
    //         printf("j=%d\n", j);
    //         if (lstPalavras[0][j] == palavra[i])
    //         {
    //             printf("Letra certa, posicao errada (j=%d, %c, %c)\n", j, lstPalavras[0][j], palavra[i]);
    //             existeLetraPalavra = true;
    //             break; // encontrou a ocorrencia e sai
    //         }
    //     }

    //     if (!existeLetraPalavra)
    //     {
    //         printf("Nao existe essa letra na palavra secreta (%c)\n", palavra[i]);
    //     }

    //     // printf("lstPalavras[0][%d] = %c\n", i, palavra[i]);
    //     // if (palavra[i] == 't') {
    //     //     printf("Eh a letra t\n");
    //     // }
    // }

    // free(nomeJogador);
    free(palavra);
    return 0;

    // teste = abnno
}

char *obterNomeJogador()
{
    char *nome = (char *)malloc(TAM_NOME_JOGADOR * sizeof(char));
    if (nome == NULL)
    {
        fprintf(stderr, "Erro na alocação de memória (Nome)\n");
        return NULL;
    }

    printf("Nome do Jogador: ");
    fgets(nome, TAM_NOME_JOGADOR, stdin);

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
    char *palavra = (char *)malloc(TAM_PALAVRA * sizeof(char));
    printf("Digite a palavra: ");
    fgets(palavra, TAM_PALAVRA, stdin);

    // Remove a quebra de linha ocasionada pelo Enter
    size_t len = strlen(palavra);
    if (len > 0 && palavra[len - 1] == '\n')
    {
        palavra[len - 1] = '\0';
    }

    return palavra;
}

/**
 * Comandos executados:
 * sudo apt-get install libncurses5-dev libncursesw5-dev // instala o curses.h
 * gcc cliente.c -o cliente
 */