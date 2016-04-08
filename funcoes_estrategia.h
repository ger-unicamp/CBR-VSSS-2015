#include "definicoes.h"
#include "funcoes_estrategia.c"

float distancia (float x1, float y1, float x2, float y2);

void busca(Robo robo, Bola bola);

void defesa_gol(Robo robo, Bola bola);

void espera();

float direcao (float x_robo, float y_robo, float x_rec, float y_rec);

void sendAllCommands(Cenario jogo);
