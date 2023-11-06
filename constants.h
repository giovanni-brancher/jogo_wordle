#define BANCO_PALAVRAS "banco_palavras_teste.txt"
#define NUM_MAX_TENTATIVAS 5
#define TAM_NOME_JOGADOR 30
#define TAM_PALAVRA 5
#define IP_SERVIDOR "127.0.0.1"
#define PORT 9015
#define NUM_MAX_CONEXOES 5

struct Partida
{
    char nomeJogador[TAM_NOME_JOGADOR + 1];
    char palavraInformada[TAM_PALAVRA + 1];
    int posicaoValidada[TAM_PALAVRA];
    float tempo;
};