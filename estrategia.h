#include "definicoes.h"

typedef struct robot
{
	int ID;
	//Coordenadas do robo e direcao
	float x;
	float y;
	float dir;
	float dir_bola;
	float distancia_bola;

	//Velocidades dos motores
	int speedLeft;
	int speedRight;
	int directionRight;
	int directionLeft;

	//1 quando tiver a posse e 0 quando não tiver a posse
	bool posse;

	char modo;

	//int movimento;
}Robot, *Robo;

typedef struct pelota{
	float x;
	float y;
	float dir_r1;
	float dir_r2;
	float dir_r3;
}Ball, *Bola;

typedef struct Jogo{
	Robo robo1;
	Robo robo2;
	Robo robo3;

	Bola bola;


}cenario, *Cenario;

void play(float x_R1, float y_R1, float dir_R1,
		  float x_R2, float y_R2, float dir_R2,
		  float x_R3, float y_R3, float dir_R3,
		  float x_bola, float y_bola,
		  Point origemCampo, int tamanhoCampoLargura, int tamanhoCampoAltura);

#include "estrategia.c"