#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define UP 87 
#define DOWN 83 
#define QUANT_PISTAS 6
#define DISTANCIA_PISTA 165
#define MAX_JOGADORES 10
#define TAMANHO_NOME 50

struct Jogador{
    char nome[TAMANHO_NOME];
    int pontos;
};

int score = 0;
int probabilidade = 4; 
int dificuldade = 0; 
int local = 2; 
char *Carro = "🚗";  
char **pista; 
char obstaculo[QUANT_PISTAS][DISTANCIA_PISTA];  

void desenho()
{
    screenClear();
    FILE *file = fopen("design.txt", "r");
    char linha[300];
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo design.txt\n");
        return;
    }
    else{
        screenGotoxy(5, 5);
        while (fgets(linha, sizeof(linha), file) != NULL) {
            printf("%s", linha);
        }
    }

    screenSetColor(RED, BLACK);

    fclose(file);
}

void Menu() {
    screenClear();
    desenho();
    screenGotoxy(10, 5);
    printf("1. Iniciar Jogo");
    screenGotoxy(10, 7);
    printf("2. Trocar de Personagem");
    screenGotoxy(10, 9);
    printf("3. Exibir placar");
    screenGotoxy(10, 11);
    printf("4. Sair");
    screenGotoxy(10, 13);
    printf("Digite o número que você deseja: ");
    screenUpdate();
}

char input() {
    char key;
    do {
        while (!keyhit()) {
            usleep(100000);
        }
        key = readch();
    } while (key != '1' && key != '2' && key != '3' && key != '4');
    return key;
}

void trocarPersonagem() {
    screenClear();
    screenGotoxy(10, 5);
    printf("Escolha um personagem:");
    screenGotoxy(10, 7);
    printf("1. 🚗");
    screenGotoxy(10, 9);
    printf("2. 🚓");
    screenGotoxy(10, 11);
    printf("3. 🛻");
    screenGotoxy(10, 13);
    printf("4. 🏎️");
    screenGotoxy(10, 15);
    printf("5. 🏍️");
    screenGotoxy(10, 17);
    printf("6. Voltar");
    screenUpdate();

    char x;
    do{
        while (!keyhit()) {
            usleep(100000);
        }
        x = readch(); 
        switch(x){
            case '1':
                Carro = "🚗";
                break;
            case '2':
                Carro = "🚓";
                break;
            case '3':
                Carro = "🛻";
                break;
            case '4':
                Carro = "🏎️";
                break;
            case '5':
                Carro = "🏍️";
                break;
        }
    } while (x != '1' && x != '2' && x != '3' && x != '4' && x != '5');
}

void iniciarPista() {
    screenClear();
    strcpy(pista[0], "🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰");
    strcpy(pista[1], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[2], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[3], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[4], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[5], "🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰");
    memset(obstaculo, ' ', sizeof(obstaculo)); 
}

void rodarJogo() {
    char contr;
    char nomeJogador[21];
    struct Jogador placar[MAX_JOGADORES]; 

    lerPlacar(placar); 
    screenHideCursor();

    do {
        baseJogo(local);

        if (keyhit()) {
            contr = readch();
            if ((contr == 'w' || contr == 'W' || contr == UP) && local > 0) {
                local--;
            } else if ((contr == 's' || contr == 'S' || contr == DOWN) && local < QUANT_PISTAS - 2) {
                local++;
            }
        }

        if (score % 100 == 0 && score != 0 && score > dificuldade) {
            probabilidade++; 
            dificuldade = score;
        }

        if (timerTimeOver()) {
            atualizarObstaculos();  
            score++; 
            screenUpdate();
            timerUpdateTimer(100);  
        }

        if (obstaculo[local][0] == '🗑️' || obstaculo[local][0] == '👮' || obstaculo[local][0] == '🛒' || obstaculo[local][0] == '🛞') {
            screenClear();
            printf("\nGame Over! Você colidiu com um obstáculo.\n");
            sleep(2);
            keyboardDestroy();
            screenShowCursor();
            printf("Você fez %d pontos!\n", score);
            printf("Digite seu nome: ");
            scanf("%20s", nomeJogador);

            inserirNoPlacar(placar, nomeJogador, score);
            salvarPlacar(placar); 

            printf("Placar atualizado.\n");
            sleep(2); 

            score = 0;
            probabilidade = 4;
            break; 
        }

        screenUpdate();
    } while (contr != '4');

    score = 0;
    probabilidade = 4;
}

void atualizarObstaculos() {
    int posicaoInicial = 39; 

    for (int i = 0; i < QUANT_PISTAS - 1; i++) {
        for (int j = 1; j < DISTANCIA_PISTA; j++) {
            obstaculo[i][j-1] = obstaculo[i][j];
            obstaculo[i][j] = ' ';
        }
    }

    char obstaculos[] = {'🗑️', '👮', '🛒', '🛞'};
    for (int i = 0; i < QUANT_PISTAS - 1; i++) {
        if (rand() % 150 < probabilidade) {
            int randObstaculo = rand() % 4; 
            obstaculo[i][posicaoInicial] = obstaculos[randObstaculo];
        }
    }
}

void baseJogo(int local) {
    int espacamento = 2; 

    for (int i = 0; i < QUANT_PISTAS; i++) {
        screenGotoxy(12, 3 + i * espacamento);
        printf("%s", pista[i]);
    }

    for (int i = 0; i < QUANT_PISTAS; i++) {
        for (int j = 0; j < DISTANCIA_PISTA; j++) {
            screenGotoxy(13 + j, 4 + i * espacamento); 
            printf("%c", obstaculo[i][j]);  
        }
    }

    screenGotoxy(13, 4 + local * espacamento);
    printf("%s", Carro);

    screenUpdate();

    screenGotoxy(12, 3 + QUANT_PISTAS * espacamento + 2); 
    printf("Score: %d", score); 
}

void lerPlacar(struct Jogador placar[]) {
    FILE *arquivo = fopen("placar.txt", "r");
    if (arquivo == NULL) {
        for (int i = 0; i < MAX_JOGADORES; i++) {
            sprintf(placar[i].nome, "-");
            placar[i].pontos = 0;
        }
    } else {
        for (int i = 0; i < MAX_JOGADORES; i++) {
            fscanf(arquivo, "%*d - %99s %d", placar[i].nome, &placar[i].pontos);
            if (placar[i].pontos == 0) strcpy(placar[i].nome, "-");
        }
        fclose(arquivo);
    }
}

void salvarPlacar(struct Jogador placar[]) {
    FILE *arquivo = fopen("placar.txt", "w");
    for (int i = 0; i < MAX_JOGADORES; i++) {
        fprintf(arquivo, "%d - %s %d\n", i + 1, placar[i].nome, placar[i].pontos);
    }
    fclose(arquivo);
}

void inserirPlacar(struct Jogador placar[], char nome[], int pontos) {
    int posicao = MAX_JOGADORES;
    for (int i = MAX_JOGADORES - 1; i >= 0; i--) {
        if (pontos > placar[i].pontos) {
            posicao = i;
        } else {
            break;
        }
    }

    if (posicao < MAX_JOGADORES) {
        for (int i = MAX_JOGADORES - 1; i > posicao; i--) {
            strcpy(placar[i].nome, placar[i - 1].nome);
            placar[i].pontos = placar[i - 1].pontos;
        }
        strcpy(placar[posicao].nome, nome);
        placar[posicao].pontos = pontos;
    }
}

void exibirPlacar() {
    struct Jogador placar[MAX_JOGADORES];
    lerPlacar(placar);

    screenClear(); 
    screenGotoxy(10, 3);
    printf("Placar Atual:");
    screenGotoxy(10, 5);

    for (int i = 0; i < MAX_JOGADORES; i++) {
        if (placar[i].pontos != 0) { 
            screenGotoxy(10, 6 + i);
            printf("%d - %s: %d pontos\n", i + 1, placar[i].nome, placar[i].pontos);
        }
    }

    screenGotoxy(10, 12);
    printf("Pressione qualquer tecla para voltar ao menu...");
    screenUpdate();

    while (!keyhit()) {
        usleep(100000);
    }
    readch(); 
}

int main() {
    screenInit(1);
    keyboardInit();
    timerInit(100);

    pista = (char **)malloc(QUANT_PISTAS * sizeof(char *));
    for (int i = 0; i < QUANT_PISTAS; i++) {
        pista[i] = (char *)malloc(DISTANCIA_PISTA * sizeof(char));
    }

    char escolhaUsuario;
    do {
        desenho();
        Menu();
        escolhaUsuario = input();

        switch (escolhaUsuario) {
            case '1':
                iniciarPista();
                rodarJogo();
                break;
            case '2':
                trocarPersonagem();
                break;
            case '3':
                exibirPlacar();
                break;
        }
    } while (escolhaUsuario != '4');

    for (int i = 0; i < QUANT_PISTAS; i++) {
        free(pista[i]);
    }
    free(pista);

    screenDestroy();
    keyboardDestroy();
    timerDestroy();

    return 0;
}