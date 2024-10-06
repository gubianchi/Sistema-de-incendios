
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "funcoes.h"

// Defini��o da matriz da floresta
char floresta[TAMANHO][TAMANHO];

// Defini��o dos mutexes e vari�veis de condi��o
pthread_mutex_t mutex[TAMANHO][TAMANHO];
pthread_cond_t cond[TAMANHO][TAMANHO];

// Inicializa a floresta e as estruturas de sincroniza��o
void inicializar_floresta() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            floresta[i][j] = SENSOR; // Cada c�lula tem um sensor ativo
            pthread_mutex_init(&mutex[i][j], NULL);
            pthread_cond_init(&cond[i][j], NULL);
        }
    }
}

// Fun��o executada por cada n� sensor
void *funcao_sensor(void *param) {
    int *pos = (int *)param;
    int x = pos[0], y = pos[1];

    while (1) {
        pthread_mutex_lock(&mutex[x][y]);

        // Verifica se h� fogo na c�lula atual
        if (floresta[x][y] == FOGO) {
            printf("Sensor [%d,%d] detectou inc�ndio!\n", x, y);

            // Notifica os vizinhos
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (x+i >= 0 && x+i < TAMANHO && y+j >= 0 && y+j < TAMANHO) {
                        pthread_mutex_lock(&mutex[x+i][y+j]);
                        pthread_cond_signal(&cond[x+i][y+j]);
                        pthread_mutex_unlock(&mutex[x+i][y+j]);
                    }
                }
            }

            // Se for um n� de borda, notifica a central
            if (x == 0 || x == TAMANHO-1 || y == 0 || y == TAMANHO-1) {
                pthread_cond_signal(&cond[0][0]); // Exemplo de envio de mensagem
            }
        }

        pthread_mutex_unlock(&mutex[x][y]);
        sleep(1); // Espera um pouco antes de verificar novamente
    }

    return NULL;
}

// Fun��o da thread que gera inc�ndios periodicamente
void *gerar_incendio(void *param) {
    while (1) {
        int x = rand() % TAMANHO;
        int y = rand() % TAMANHO;

        pthread_mutex_lock(&mutex[x][y]);
        if (floresta[x][y] == SENSOR) {
            floresta[x][y] = FOGO;
            printf("Inc�ndio iniciado em [%d,%d]\n", x, y);
        }
        pthread_mutex_unlock(&mutex[x][y]);

        sleep(3); // A cada 3 segundos gera um inc�ndio
    }

    return NULL;
}

// Fun��o executada pela central de controle para combater inc�ndios
void *central_controle(void *param) {
    while (1) {
        pthread_mutex_lock(&mutex[0][0]); // Monitora sinais vindos da borda
        pthread_cond_wait(&cond[0][0], &mutex[0][0]);

        // Combate o inc�ndio nas c�lulas da borda
        for (int i = 0; i < TAMANHO; i++) {
            for (int j = 0; j < TAMANHO; j++) {
                if (floresta[i][j] == FOGO) {
                    combate_ao_fogo(i, j);
                }
            }
        }

        pthread_mutex_unlock(&mutex[0][0]);
    }

    return NULL;
}

// Fun��o para combater o fogo e mudar o estado da c�lula
void combate_ao_fogo(int x, int y) {
    pthread_mutex_lock(&mutex[x][y]);
    if (floresta[x][y] == FOGO) {
        floresta[x][y] = QUEIMADA;
        printf("Combate ao fogo em [%d,%d]\n", x, y);
    }
    pthread_mutex_unlock(&mutex[x][y]);
}
