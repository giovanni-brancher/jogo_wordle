#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>

#define TAM_NOME_JOGADOR 30 + 1 // + 1 para o \0 no final

char* obterNomeJogador();

int main(int argc, char const *argv[])
{
    char* nomeJogador = obterNomeJogador();
    printf("Nome armazenado: %s\n", nomeJogador);

    free(nomeJogador);
    return 0;
}

char* obterNomeJogador() {
    char* nome = (char*)malloc(TAM_NOME_JOGADOR * sizeof(char));
    if (nome == NULL) {
        fprintf(stderr, "Erro na alocação de memória (Nome)\n");
        return NULL;
    }

    printf("Nome do Jogador: ");
    fgets(nome, TAM_NOME_JOGADOR, stdin);

    // Remove a quebra de linha ocasionada pelo Enter
    size_t len = strlen(nome);
    if (len > 0 && nome[len - 1] == '\n') {
        nome[len - 1] = '\0';
    }

    return nome;
}

/**
 * Comandos executados:
 * sudo apt-get install libncurses5-dev libncursesw5-dev // instala o curses.h
 * gcc cliente.c -o cliente
*/