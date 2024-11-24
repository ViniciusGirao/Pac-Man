#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h> // Para gerar números aleatórios

#define WIDTH 40  // Largura do tabuleiro
#define HEIGHT 20 // Altura do tabuleiro
#define PACMAN 'C'
#define WALL '#'
#define FOOD '.'
#define EMPTY ' '
#define DEMON 'X'

// Variáveis globais
int i, j, k, f;
int res = 0;
int score = 0;
int pacman_x, pacman_y;
char board[HEIGHT][WIDTH];
int food = 0;
int curr = 0;
int demon_x[8], demon_y[8]; // Posições dos demônios (até 8 demônios)
int demon_count;            // Número de demônios
int current_level = 0;      // Nível atual do jogo
int total_levels = 5;       // Total de níveis
int move_counter = 0;       // Contador para controlar a movimentação dos demônios
const int base_food_count = 10; // Quantidade inicial de comida
const int move_delay = 3;       // Reduzido para tornar os demônios mais rápidos

// Matriz auxiliar para rastrear alimentos
int food_positions[HEIGHT][WIDTH] = {0};

// Função para inicializar o jogo
void initialize() {
    // Limpa o tabuleiro e a matriz de alimentos
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            board[i][j] = EMPTY;
            food_positions[i][j] = 0;
        }
    }

    // Colocando paredes como limite do jogo
    for (i = 0; i < HEIGHT; i++) {
        board[i][0] = WALL;
        board[i][WIDTH - 1] = WALL;
    }
    for (j = 0; j < WIDTH; j++) {
        board[0][j] = WALL;
        board[HEIGHT - 1][j] = WALL;
    }

    // Configurações específicas por nível
    demon_count = 2 + current_level * 2; // Mais demônios a cada nível
    int walls = 10 + current_level * 5;  // Mais paredes a cada nível
    food = base_food_count + current_level * 5; // Mais comida a cada nível

    // Coloca paredes aleatórias
    while (walls--) {
        int row = (rand() % (HEIGHT - 2)) + 1;
        int col = (rand() % (WIDTH - 2)) + 1;
        if (board[row][col] == EMPTY) {
            board[row][col] = WALL;
        }
    }

    // Coloca alimentos aleatórios
    for (f = food; f > 0;) {
        int row, col;
        do {
            row = rand() % (HEIGHT - 2) + 1;
            col = rand() % (WIDTH - 2) + 1;
        } while (board[row][col] != EMPTY || food_positions[row][col] == 1);
        board[row][col] = FOOD;
        food_positions[row][col] = 1; // Marca a posição do alimento
        f--;
    }

    // Coloca demônios aleatórios
    for (k = 0; k < demon_count; k++) {
        do {
            demon_x[k] = rand() % (WIDTH - 2) + 1;
            demon_y[k] = rand() % (HEIGHT - 2) + 1;
        } while (board[demon_y[k]][demon_x[k]] != EMPTY);
        board[demon_y[k]][demon_x[k]] = DEMON;
    }

    // Define a posição inicial do Pac-Man
    pacman_x = WIDTH / 2;
    pacman_y = HEIGHT / 2;
    board[pacman_y][pacman_x] = PACMAN;
}

// Função para desenhar o jogo
void draw() {
    system("cls");
    printf("Fase: %d\n", current_level + 1);
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
    printf("Alimentos restantes: %d\n", food);
}

// Função para mover o Pac-Man
void move(int move_x, int move_y) {
    int x = pacman_x + move_x;
    int y = pacman_y + move_y;

    if (board[y][x] != WALL) {
        if (board[y][x] == FOOD) {
            score++;
            food--;
            curr++;
            food_positions[y][x] = 0; // Remove a marcação do alimento
            if (food <= 0) {
                current_level++;
                if (current_level < total_levels) {
                    initialize(); // Próximo nível
                } else {
                    res = 2; // Vitória
                }
                return;
            }
        }

        if (board[y][x] == DEMON) {
            res = 1; // Derrota
        }

        board[pacman_y][pacman_x] = EMPTY;
        pacman_x = x;
        pacman_y = y;
        board[pacman_y][pacman_x] = PACMAN;
    }
}

// Função para mover os demônios
void moveDemons() {
    move_counter++;

    if (move_counter >= move_delay) {
        for (k = 0; k < demon_count; k++) {
            int move_x, move_y;

            if (pacman_x > demon_x[k]) move_x = 1;
            else if (pacman_x < demon_x[k]) move_x = -1;
            else move_x = 0;

            if (pacman_y > demon_y[k]) move_y = 1;
            else if (pacman_y < demon_y[k]) move_y = -1;
            else move_y = 0;

            if (board[demon_y[k] + move_y][demon_x[k] + move_x] != WALL &&
                board[demon_y[k] + move_y][demon_x[k] + move_x] != DEMON) {
                // Restaura alimentos se demônios saírem de suas posições
                if (food_positions[demon_y[k]][demon_x[k]] == 1) {
                    board[demon_y[k]][demon_x[k]] = FOOD;
                } else {
                    board[demon_y[k]][demon_x[k]] = EMPTY;
                }

                demon_x[k] += move_x;
                demon_y[k] += move_y;

                if (pacman_y == demon_y[k] && pacman_x == demon_x[k]) {
                    res = 1;
                } else {
                    board[demon_y[k]][demon_x[k]] = DEMON;
                }
            }
        }
        move_counter = 0; // Reseta o contador
    }
}

// Função principal
int main() {
    srand(time(NULL));
    initialize();

    char ch;
    
    setlocale(LC_ALL, "Portuguese");
    printf("Use as teclas: w (cima), a (esquerda), d (direita), s (baixo)\n");
    printf("Pressione q para sair\n");

    while (1) {
        draw();

        if (res == 1) {
            system("cls");
            printf("Game Over! Você foi pego pelos demônios.\nSua pontuação: %d\n", score);
            return 0;
        }

        if (res == 2) {
            system("cls");
            printf("Parabéns! Você venceu todas as fases.\nSua pontuação: %d\n", score);
            return 0;
        }

        moveDemons();

        ch = getch();
        switch (ch) {
            case 'w': move(0, -1); break;
            case 's': move(0, 1); break;
            case 'a': move(-1, 0); break;
            case 'd': move(1, 0); break;
            case 'q':
                printf("Jogo encerrado. Pontuação: %d\n", score);
                return 0;
        }
    }

    return 0;
}
