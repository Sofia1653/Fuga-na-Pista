#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Biblioteca para usleep
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define MAXX 30
#define MAXY 30
#define MINX 5
#define MINY 5
#define VAZIO ' '
#define CARRO 'A'  
#define OBSTACULO 'O'

char matriz[MAXY][MAXX]; // matriz do espaço que tem para o jogo
int carrinho = MAXX / 2; // a posição que o carro vai começar

// Função para desenhar a borda e a matriz
void drawMatriz() {
    int startX = MINX;
    int startY = MINY;
    int width = MAXX - MINX;
    int height = MAXY - MINY;

    screenClear();

    // Desenha as linhas horizontais (borda superior e inferior)
    for (int x = startX; x <= width; x++) {
        screenGotoxy(x, startY);    // Linha superior
        putchar('-');
        screenGotoxy(x, startY + height);  // Linha inferior
        putchar('-');
    }

    // Desenha as linhas verticais (borda esquerda e direita)
    for (int y = startY; y <= height; y++) {
        screenGotoxy(startX, y);    // Linha esquerda
        putchar('|');
        screenGotoxy(startX + width, y);  // Linha direita
        putchar('|');
    }

    // Desenha a matriz de jogo (campo + obstáculos + carro)
    for (int i = 0; i < MAXY; i++) {
        for (int j = 0; j < MAXX; j++) {
            screenGotoxy(j + 1, i + 1);  // Move o cursor para (j, i)
            putchar(matriz[i][j]);
        }
    }
    screenUpdate(); 
}

// Função para colocar o carro na posição inicial
void lugarCarro() {
    matriz[MAXY - 1][carrinho] = CARRO;
}

// Função para colocar obstáculos na matriz
void lugarObstaculos(int row, int col) {
    if (row < MAXY && col < MAXX) {
        matriz[row][col] = OBSTACULO;
    }
}

// Função para mover o carro
void movimentosCarro(int direction) {
    matriz[MAXY - 1][carrinho] = VAZIO; // Remove o carro da posição atual
    carrinho += direction; // Move o carro para a esquerda ou direita
    
    // Impede que o carrinho saia das bordas da tela
    if (carrinho < MINX) carrinho = MINX;
    if (carrinho >= MAXX) carrinho = MAXX - 1;
    
    matriz[MAXY - 1][carrinho] = CARRO; // Coloca o carro na nova posição
}

// Função para verificar se há colisão com obstáculos
int verificarColisao() {
    return matriz[MAXY - 2][carrinho] == OBSTACULO;
}

// Função para atualizar a posição dos obstáculos
void atualizarObstaculo() {
    // Move os obstáculos para baixo e remove os obstáculos na linha do carrinho
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
    if (rand() % 3 == 0) {
        int novo = rand() % (MAXX - MINX) + MINX; // Gera uma posição aleatória para o obstáculo
        lugarObstaculos(0, novo);
    }
}

/*void pistas(){
    fazer uma função pra deixar mais arrumadinho as pistas
}*/

// Função principal do jogo
void startGame() {
    srand(time(NULL)); // gerar os obstáculos aleatoriamente
    keyboardInit();
    screenInit(1); // Inicializa a tela com bordas
    drawMatriz();
    lugarCarro();

    while (1) {
        atualizarObstaculo(); // Atualiza a posição dos obstáculos
        drawMatriz(); // Redesenha a tela com os obstáculos e o carro
        
        if (verificarColisao()) {
            screenGotoxy(1, MAXY + 2); // Exibe a mensagem de Game Over
            printf("Você colidiu! Game Over!\n");
            break;
        }

        if (keyhit()) {
            char key = readch();
            if (key == 'a') movimentosCarro(-1);  // Mover para a esquerda
            else if (key == 'd') movimentosCarro(1);  // Mover para a direita
            else if (key == 'q') break;  // Se pressionar 'q', sai do jogo
        }

        usleep(200000); // Delay de 200ms para controle de velocidade
    }

    screenDestroy();  // Finaliza a tela
    keyboardDestroy();
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
            startGame();
        } else if (choice == 2) {
            printf("Saindo do jogo. Até logo!\n");
            break;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }

    return 0;
}
