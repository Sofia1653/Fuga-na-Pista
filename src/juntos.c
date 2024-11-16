#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UP 65 //ASCII para cima
#define DOWN 66 //ASCII para baixo
#define PISTAS 6
#define DISTANCIA_PISTA 165
#define MAX_JOGADORES 10 //Quantidade de jogadores no placar final
#define TAMANHO_NOME 100 //Tamanho do nome dos jogadores pro Placar
#define OBSTACULOS {"🛞", "👮", "🛒", "🗑️"}
#define CARRO "🚗"

struct Player {
    char nome[TAMANHO_NOME];
    int tempo;
};

int score = 0;
int probabilidadeObstaculo = 4;
int dificuldade;
int playerLugar = 2;
char *jogadorCarro = "🚓";
char **pista;
char obstaculo[PISTAS][DISTANCIA_PISTA];

void Menu(){
    int choise;

    screenClear();

    printf("1. Iniciar");
    printf("2. Trocar Personagem");
    printf("3. Placar");
    printf("4. Instruções");
    printf("5. Sair");
    scanf("%d", &choise);

    if (choise == 1){
        Jogo();
    }
    else if (choise == 2){
        
    }

    screenUpdate();
}

void iniciarJogo() {
    screenClear();
    // Inicializa as pistas
    strcpy(pista[0], "🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰");
    strcpy(pista[1], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[2], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[3], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[4], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[5], "🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰");
    // Inicializa obstáculos
    memset(obstaculo, ' ', sizeof(obstaculo));  // Limpa a matriz de obstáculos
}

void trocarPersonagem() {
    screenClear();
    screenGotoxy(10, 5);
    printf("Escolha um personagem:");
    screenGotoxy(10, 7);
    printf("1) 🐎");
    screenGotoxy(10, 9);
    printf("2) ⏩");
    screenGotoxy(10, 11);
    printf("3) 🚓");
    screenUpdate();

    char key;
    do {
        while (!keyhit()) {
            usleep(100000); // Aguarda até uma tecla ser pressionada
        }
        key = readch();
        switch (key) {
            case '1':
                jogadorCarro = "🐎";
                break;
            case '2':
                jogadorCarro = "⏩";
                break;
            case '3':
                jogadorCarro = "🚓";
                break;
        }
    } while (key != '1' && key != '2' && key != '3');
}

void atualizarObstaculos() {
    int posicaoInicial = 39; // Posição dentro da matriz de obstáculos que reflete screenGotoxy(50, ...)

    // Mover obstáculos para a esquerda
    for (int i = 0; i < PISTAS - 1; i++) {
        for (int j = 1; j < DISTANCIA_PISTA; j++) {
            obstaculo[i][j-1] = obstaculo[i][j];
            obstaculo[i][j] = ' ';
        }
    }

    // Gerar novos obstáculos aleatoriamente na última coluna
    char obstaculos[] = {OBSTACULOS};
    for (int i = 0; i < PISTAS - 1; i++) {
        if (rand() % 150 < probabilidadeObstaculo) {
            int randObstaculo = rand() % 4; // Gera um número entre 0 e 3 para escolher o obstáculo
            obstaculo[i][posicaoInicial] = obstaculos[randObstaculo];
        }
    }
}

void displayJogo(int playerLugar) {
    int espacamento = 2; // Espaçamento entre as linhas

    // Desenha as pistas
    for (int i = 0; i < PISTAS; i++) {
        screenGotoxy(12, 3 + i * espacamento);
        printf("%s", pista[i]);
    }

    // Atualiza os obstáculos
    for (int i = 0; i < PISTAS; i++) {
        for (int j = 0; j < DISTANCIA_PISTA; j++) {
            screenGotoxy(13 + j, 4 + i * espacamento); // Posiciona os obstáculos na tela
            printf("%c", obstaculo[i][j]);  // Desenha o obstáculo
        }
    }

    // Desenha o jogador na nova posição
    screenGotoxy(13, 4 + playerLugar * espacamento);
    printf("%s", CARRO);

    // Atualiza a tela
    screenUpdate();

    // Exibe a pontuação
    screenGotoxy(12, 3 + PISTAS * espacamento + 2);  // Posição abaixo das pistas
    printf("Score: %d", score);  // Exibe a pontuação
}

void lerPlacar(struct Jogador placar[]) {
    FILE *arquivo = fopen("placar.txt", "r");
    if (arquivo == NULL) {
        for (int i = 0; i < MAX_JOGADORES; i++) {
            strcpy(placar[i].nome, "-");
            placar[i].tempo = 0;
        }
    } else {
        for (int i = 0; i < MAX_JOGADORES; i++) {
            if (fscanf(arquivo, "%*d - %99s %d", placar[i].nome, &placar[i].tempo) !=2 );
                strcpy(placar[i].nome, "-");
                placar[i].tempo = 0;
        }
        fclose(arquivo);
    }
}

void salvarPlacar(struct Jogador placar[]) {
    FILE *arquivo = fopen("placar.txt", "w");
    for (int i = 0; i < MAX_JOGADORES; i++) {
        fprintf(arquivo, "%d - %s %d\n", i + 1, placar[i].nome, placar[i].tempo);
    }
    fclose(arquivo);
}

void inserirNoPlacar(struct Jogador placar[], char nome[], int tempo) {
    int posicao = MAX_JOGADORES;
    for (int i = MAX_JOGADORES - 1; i >= 0; i--) {
        if (tempo > placar[i].tempo) {
            posicao = i;
        } else {
            break;
        }
    }

    if (posicao < MAX_JOGADORES) {
        for (int i = MAX_JOGADORES - 1; i > posicao; i--) {
            strcpy(placar[i].nome, placar[i - 1].nome);
            placar[i].tempo = placar[i - 1].tempo;
        }
        strcpy(placar[posicao].nome, nome);
        placar[posicao].tempo = tempo;
    }
}

void exibirPlacar() {
    struct Jogador placar[MAX_JOGADORES];
    lerPlacar(placar); // Carrega o placar do arquivo

    screenClear(); // Limpa a tela antes de exibir o placar
    screenGotoxy(10, 3);
    printf("Placar Atual:");
    screenGotoxy(10, 5);

    for (int i = 0; i < MAX_JOGADORES; i++) {
        if (placar[i].tempo != 0) { // Mostra apenas entradas válidas
            screenGotoxy(10, 6 + i);
            printf("%d - %s: %d tempo\n", i + 1, placar[i].nome, placar[i].tempo);
        }
    }

    screenGotoxy(10, 12);
    printf("Pressione qualquer tecla para voltar ao menu...");
    screenUpdate(); // Atualiza a tela

    // Aguarda o usuário pressionar qualquer tecla antes de retornar ao menu
    while (!keyhit()) {
        usleep(100000);
    }
    readch(); // Lê a tecla para limpar o buffer de entrada
}


/*void rodarJogo(){ //runGame
    char key;
    char apelido[30];
    struct Jogador placar[MAX_JOGADORES];

    lerPlacar(placar);;
    screenHideCursor();

    do {
        
    }
}*/

int main(){
    char *telaInicial = "design.txt";
    screenClear();
    FILE *file = fopen(telaInicial, "r");
     if (file == NULL){
        printf("Erro ao abrir");
    }
    else{
        char linha[300];
        while (fgets(linha, sizeof(linha), file) != NULL){
            printf("%s", linha);
        }
        fclose(file);
    }
    Menu();
    return 0;
}