#define PI 3.14159265

/*delta: define a delta-vizinhanca de um robo: se a ordenada da bola estiver dentro da delta-vizinhanca da ordenada do robo, a bola e considerada na mesma direcao do robo*/
#define DELTA 5 //colocar um numero! coloquei um aleatorio so para poder compilar

/*delta direcao do robo: define a vizinhanca t.q. se a direcao da bola estiver nessa vizinhanca da direcao do robo, eles estao na mesma direcao*/
#define DELTA_DIR 5

/*limites da area, em ordem crescente*/
#define limitex1_area 0 //depende do lado do campo: 0 ou 135
#define limitex2_area 15 //depende do lado do campo: 15 ou 150
#define limitey1_area 30 //ja esta certo e eh fixo
#define limitey2_area 100 //ja esta certo e eh fixo

#define DELTA_GOLEIRO 5 //eh o delta para o angulo do goleiro (direcao)

#define DISTANCIA_BOLA_LIM 15 //Distancia proxima para comecar a desacelerar ao buscar a bola

#define DISTANCIA_POSSE_BOLA 3 //Distancia considerada posse de bola

#define CONSTANTE_GIRO1 0.7
#define CONSTANTE_GIRO2 175

/*posicao do gol*/
#define X_GOL 150.0
