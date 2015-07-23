/**
 *   Arquivo: campo.cpp
 * Descrição: Funções relativas à interpretação espacial do campo.
 **/

// Bibliotecas C++
#include <iostream>
#
// Bibliotecas C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// Bibliotecas Externas
// OpenCV
#include <opencv2/opencv.hpp>

// Funções

// Classes

// Namespaces
using namespace cv;
using namespace std;


void defineABCD (vector<Point> campo, Point* A, Point* B, Point* C, Point* D)
{
/* Pontos A, B, C, D:

A: Índice da menor soma x + y
D: Índice da maior soma x + y

	A - - B
	|     |
	|     |
	|     |
	|     |
	|     |
	C - - D

*/

	int somas[4], difA[4], difD[4];
	int i, menorsoma, maiorsoma=0, pontoA, pontoB, pontoC, pontoD;

	for (i=0; i < 4; i++)
	{
		somas[i] = campo[i].x + campo[i].y;
		if (maiorsoma < somas[i])
		{
			maiorsoma = somas[i];
			pontoD = i;
		}	
	}

	menorsoma = maiorsoma;

	for (i=0; i < 4; i++)
	{
		if (somas[i] < menorsoma)
		{
			menorsoma = somas[i];
			pontoA = i;
		}
	}

	for (i=0; i < 4; i++)
	{
		difA[i] = campo[i].y - campo[pontoA].y;
		difD[i] = campo[i].y - campo[pontoD].y;
	}

	for (i=0; i < 4; i++)
	{
		if(i != pontoA && i != pontoD)
		{
			pontoB = i;
			pontoC = i;
		}
	}

	for (i=0; i < 4; i++)
	{
		if(i != pontoA && i != pontoD)
		{
			if (abs(difA[i]) < abs(difA[pontoB]))
			{
				pontoB = i;
			}
			if (abs(difD[i]) < abs(difD[pontoC]))
			{
				pontoC = i;
			}
		}
	}

	*A = campo[pontoA];
	*B = campo[pontoB];
	*C = campo[pontoC];
	*D = campo[pontoD];
}

void defineCantos (Point aEsq, Point cEsq, Point bDir, Point dDir, Point* superiorEsq, Point* inferiorEsq, Point* superiorDir, Point* inferiorDir)
{
	// Define, a partir do conjunto de oito pontos de entrada (quatro pontos de cada pequena área),
	// quatro pontos que representam os limites externos do campo completo.
/*
	sE- - - - - - - - - - - - - - sD
	|							  |
	|							  |
	|							  |
	A - - B				    A - - B
	|     |					|     |
	|     |					|     |
	|     |					|     |
	|     |					|     |
	|     |					|     |
	C - - D					C - - D
	|							  |
	|							  |
	|							  |
	iE- - - - - - - - - - - - - - iD

*/
	Point xEsq = aEsq - cEsq;
	Point xDir = bDir - dDir;

	Point sE, iE, sD, iD;

	sE.x = cEsq.x + (xEsq.x * 100)/70;
	sE.y = cEsq.y + (xEsq.y * 100)/70;
	iE.x = cEsq.x - (xEsq.x * 30)/70;
	iE.y = cEsq.y - (xEsq.y * 30)/70;

	sD.x = dDir.x + (xDir.x * 100)/70;
	sD.y = dDir.y + (xDir.y * 100)/70;
	iD.x = dDir.x - (xDir.x * 30)/70;
	iD.y = dDir.y - (xDir.y * 30)/70;

	*superiorEsq = sE;
	*inferiorEsq = iE;
	*superiorDir = sD;
	*inferiorDir = iD;
}

Mat transformaCampo(Point superiorEsq, Point inferiorEsq, Point superiorDir, Point inferiorDir, Mat campo)
{
	vector<Point2f> corners;

	Point2f sE, iE, sD, iD;

	sE = superiorEsq;
	iE = inferiorEsq;
	sD = superiorDir;
	iD = inferiorDir;

	corners.push_back(sE);    // Criando os pontos do vetor corner
	corners.push_back(sD);
	corners.push_back(iD);
	corners.push_back(iE);	

	Mat quad = Mat::zeros(130*4, 150*4, CV_8UC3);  // Criando tamanho da imagem destino (que será depois retornada)

	vector<Point2f> quad_pts;
	quad_pts.push_back(Point2f(0, 0));
	quad_pts.push_back(Point2f(quad.cols, 0));
	quad_pts.push_back(Point2f(quad.cols, quad.rows));
	quad_pts.push_back(Point2f(0, quad.rows));

	Mat transmtx = getPerspectiveTransform(corners, quad_pts);  // Funcoes que fazem a transformação
	warpPerspective(campo, quad, transmtx, quad.size());

	return quad;
}

void defineCentro (Point superiorEsq, Point inferiorEsq, Point superiorDir, Point inferiorDir, Point* centro)
{
	// Define o ponto central a partir dos vértices do campo retangular.
	Point ctr;

	ctr.x = (superiorEsq.x + inferiorEsq.x + superiorDir.x + inferiorDir.x)/4;
	ctr.y = (superiorEsq.y + inferiorEsq.y + superiorDir.y + inferiorDir.y)/4;

	*centro = ctr;

}

int calculaAngulo(Point a, Point b)
{
	//Ângulo em graus
	double yDif, xDif, resultado;

	yDif = b.y - a.y;
	xDif = b.x - a.x;
	
	resultado = (atan2 (yDif,xDif) * 180 ) / PI;

	return (int) resultado;
}

