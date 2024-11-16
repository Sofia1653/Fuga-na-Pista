#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define MAXX 30
#define MAXY 15
#define MINX 5
#define MINY 5
#define VAZIO ' '
#define CARRO 'A'
#define OBSTACULO 'O'

char matriz[MAXY][MAXX]; // Matriz do espaço do jogo
int carrinho = MAXX / 2;  // A posição inicial do carro

// Função para desenhar a borda e a matriz
void drawMatriz() {
    screenClear();  // Limpa a tela

    // Desenha a matriz de jogo (campo + obstáculos + carro)
    for (int i = 0; i < MAXY; i++) {
        for (int j = 0; j < MAXX; j++) {
            screenGotoxy(j + 1, i + 1);  // Move o cursor para (j, i)
            putchar(matriz[i][j]);
        }
    }
    screenUpdate();  // Atualiza a tela
}

// Função para colocar o carro na posição inicial
void lugarCarro() {
    matriz[MAXY - 1][carrinho] = CARRO;  // Coloca o carro na última linha
}

// Função para colocar obstáculos na matriz
void lugarObstaculos(int row, int col) {
    if (row < MAXY && col < MAXX) {
        matriz[row][col] = OBSTACULO;  // Coloca um obstáculo na posição (row, col)
    }
}

// Função para mover o carro
void movimentosCarro(int direction) {
    matriz[MAXY - 1][carrinho] = VAZIO;  // Remove o carro da posição atual
    carrinho += direction;  // Move o carro para a esquerda ou direita
    
    // Impede que o carrinho saia das bordas da tela
    if (carrinho < MINX) carrinho = MINX;
    if (carrinho >= MAXX) carrinho = MAXX - 1;
    
    matriz[MAXY - 1][carrinho] = CARRO;  // Coloca o carro na nova posição
}

// Função para verificar se há colisão com obstáculos
int verificarColisao() {
    return matriz[MAXY - 2][carrinho] == OBSTACULO;  // Verifica se o carro colidiu com um obstáculo
}

// Função para atualizar a posição dos obstáculos
void atualizarObstaculo() {
    // Move os obstáculos para baixo e remove os obstáculos da linha do carrinho
    for (int i = MAXY - 2; i >= 0; i--) {  // Começa da penúltima linha
        for (int j = 0; j < MAXX; j++) {
            if (matriz[i][j] == OBSTACULO) {
                // Move o obstáculo para a linha abaixo
                matriz[i + 1][j] = OBSTACULO;
                matriz[i][j] = VAZIO;
            }
        }
    }
    
    // Adiciona um novo obstáculo aleatoriamente na primeira linha
    if (rand() % 4 == 0) {  // Probabilidade de adicionar um obstáculo (1 em 4)
        int novo = rand() % (MAXX - MINX) + MINX;  // Gera uma posição aleatória para o obstáculo
        lugarObstaculos(0, novo);  // Coloca o obstáculo na linha 0
    }
}

// Função para desenhar as bordas e o campo de jogo
void desenharBordas() {
    for (int i = 0; i < MAXY; i++) {
        for (int j = 0; j < MAXX; j++) {
            if (i == 0 || i == MAXY - 1 || j == 0 || j == MAXX - 1) {
                matriz[i][j] = '#';  // Desenha as bordas
            } else {
                matriz[i][j] = VAZIO;  // Espaço vazio
            }
        }
    }
}

// Função principal do jogo
void startGame() {
    srand(time(NULL));  // Gera números aleatórios para os obstáculos
    keyboardInit();  // Inicializa o teclado
    screenInit(1);  // Inicializa a tela com bordas
    desenharBordas();  // Desenha as bordas
    lugarCarro();  // Coloca o carro na posição inicial

    while (1) {
        atualizarObstaculo();  // Atualiza a posição dos obstáculos
        drawMatriz();  // Redesenha a tela com obstáculos e o carro

        if (verificarColisao()) {  // Verifica se houve colisão
            screenGotoxy(1, MAXY + 2);  // Exibe a mensagem de Game Over
            printf("Você colidiu! Game Over!\n");
            break;  // Encerra o jogo
        }

        if (keyhit()) {  // Verifica se o jogador pressionou uma tecla
            char key = readch();  // Lê a tecla pressionada
            if (key == 'a') movimentosCarro(-1);  // Mover para a esquerda
            else if (key == 'd') movimentosCarro(1);  // Mover para a direita
            else if (key == 'q') break;  // Se pressionar 'q', sai do jogo
        }

        timerInit(500);  // Delay de 500ms para controle de velocidade
        while (!timerTimeOver()) {
            // Aguarda o timer para a próxima atualização
        }
    }

    screenDestroy();  // Finaliza a tela
    keyboardDestroy();  // Finaliza o teclado
    timerDestroy();  // Finaliza o timer
}

// Função de menu
int main() {
    int choice;

    while (1) {
        screenClear();  // Limpa a tela
        printf("=== Jogo do Carrinho ===\n");
        printf("1. Iniciar Jogo\n");
        printf("2. Sair\n");
        printf("=========================\n");

        scanf("%d", &choice);
        getchar();  // Limpa o buffer de entrada

        if (choice == 1) {
            startGame();  // Inicia o jogo
        } else if (choice == 2) {
            printf("Saindo do jogo. Até logo!\n");
            break;  // Encerra o menu
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }

    return 0;
}
