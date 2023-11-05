/**
 * Comandos executados:
 * sudo apt-get install libncurses5-dev libncursesw5-dev // instala o curses.h
 * gcc servidor.c -o servidor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

char **lerArquivoDeTexto(const char *nomeArquivo, int *numLinhas);

int main(int argc, char const *argv[])
{
    int numLinhas;
    char **linhas = lerArquivoDeTexto("banco_palavras_teste.txt", &numLinhas);

    if (linhas != NULL)
    {
        printf("Conteúdo do arquivo:\n");
        for (int i = 0; i < numLinhas; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                char caractere = linhas[i][j];
                printf("%c", caractere);
            }
            printf("\n");

            // printf("%s %li", linhas[i], strlen(linhas[i]));
            free(linhas[i]); // Libera a memória alocada para cada linha
        }
        free(linhas); // Libera a memória alocada para o array de linhas
        printf("\nNúmero de palavras lidas: %d\n", numLinhas);
    }

    return 0;
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