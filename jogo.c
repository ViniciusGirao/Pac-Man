#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h> // Para gerar números aleatórios

#define LARGURA 40  // Largura do tabuleiro
#define ALTURA 20 // Altura do tabuleiro
#define PACMAN 'C'
#define PAREDE '#'
#define COMIDA '.'
#define VAZIO ' '
#define DEMONIO 'X'

// Definição da struct para o estado do jogo
typedef struct {
    int pacman_x, pacman_y;
    int pontuacao;
    int comida;
    int nivel_atual;
    int contagem_demonios;
    int contador_movimentos;
    int **posicoes_comida;
    char **tabuleiro;
    int *demonio_x, *demonio_y;
} EstadoJogo;

// Definir o estado inicial do jogo
EstadoJogo jogo;
int i, j, f, k;

// Função para inicializar o jogo
void inicializar() {
    // Aloca dinamicamente a memória para o tabuleiro e a matriz de alimentos
    jogo.tabuleiro = (char*)malloc(ALTURA * sizeof(char));  // Um array de ponteiros
    for (i = 0; i < ALTURA; i++) {
        jogo.tabuleiro[i] = (char*)malloc(LARGURA * sizeof(char));
    }

    jogo.posicoes_comida = (int*)malloc(ALTURA * sizeof(int));  // Um array de ponteiros
    for (i = 0; i < ALTURA; i++) {
        jogo.posicoes_comida[i] = (int*)malloc(LARGURA * sizeof(int));
    }

    // Inicializa as variáveis
    for (i = 0; i < ALTURA; i++) {
        for (j = 0; j < LARGURA; j++) {
            jogo.tabuleiro[i][j] = VAZIO;
            jogo.posicoes_comida[i][j] = 0;
        }
    }

    // Paredes como limite do jogo
    for (i = 0; i < ALTURA; i++) {
        jogo.tabuleiro[i][0] = PAREDE;
        jogo.tabuleiro[i][LARGURA - 1] = PAREDE;
    }
    for (j = 0; j < LARGURA; j++) {
        jogo.tabuleiro[0][j] = PAREDE;
        jogo.tabuleiro[ALTURA - 1][j] = PAREDE;
    }

    // Configurações específicas por nível
    jogo.contagem_demonios = 2 + jogo.nivel_atual * 2; // Mais demônios a cada nível
    int paredes = 10 + jogo.nivel_atual * 5;  // Mais paredes a cada nível
    jogo.comida = 10 + jogo.nivel_atual * 5; // Mais comida a cada nível

    // Aloca dinamicamente a memória para as posições dos demônios
    jogo.demonio_x = (int*)malloc(jogo.contagem_demonios * sizeof(int));
    jogo.demonio_y = (int*)malloc(jogo.contagem_demonios * sizeof(int));

    // Coloca paredes aleatórias
    while (paredes--) {
        int linha = (rand() % (ALTURA - 2)) + 1;
        int coluna = (rand() % (LARGURA - 2)) + 1;
        if (jogo.tabuleiro[linha][coluna] == VAZIO) {
            jogo.tabuleiro[linha][coluna] = PAREDE;
        }
    }

    // Coloca alimentos aleatórios
    for (f = jogo.comida; f > 0;) {
        int linha, coluna;
        do {
            linha = rand() % (ALTURA - 2) + 1;
            coluna = rand() % (LARGURA - 2) + 1;
        } while (jogo.tabuleiro[linha][coluna] != VAZIO || jogo.posicoes_comida[linha][coluna] == 1);
        jogo.tabuleiro[linha][coluna] = COMIDA;
        jogo.posicoes_comida[linha][coluna] = 1; // Marca a posição do alimento
        f--;
    }

    // Coloca demônios aleatórios
    for (k = 0; k < jogo.contagem_demonios; k++) {
        do {
            jogo.demonio_x[k] = rand() % (LARGURA - 2) + 1;
            jogo.demonio_y[k] = rand() % (ALTURA - 2) + 1;
        } while (jogo.tabuleiro[jogo.demonio_y[k]][jogo.demonio_x[k]] != VAZIO);
        jogo.tabuleiro[jogo.demonio_y[k]][jogo.demonio_x[k]] = DEMONIO;
    }

    // Define a posição inicial do Pac-Man
    jogo.pacman_x = LARGURA / 2;
    jogo.pacman_y = ALTURA / 2;
    jogo.tabuleiro[jogo.pacman_y][jogo.pacman_x] = PACMAN;
}

// Função para desenhar o jogo
void desenhar() {
    system("cls");
    printf("Fase: %d\n", jogo.nivel_atual + 1);
    for (i = 0; i < ALTURA; i++) {
        for (j = 0; j < LARGURA; j++) {
            if (jogo.tabuleiro[i][j] == PAREDE) {
                printf("\033[44m\033[34m%c\033[0m", jogo.tabuleiro[i][j]); // Fundo azul, texto branco
            } else if (jogo.tabuleiro[i][j] == PACMAN) {
                printf("\033[43m\033[33m%c\033[0m", jogo.tabuleiro[i][j]); // Fundo amarelo, texto amarelo
            } else if (jogo.tabuleiro[i][j] == COMIDA) {
                printf("\033[32m%c\033[0m", jogo.tabuleiro[i][j]); // Fundo verde, texto verde
            } else if (jogo.tabuleiro[i][j] == DEMONIO) {
                printf("\033[41m\033[31m%c\033[0m", jogo.tabuleiro[i][j]); // Fundo vermelho, texto vermelho
            } else {
                printf("%c", jogo.tabuleiro[i][j]); // Caracteres padrão
            }
        }
        printf("\n");
    }
    printf("Pontuação: %d\n", jogo.pontuacao);
    printf("Alimentos restantes: %d\n", jogo.comida);
}

// Função para mover o Pac-Man
void mover(int movimento_x, int movimento_y) {
    int x = jogo.pacman_x + movimento_x;
    int y = jogo.pacman_y + movimento_y;

    if (jogo.tabuleiro[y][x] != PAREDE) {
        if (jogo.tabuleiro[y][x] == COMIDA) {
            jogo.pontuacao++;
            jogo.comida--;
            jogo.posicoes_comida[y][x] = 0; // Remove a marcação do alimento
            if (jogo.comida <= 0) {
                jogo.nivel_atual++;
                if (jogo.nivel_atual < 5) {
                    inicializar(); // Próximo nível
                } else {
                    printf("Parabéns! Você venceu todas as fases.\nSua pontuação: %d\n", jogo.pontuacao);
                    exit(0); // Vitória
                }
                return;
            }
        }

        if (jogo.tabuleiro[y][x] == DEMONIO) {
            printf("Game Over! Você foi pego pelos demônios.\nSua pontuação: %d\n", jogo.pontuacao);
            exit(0); // Derrota
        }

        jogo.tabuleiro[jogo.pacman_y][jogo.pacman_x] = VAZIO;
        jogo.pacman_x = x;
        jogo.pacman_y = y;
        jogo.tabuleiro[jogo.pacman_y][jogo.pacman_x] = PACMAN;
    }
}

// Função para mover os demônios
void moverDemonios() {
    jogo.contador_movimentos++;

    if (jogo.contador_movimentos >= 3) {
        for (k = 0; k < jogo.contagem_demonios; k++) {
            int movimento_x, movimento_y;

            if (jogo.pacman_x > jogo.demonio_x[k]) movimento_x = 1;
            else if (jogo.pacman_x < jogo.demonio_x[k]) movimento_x = -1;
            else movimento_x = 0;

            if (jogo.pacman_y > jogo.demonio_y[k]) movimento_y = 1;
            else if (jogo.pacman_y < jogo.demonio_y[k]) movimento_y = -1;
            else movimento_y = 0;

            if (jogo.tabuleiro[jogo.demonio_y[k] + movimento_y][jogo.demonio_x[k] + movimento_x] != PAREDE &&
                jogo.tabuleiro[jogo.demonio_y[k] + movimento_y][jogo.demonio_x[k] + movimento_x] != DEMONIO) {
                // Restaura alimentos se demônios saírem de suas posições
                if (jogo.posicoes_comida[jogo.demonio_y[k]][jogo.demonio_x[k]] == 1) {
                    jogo.tabuleiro[jogo.demonio_y[k]][jogo.demonio_x[k]] = COMIDA;
                } else {
                    jogo.tabuleiro[jogo.demonio_y[k]][jogo.demonio_x[k]] = VAZIO;
                }

                jogo.demonio_x[k] += movimento_x;
                jogo.demonio_y[k] += movimento_y;

                if (jogo.pacman_y == jogo.demonio_y[k] && jogo.pacman_x == jogo.demonio_x[k]) {
                    printf("Game Over! Você foi pego pelos demônios.\nSua pontuação: %d\n", jogo.pontuacao);
                    exit(0); // Derrota
                } else {
                    jogo.tabuleiro[jogo.demonio_y[k]][jogo.demonio_x[k]] = DEMONIO;
                }
            }
        }
        jogo.contador_movimentos = 0; // Reseta o contador
    }
}

// Função para exibir o menu e obter a escolha
int exibirMenu() {
    int opcao;
    system("cls");
    printf("MENU PRINCIPAL\n");
    printf("1 - Iniciar jogo\n");
    printf("2 - Ver controles\n");
    printf("3 - Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    return opcao;
}

// Função principal
int main() {
    srand(time(NULL));

    setlocale(LC_ALL, "Portuguese");

    int opcao;

    while (1) {
        opcao = exibirMenu();

        switch (opcao) {
            case 1:
                jogo.nivel_atual = 0;
                jogo.pontuacao = 0;
                inicializar();
                while (1) {
                    desenhar();
                    moverDemonios();
                    char ch = getch();
                    switch (ch) {
                        case 'w': mover(0, -1); break;
                        case 's': mover(0, 1); break;
                        case 'a': mover(-1, 0); break;
                        case 'd': mover(1, 0); break;
                        case 'q':
                            printf("Jogo encerrado. Pontuação: %d\n", jogo.pontuacao);
                            return 0;
                    }
                }
                break;
            case 2:
                printf("Controles:\n");
                printf("w - Mover para cima\n");
                printf("s - Mover para baixo\n");
                printf("a - Mover para a esquerda\n");
                printf("d - Mover para a direita\n");
                printf("q - Sair do jogo\n");
                system("pause");
                break;
            case 3:
                printf("Saindo... Obrigado por jogar!\n");
                return 0;
            default:
                printf("Opção inválida! Tente novamente.\n");
                system("pause");
                break;
        }
    }

    return 0;
}
