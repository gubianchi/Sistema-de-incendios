
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "funcoes.h"

int main() {
    srand(time(NULL));
    inicializar_floresta();

    // Cria threads para cada nó sensor
    pthread_t sensores[TAMANHO][TAMANHO];
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            int *pos = malloc(2 * sizeof(int));
            pos[0] = i;
            pos[1] = j;
            pthread_create(&sensores[i][j], NULL, funcao_sensor, pos);
        }
    }

    // Cria thread para gerar incêndios
    pthread_t gerador_incendios;
    pthread_create(&gerador_incendios, NULL, gerar_incendio, NULL);

    // Cria a thread central
    pthread_t central;
    pthread_create(&central, NULL, central_controle, NULL);

    // Junta as threads
    pthread_join(gerador_incendios, NULL);
    pthread_join(central, NULL);

    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_join(sensores[i][j], NULL);
        }
    }

    return 0;
}
