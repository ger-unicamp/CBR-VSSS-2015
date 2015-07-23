// campo.cpp

// Funções relativas à interpretação espacial do campo.

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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









