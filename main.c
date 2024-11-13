#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define UP_KEY 65
#define DOWN_KEY 66
#define PISTAS 6
#define DISTANCIA_PISTA 165
#define MAX_JOGADORES 5
#define TAMANHO_NOME 100

// Estrutura para o placar
typedef struct {
    char nome[TAMANHO_NOME];
    int tempo;
} Jogador;

// Funções
void displayMenu();
char receberInput();
void trocarPersonagem();
void iniciarJogo();
void runGame();
void atualizarObstaculos();
void displayJogo(int playerLugar);
void lerPlacar(Jogador placar[]);
void salvarPlacar(Jogador placar[]);
void inserirNoPlacar(Jogador placar[], char nome[], int tempo);
void exibirPlacar();

// Variáveis globais
int score = 0; // Pontuação inicial
int probabilidadeObstaculo = 4; // Probabilidade inicial de aparecer um obstáculo
int incrementaDificuldade = 0; // Controla a dificuldade
int playerLugar = 2; // Começa entre pista[2] e pista[3] (meio)
char *simboloJogador = "🚓";  // Carro
char **pista; // Ponteiro de ponteiro para pistas
char obstacles[PISTAS][DISTANCIA_PISTA];  // Matriz separada para obstáculos

int main() {
    // Inicializa as bibliotecas de terminal e temporizador
    screenInit(1);
    keyboardInit();
    timerInit(100);

    // Alocação dinâmica das pistas
    pista = (char **)malloc(PISTAS * sizeof(char *));
    for (int i = 0; i < PISTAS; i++) {
        pista[i] = (char *)malloc(DISTANCIA_PISTA * sizeof(char));
    }

    char escolhaUsuario;
    do {
        displayMenu();
        escolhaUsuario = receberInput();

        switch (escolhaUsuario) {
            case '1':
                iniciarJogo();
                runGame();
                break;
            case '2':
                trocarPersonagem();
                break;
            case '3':
                exibirPlacar();
                break;
        }
    } while (escolhaUsuario != 'q');

    // Libera memória das pistas
    for (int i = 0; i < PISTAS; i++) {
        free(pista[i]);
    }
    free(pista);

    // Finaliza as bibliotecas de terminal e temporizador
    screenDestroy();
    keyboardDestroy();
    timerDestroy();

    return 0;
}

void displayMenu() {
    screenClear();
    screenGotoxy(10, 5);
    printf("1) Iniciar Jogo");
    screenGotoxy(10, 7);
    printf("2) Trocar de Personagem");
    screenGotoxy(10, 9);
    printf("3) Exibir placar");
    screenGotoxy(10, 11);
    printf("Pressione 'q' para sair");
    screenUpdate();
}

char receberInput() {
    char key;
    do {
        while (!keyhit()) {
            usleep(100000); // Aguarda até uma tecla ser pressionada
        }
        key = readch();
    } while (key != '1' && key != '2' && key != '3' && key != 'q');
    return key;
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
                simboloJogador = "🐎";
                break;
            case '2':
                simboloJogador = "⏩";
                break;
            case '3':
                simboloJogador = "🚓";
                break;
        }
    } while (key != '1' && key != '2' && key != '3');
}

void iniciarJogo() {
    screenClear();
    // Inicializa as pistas
    strcpy(pista[0], "🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰");
    strcpy(pista[1], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[2], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[3], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[4], "➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖");
    strcpy(pista[5], "🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰🟰");
    // Inicializa obstáculos
    memset(obstacles, ' ', sizeof(obstacles));  // Limpa a matriz de obstáculos
}

void runGame() {
    char key;
    char nomeJogador[21];
    Jogador placar[MAX_JOGADORES]; // Array de jogadores para o placar

    lerPlacar(placar); // Carrega o placar existente
    screenHideCursor();

    do {
        displayJogo(playerLugar); // Exibe o jogo

        if (keyhit()) {
            key = readch();
            if ((key == 'w' || key == 'W' || key == UP_KEY) && playerLugar > 0) {
                playerLugar--;
            } else if ((key == 's' || key == 'S' || key == DOWN_KEY) && playerLugar < PISTAS - 2) {
                playerLugar++;
            }
        }

        if (score % 100 == 0 && score != 0 && score > incrementaDificuldade) {
            probabilidadeObstaculo++;  // Aumenta a chance de geração de obstáculos em 1%
            incrementaDificuldade = score;
        }

        if (timerTimeOver()) {
            atualizarObstaculos();   // Atualiza os obstáculos
            score++;  // Incrementa a pontuação
            screenUpdate();
            timerUpdateTimer(100);  // Reinicia o temporizador para 100 milissegundos
        }

        // Verifica se há colisão entre o jogador e um obstáculo
        if (obstacles[playerLugar][0] == '#' || obstacles[playerLugar][0] == '@' || obstacles[playerLugar][0] == '%' || obstacles[playerLugar][0] == '&') {
            screenClear();
            printf("\nGame Over! Você colidiu com um obstáculo.\n");
            sleep(2);
            keyboardDestroy();
            screenShowCursor();
            printf("Você fez %d tempo!\n", score);
            printf("Digite seu nome: ");
            scanf("%20s", nomeJogador);

            // Insere o jogador no placar
            inserirNoPlacar(placar, nomeJogador, score);
            salvarPlacar(placar); // Salva o placar atualizado no arquivo

            printf("Placar atualizado.\n");
            sleep(2); // Mostra a mensagem por 2 segundos

            score = 0;
            probabilidadeObstaculo = 4;
            break; // Sai do loop do jogo
        }

        screenUpdate();
    } while (key != 'q');

    // Reseta o jogo
    score = 0;
    probabilidadeObstaculo = 4;
}

void atualizarObstaculos() {
    int posicaoInicial = 39; // Posição dentro da matriz de obstáculos que reflete screenGotoxy(50, ...)

    // Mover obstáculos para a esquerda
    for (int i = 0; i < PISTAS - 1; i++) {
        for (int j = 1; j < DISTANCIA_PISTA; j++) {
            obstacles[i][j-1] = obstacles[i][j];
            obstacles[i][j] = ' ';
        }
    }

    // Gerar novos obstáculos aleatoriamente na última coluna
    char obstaculos[] = {'#', '@', '%', '&'};
    for (int i = 0; i < PISTAS - 1; i++) {
        if (rand() % 150 < probabilidadeObstaculo) {
            int randObstaculo = rand() % 4; // Gera um número entre 0 e 3 para escolher o obstáculo
            obstacles[i][posicaoInicial] = obstaculos[randObstaculo];
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
            printf("%c", obstacles[i][j]);  // Desenha o obstáculo
        }
    }

    // Desenha o jogador na nova posição
    screenGotoxy(13, 4 + playerLugar * espacamento);
    printf("%s", simboloJogador);

    // Atualiza a tela
    screenUpdate();

    // Exibe a pontuação
    screenGotoxy(12, 3 + PISTAS * espacamento + 2);  // Posição abaixo das pistas
    printf("Score: %d", score);  // Exibe a pontuação
}

void lerPlacar(Jogador placar[]) {
    FILE *arquivo = fopen("placar.txt", "r");
    if (arquivo == NULL) {
        for (int i = 0; i < MAX_JOGADORES; i++) {
            sprintf(placar[i].nome, "-");
            placar[i].tempo = 0;
        }
    } else {
        for (int i = 0; i < MAX_JOGADORES; i++) {
            fscanf(arquivo, "%*d - %99s %d", placar[i].nome, &placar[i].tempo);
            if (placar[i].tempo == 0) strcpy(placar[i].nome, "-");
        }
        fclose(arquivo);
    }
}

void salvarPlacar(Jogador placar[]) {
    FILE *arquivo = fopen("placar.txt", "w");
    for (int i = 0; i < MAX_JOGADORES; i++) {
        fprintf(arquivo, "%d - %s %d\n", i + 1, placar[i].nome, placar[i].tempo);
    }
    fclose(arquivo);
}

void inserirNoPlacar(Jogador placar[], char nome[], int tempo) {
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
    Jogador placar[MAX_JOGADORES];
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
