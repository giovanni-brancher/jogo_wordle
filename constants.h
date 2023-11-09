#define BANCO_PALAVRAS "banco_palavras.txt"
#define NUM_MAX_TENTATIVAS 5
#define TAM_NOME_JOGADOR 30
#define TAM_PALAVRA 5
#define IP_SERVIDOR "127.0.0.1"
#define PORT 9001
#define NUM_MAX_CONEXOES 3

struct PartidaInfo
{
    int id;
    char palavraSecreta[TAM_PALAVRA + 1];
};

struct Partida
{
    char nomeJogador[TAM_NOME_JOGADOR + 1];
    char palavraInformada[TAM_PALAVRA + 1];
    int posicaoValidada[TAM_PALAVRA];
    float tempo;
};