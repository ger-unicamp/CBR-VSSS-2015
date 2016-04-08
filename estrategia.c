#include "definicoes.h"
#include "funcoes_estrategia.h"

void play(float x_R1, float y_R1, float dir_R1,
		  float x_R2, float y_R2, float dir_R2,
		  float x_R3, float y_R3, float dir_R3,
		  float x_bola, float y_bola,
		  Point origemCampo, int tamanhoCampoLargura, int tamanhoCampoAltura)
{
	// cout << "play init" << endl;
	//declaracao do status dos jogadores (disponivel = 1 = dentro do campo)
//	short int status_R1=0, status_R2=0, status_R3=0;

	// 150 cm e 130 cm são a largura e altura do campo real, respectivamente.
	float pixelPorCm = ((tamanhoCampoLargura / 150) + (tamanhoCampoLargura / 130)) / 2;

	// Converte de pixel para centímetros os parâmetros recebidos.
	x_R1 = (x_R1 - origemCampo.x) / pixelPorCm;
	x_R2 = (x_R2 - origemCampo.x) / pixelPorCm;
	x_R3 = (x_R3 - origemCampo.x) / pixelPorCm;
	y_R1 = (y_R1 - origemCampo.y) / pixelPorCm;
	y_R2 = (y_R2 - origemCampo.y) / pixelPorCm;
	y_R3 = (y_R3 - origemCampo.y) / pixelPorCm;
	x_bola = (x_bola - origemCampo.x) / pixelPorCm;
	y_bola = (y_bola - origemCampo.y) / pixelPorCm;

	Robo robo1 = (Robot*)malloc(sizeof(Robot));
	Robo robo2 = (Robot*)malloc(sizeof(Robot));
	Robo robo3 = (Robot*)malloc(sizeof(Robot));
	Cenario jogo = (cenario*)malloc(sizeof(cenario));
	Bola bola = (Ball*)malloc(sizeof(Ball));

	robo1->ID = 1;
	robo1->x=x_R1;
	robo1->y=y_R1;
	robo1->dir=dir_R1;
	robo1->posse=0;

	robo2->ID = 2;
	robo2->x=x_R2;
	robo2->y=y_R2;
	robo2->dir=dir_R2;
	robo2->posse=0;

	robo3->ID = 3;
	robo3->x=x_R3;
	robo3->y=y_R3;
	robo3->dir=dir_R3;
	robo3->posse=0;

	bola->x=x_bola;
	bola->y=y_bola;

	jogo->robo1=robo1;
	jogo->robo2=robo2;
	jogo->robo3=robo3;
	jogo->bola=bola;

	//verifica se eles estao dentro do campo
	
	/*
	if(((robo1->x>=0)&&(robo1->x<=MAX_x))&&((robo1->y>=0)&&(robo1.y<=MAX_y))) status_R1 = 1; printf("R1 detectado\n");
	if(((robo2->x>=0)&&(robo2->x<=MAX_x))&&((robo2->y>=0)&&(robo2.y<=MAX_y))) status_R2 = 1; printf("R2 detectado\n");
	if(((robo3->x>=0)&&(robo3->x<=MAX_x))&&((robo3y>=0)&&(robo3.y<=MAX_y))) status_R3 = 1; printf("R3 detectado\n");
	*/
	
	//se os tres robos estiverem dentro do campo, um deles vai se tornar o goleiro (R3)
		//e se os tres nao estiverem?? pensar...
	
	/*if((status_R1==1)&&(status_R2==1)&&(status_R3==1)) ; printf("R3 modo goleiro (G)\n");
	*/
	
	// cout << "distancia" << endl;
	//calcula as distancias dos jogadores 1 e 2 (jogadores de linha) ate a bola
	robo1->distancia_bola = distancia(robo1->x, robo1->y, bola->x, bola->y);
	robo2->distancia_bola = distancia(robo2->x, robo2->y, bola->x, bola->y);
	
	//POR ENQUANTO NAO VAMOS UTILIZAR O MODO ESPERA

	//se os dois estiverem proximos (dist<=LIMITE_DIST), os dois vao em cima da bola
	/*if((robo1.distancia_bola<=LIMITE_DIST)&&(robo2.distancia_bola<=LIMITE_DIST))
	{
		robo1.modo='B'; printf("R1 modo busca bola (B)\n");
		robo2.modo='B'; printf("R2 modo busca bola (B)\n");
	}
	//se R1 esta mais proximo, ele busca bola e R2 espera
	else if (dist1 < dist2)
	{
		robo1.modo ='B'; printf("R1 modo busca bola (B)\n");
		robo2.modo ='E'; printf("R2 modo espera (E)\n");
	}
	//se R2 esta mais proximo, ele busca bola e R1 espera
	else
	{
		robo1.modo='E'; printf("R1 modo espera (E)\n");
		robo2.modo='B'; printf("R2 modo busca bola (B)\n");
	}*/
	
	// cout << "busca" << endl;

	//agora que ja sabemos qual funcao cada jogador assumiu, vamos atribuir os movimentos a cada uma dessas funcoes
	// busca(robo1, bola);
	sendCommands(jogo->robo1);
	//else {}//espera();
		
	// busca(robo2, bola);
	sendCommands(jogo->robo2);
	//else {}//espera();
		
	// busca(robo3, bola);
	sendCommands(jogo->robo3);

	// sendAllCommands(jogo);
	// cout << "play end" << endl;
}
