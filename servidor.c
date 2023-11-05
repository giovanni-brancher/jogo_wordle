/**
 * Comandos executados:
 * sudo apt-get install libncurses5-dev libncursesw5-dev // instala o curses.h
 * gcc servidor.c -o servidor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "constants.h"

char **lerArquivoDeTexto(const char *nomeArquivo, int *numLinhas);
int obterIndicePalavraAleatoria(int qtdPalavras);

int main(int argc, char const *argv[])
{
    printf("Iniciando servidor\n");
    printf("Lendo banco de palavras\n");

    #pragma region Leitura do banco de palavras
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
    #pragma endregion

    printf("Número de palavras: %d\n", numLinhas);
    printf("Banco de palavras lido\n");








    int indiceSorteado = obterIndicePalavraAleatoria(numLinhas);
    printf("indiceSorteado=%d\n", indiceSorteado);
    printf("%s\n", lstPalavras[indiceSorteado]);
    return 0;
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