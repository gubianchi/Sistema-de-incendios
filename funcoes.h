
#ifndef FUNCOES_H
#define FUNCOES_H

#include <pthread.h>

#define TAMANHO 5
#define LIVRE '-'
#define SENSOR 'T'
#define FOGO '@'
#define QUEIMADA '/'

// Matriz representando a floresta
extern char floresta[TAMANHO][TAMANHO];

// Mutexes e vari�veis de condi��o para comunica��o entre os n�s
extern pthread_mutex_t mutex[TAMANHO][TAMANHO];
extern pthread_cond_t cond[TAMANHO][TAMANHO];

// Fun��es para inicializar e lidar com o sistema
void inicializar_floresta();
void *funcao_sensor(void *param);
void *gerar_incendio(void *param);
void *central_controle(void *param);
void combate_ao_fogo(int x, int y);

#endif
