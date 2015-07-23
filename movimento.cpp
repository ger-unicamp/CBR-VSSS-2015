// movimento.cpp

// Funções relativas às movimentações tomadas pelos robôs.

// Bibliotecas C++
// Bibliotecas C
// Bibliotecas Externas
// OpenCV
// Funções
// Classes

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "campo.hpp"

#define fatorMultiplicacao 10;

using namespace cv;
using namespace std;


// Recebe a distancia em pixels do robo ao objetivo, deois endereços de inteiros para sentido das rodas e dois endereços apara os potenciais.
void andaFrente(Point origem, Point destino, int* sentidoEsquerda, int* sentidoDireita, int* potenciaEsquerda, int* potenciaDireita)
{
	double distancia;
	int potencia, sent;

	distancia = abs( sqrt( pow((destino.x - origem.x), 2) + pow((destino.y - origem.y), 2)) );

	if (distancia > 50 * fatorMultiplicacao)
	{
		potencia = 700;
		sent = 1;
	}
	else if (2 * fatorMultiplicacao < distancia < 50 * fatorMultiplicacao)
	{
		potencia = (int) (((distancia - 1)*500)/49 + 200);
		sent = 1
	}
	else
	{
		potencia = 0;
		sent = 0;
	}

	*sentidoEsquerda = sent;
	*sentidoDireita = sent;
	*potenciaEsquerda = potencia;
	*potenciaDireita = potencia
}


// Recebe o Ponto do centro do objeto, recebe o ponto ad sua frente e o destino, calcula o angulo entre os vetores e seta os atributos.
void giraEixo(Point origem, Point frente, Point destino, int* sentidoEsquerda, int* sentidoDireita, int* potenciaEsquerda, int* potenciaDireita)
{
	int anguloVetor, anguloDestino;

	anguloVetor = calculaAngulo(origem, frente);
	anguloDestino = calculaAngulo(origem, destino);

	if ( 10 < (anguloVetor - anguloDestino) =<  180 )
	{
		*sentidoEsquerda = 1;
		*sentidoDireita = -1;

 		*potenciaEsquerda = 400;
		*potenciaDireita = 400;
	}
	else if ( 180 < (anguloVetor - anguloDestino) <  350 )
	{
		*sentidoEsquerda = -1;
		*sentidoDireita = 1;

 		*potenciaEsquerda = 400;
		*potenciaDireita = 400;
	}
	else
	{
		*sentidoEsquerda = 0;
		*sentidoDireita = 0;

 		*potenciaEsquerda = 0;
		*potenciaDireita = 0;
	}	
}
