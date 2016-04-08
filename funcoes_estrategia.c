#include "definicoes.h"
#include <cstring>

// Arduino Serial
#include "arduino-serial-lib.h"

float distancia(float x1, float y1, float x2, float y2)
{
	// cout << "distancia init" << endl;
	float resultado = sqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2)));
	// cout << "distancia end" << endl;
	return resultado;
}

float ajusta_angulo(float angulo){
	// cout << "ajusta_angulo init" << endl;
	if(angulo<0)
		angulo=angulo+360;
	
	if(angulo>360)
		angulo=angulo-360;

	// cout << "ajusta_angulo end" << endl;
	return angulo;
}
/*
void curvaSuave(Robo robo, int velocidade, bool sentido, float dif_dir){
	//dif_dir eh a diferenca entre a direcao do robo e a direcao objetivo
	//sentido eh 1 se for virar para a direita e 0 se for virar para a esquerda
	//Curva suave para a direita
	if (sentido == 1) {
		robo->directionLeft = 1;
		robo->speedLeft = 275;
		robo->speedRight = dif_dir * CONSTANTE_GIRO1-CONSTANTE_GIRO2;
		robo->directionRight = 1;
		if (robo->speedRight == 0) {
			robo->directionRight = 0;
		} else if (robo->speedRight < 0) {
			robo->directionRight = -1;
			robo->speedRight = abs(robo->speedRight);
		}
	} else {
		//Curva suave para a esquerda
		robo->directionRight = 1;
		robo->speedRight = 275;
		robo->speedRight = dif_dir * CONSTANTE_GIRO1 - CONSTANTE_GIRO2;
		robo->directionLeft = 1;
		if (robo->speedLeft == 0) {
			robo->directionLeft = 0;
		} else if (robo->speedLeft < 0) {
			robo->directionLeft = -1;
			robo->speedLeft = abs(robo->speedLeft);
		}
	}
}
*/
void alinhar_angulo (Robo robo, float dir_objetivo)
{
	// cout << "alinhar_angulo init" << endl;
	//o robo NAO esta na direcao objetivo
	// if((robo->dir < ajusta_angulo(robo->dir-DELTA_GOLEIRO))||(robo->dir > ajusta_angulo(robo->dir + DELTA_GOLEIRO)))
	// {	
		int dif_dir = abs(robo->dir - dir_objetivo);
		if(dif_dir <= 180)
		{
			//giro horario
			//virar para a direita
			robo->directionLeft=1;
			robo->speedLeft=275;
			robo->directionRight=-1;
			robo->speedRight=275;
			// curvaSuave(robo, 275, 1, dif_dir);
		} else {
			//giro anti-horario
			//virar para a esquerda
			robo->directionLeft=-1;
			robo->speedLeft=275;
			robo->directionRight=1;
			robo->speedRight=275;
			// curvaSuave(robo, 275, 0, dif_dir);

		}
	// }
	// cout << "alinhar_angulo end" << endl;
}

float direcao (float x_robo, float y_robo, float x_rec, float y_rec)
{
	// cout << "direcao init" << endl;
	float x = x_rec-x_robo;
	float y = y_rec-y_robo;
	float resultado = atan2(y,x)*180/PI;
	if (x<0) resultado = resultado+180;
	// cout << "direcao end" << endl;
	return resultado;
}

/*void acerta_longitude ()
{
	return;
}
*/

void seguir_bola (Robo robo, Bola bola)
{
	// cout << "seguir_bola init" << endl;
	if (bola->y == robo->y) {
		/*para motor*/
		robo->directionLeft=0;
		robo->speedLeft=0;
		robo->directionRight=0;
		robo->speedRight=0;
	} else if (bola->y > robo->y) {
		/*vai para um lado*/
		robo->directionLeft=1;
		robo->speedLeft=275;
		robo->directionRight=1;
		robo->speedRight=275;
	} else if (bola->y < robo->y) {
		/*vai para o outro lado*/
		robo->directionLeft=-1;
		robo->speedLeft=275;
		robo->directionRight=-1;
		robo->speedRight=275;
	}
	// cout << "seguir_bola end" << endl;
}

void defesa_gol(Robo robo, Bola bola)
{
	// cout << "defesa_gol init" << endl;
	//o robo esta no x certo?
	if((robo->x > 7.0f)&&(robo->x < 8.0f))
	{
		//o robo esta na direcao certa para executar sua funcao (270 graus)?
		if((robo->dir > 270-DELTA_GOLEIRO)&&(robo->dir < 270+DELTA_GOLEIRO))
		{
			//o robo esta dentro da area?
			if((robo->y > limitey1_area)&&(robo->y < limitey2_area))
			{
				//segue a bola (no eixo y)
				seguir_bola(robo, bola);
			}
			else //nao esta dentro da area
			{
				if(robo->y < limitey1_area)
				{
					//vai para frente
					robo->directionLeft=1;
					robo->speedLeft=275;
					robo->directionRight=1;
					robo->speedRight=275;

				}
				if(robo->y > limitey2_area)
				{
					//vai para tras
					robo->directionLeft=-1;
					robo->speedLeft=275;
					robo->directionRight=-1;
					robo->speedRight=275;


				}
			}
		}
		else //nao esta na direcao para executar sua funcao
		{
			//gira ate achar
			alinhar_angulo(robo, 270);
		}
	}
	else //nao esta no x certo
	{
		//o robo esta fora da direcao 0 graus (+/- um erro)?
		alinhar_angulo(robo, 0);
		//o robo esta na direcao 0 graus
		if(robo->x < 7.0f)
		{
			//vai pra frente
			robo->directionLeft=1;
			robo->speedLeft=275;
			robo->directionRight=1;
			robo->speedRight=275;
		}
		if(robo->x > 8.0f)
		{
			//vai para tras
			robo->directionLeft=-1;
			robo->speedLeft=275;
			robo->directionRight=-1;
			robo->speedRight=275;

		}
	}
	// cout << "defesa_gol end" << endl;
}
/*
void espera()
{
	//acertar a longitude
	//seguir a bola, dentro dos limites do campo
	return;
}
*/

void busca_gol(Robo robo)
{
	// cout << "busca_gol init" << endl;
	// float direcao_gol1 = direcao(robo->x, robo->y, X_GOL, 50.0f);
	// float direcao_gol2 = direcao(robo->x, robo->y, X_GOL, 80.0f);
	float direcao_gol_med = direcao(robo->x, robo->y, X_GOL, 65.0f);
	//float direcao_gol_max = fmax(direcao_gol1, direcao_gol2);
	//float direcao_gol_min = fmin(direcao_gol1, direcao_gol2);
	direcao_gol_med = ajusta_angulo(direcao_gol_med);

	//se esta na direcao do gol
	if ((direcao_gol_med >= 5 && direcao_gol_med <= 355 && // Intervalo segura de 5 a 355
		 robo->dir > direcao_gol_med - DELTA_DIR && robo->dir < direcao_gol_med + DELTA_DIR) ||
		((direcao_gol_med > 355 || direcao_gol_med < 5) &&  // Intervalo perigoso de 0 a 5 e de 355 a 360
			   (robo->dir > 355 || robo->dir < 5))) {
	/*vai pra frente*/
		// if((robo->dir < direcao_gol1)||(robo->dir > direcao_gol2)){
		robo->speedLeft=275;
		robo->directionLeft=1;
		robo->speedRight=275;
		robo->directionRight=1;	
		// } 
	//se nao estao na direcao do gol
	} else {

	}
	// cout << "busca_gol end" << endl;
}

/*
void busca_bola(Robo robo, Bola bola)
{
	//se o robo esta na direcao da bola				
	if((robo->dir_bola > robo->dir - DELTA_DIR)&&(robo->dir_bola < robo->dir + DELTA_DIR)){
	//vai para frente

	//se estiver muito perto da bola, reduzir velocidade
		if(distancia(robo->x, robo->y, bola->x, bola->y)<DISTANCIA_BOLA_LIM){
			robo->speedLeft=500;
			robo->directionLeft=1;
			robo->speedRight=500;
			robo->directionRight=1;
		}
		//se não, vai com tudo
		else{
			robo->speedLeft=700;
			robo->directionLeft=1;
			robo->speedRight=700;
			robo->directionRight=1;
		}
	}
	//o robo nao esta na direcao da bola
	//busca ficar na direcao da bola;
	else{

		alinhar_angulo(robo, robo->dir_bola);
	}
	return;
}
*/

void busca(Robo robo, Bola bola)
{
	// cout << "busca init" << endl;
	robo->dir_bola = direcao(robo->x, robo->y, bola->x, bola->y);
	if (bola->x > robo->x) {
		if ((robo->dir_bola >= 5 && robo->dir_bola <= 355 && // Intervalo segura de 5 a 355
			 robo->dir > robo->dir_bola - DELTA_DIR && robo->dir < robo->dir_bola + DELTA_DIR) ||
			((robo->dir_bola > 355 || robo->dir_bola < 5) &&  // Intervalo perigoso de 0 a 5 e de 355 a 360
			 (robo->dir > 355 || robo->dir < 5))) {
			robo->posse = TRUE;
			// busca_gol(robo);
			robo->speedLeft=275;
			robo->directionLeft=1;
			robo->speedRight=275;
			robo->directionRight=1;	
		} else {
			// Alinhar com a bola para ir atrás dela
			alinhar_angulo(robo, robo->dir_bola);
		}
	} else {
		// Fazer nada
		robo->posse = FALSE;
		// busca_bola(robo, bola);
	}
	// cout << "busca end" << endl;
}

void sendCommands(Robo robo)
{
	if ((robo->ID < 1 || robo->ID > 3) || 
		(robo->directionRight < -1 || robo->directionRight > 1) || (robo->directionLeft < -1 || robo->directionLeft > 1) ||
		(robo->speedRight < 0 || robo->speedRight > 1000) || (robo->speedLeft < 0 || robo->speedLeft > 1000)) {
		return;
	}
	// cout << "sendCommands init" << endl;
	char serialPort[] = "/dev/tty.usbmodem1411";
	// int robot;
	char directionRight[3], speedRight[4], directionLeft[3], speedLeft[4];
	// robot = robo->ID;
	// directionRight = robo->directionRight;
	// directionLeft = robo->directionLeft;
	// speedRight = robo->speedRight;
	// speedLeft = robo->speedLeft;
	// cout << robo->directionRight << " ; " << robo->directionLeft << " ; " << robo->speedRight << " ; " << robo->speedLeft << endl;
	sprintf(directionRight, "%d", (int) robo->directionRight);
	sprintf(directionLeft, "%d", (int) robo->directionLeft);
	sprintf(speedRight, "%d", (int) robo->speedRight);
	sprintf(speedLeft, "%d", (int) robo->speedLeft);

	const int buf_max = 256;
	char buf[buf_max];

	int fd = serialport_init(serialPort, 9600);
	if (fd == -1) cout << "couldn't open port" << endl;

	char data[18] = {'S', (char) (robo->ID + 48)};
	// strcpy(data, "S");
	// strcat(data, (char) (robo->ID + 48));
	strcat(data, "#");
	strcat(data, directionRight);
	strcat(data, "#");
	strcat(data, speedRight);
	strcat(data, "#");
	strcat(data, directionLeft);
	strcat(data, "#");
	strcat(data, speedLeft);
	strcat(data, "E\0");

	// char data[] = {'S', (char)(robot + 48), '#', directionRight, '#', speedRight, '#', directionLeft, '#', speedLeft, 'E'};
	cout << data << endl;
	sprintf(buf, "%s", data);
	int rc = serialport_write(fd, buf);
	if (rc == -1) cout << "error writing" << endl;

	/*
    char data[] = {'S', robot, '#', directionRight, '#', speedRight, '#', directionLeft, '#', speedLeft, 'E'};
    cout << data << endl;
    FILE *file;
    file = fopen(serialPort, "w"); // Opening usb device file. 
    int i = 0;
    for (i = 0 ; i < 13 ; i++) {
        fprintf(file, "%c", data[i]); // Writing to the file.
        //printf("%c\n", (char) data[i]);
    }
    fclose(file);
    */
    // cout << "sendCommands end" << endl;
}
/*
void sendAllCommands(Cenario jogo){
	sendCommands(jogo->robo1);
	sendCommands(jogo->robo2);
	sendCommands(jogo->robo3);
}
*/