/**
 *   Arquivo: inits.cpp
 * Descrição: Funções de inicialização
 **/

// Bibliotecas C++
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Bibliotecas C
#include <stdio.h>

// Bibliotecas Externas

// Funções

// Classes

// Parâmetros BOLA
int saveBallParametersOfTrackbar = FALSE;
int areaBallL = 0;
int areaBallH = 0;
int hBallL = 0;
int hBallH = 0;
int sBallL = 0;
int sBallH = 0;
int vBallL = 0;
int vBallH = 0;

// Parâmetros AREA
int saveAreaParametersOfTrackbar = FALSE;
int areaAreaL = 0;
int areaAreaH = 0;

// Parâmetros BLUE
int saveBlueParametersOfTrackbar = FALSE; // Save BLUE parameters of trackbar
int rBlueL = 0;
int rBlueH = 0;
int gBlueL = 0;
int gBlueH = 0;
int bBlueL = 0;
int bBlueH = 0;
int hBlueL = 0;
int hBlueH = 0;
int sBlueL = 0;
int sBlueH = 0;
int vBlueL = 0;
int vBlueH = 0;

// Parâmetros YELLOW
int saveYellowParametersOfTrackbar = FALSE; // Save YELLOW parameters of trackbar
int hYellowL = 0;
int hYellowH = 0;
int sYellowL = 0;
int sYellowH = 0;
int vYellowL = 0;
int vYellowH = 0;
int rYellowL = 0;
int rYellowH = 0;
int gYellowL = 0;
int gYellowH = 0;
int bYellowL = 0;
int bYellowH = 0;

// Parâmetros PURPLE
int savePurpleParametersOfTrackbar = FALSE; // Save PURPLE parameters of trackbar
int hPurpleL = 0;
int hPurpleH = 0;
int sPurpleL = 0;
int sPurpleH = 0;
int vPurpleL = 0;
int vPurpleH = 0;

// Parametros GREEN
int saveGreenParametersOfTrackbar = FALSE; // Save GREEN parameters of trackbar
int hGreenL = 0;
int hGreenH = 0;
int sGreenL = 0;
int sGreenH = 0;
int vGreenL = 0;
int vGreenH = 0;

Point posJogador1[2];
Point posJogador2[2];
Point posJogador3[2];
Point posBola;

// Parâmetros jogadores geral
int saveJogadoresParametersOfTrackbar = FALSE; // Save JOGADORES parameters of trackbar
int areaJogadorL = 0;
int areaJogadorH = 0;
int areaId1L = 0;
int areaId1H = 0;
int areaId2L = 0;
int areaId2H = 0;
int areaId3L = 0;
int areaId3H = 0;

bool encontrouJogador1 = false;
bool encontrouJogador2 = false;
bool encontrouJogador3 = false;
bool encontrouBola = false;

int grauFrenteJogador1 = 0;
int grauFrenteJogador2 = 0;
int grauFrenteJogador3 = 0;
// pontos apenas para verificar a inclinação do robô
Point posJogador1Aux[2];
Point posJogador2Aux[2];
Point posJogador3Aux[2];

bool openConnection = false;
int fd;

/**
 *     Função: initBallParameters
 * Parâmetros: -
 *    Retorno: -
 *  Descrição: Inicializa os parâmetros da Bola baseado no arquivo "ball-parameters.txt".
 *    Exemplo: -
 **/
void initBallParameters() {
	string line;
	ifstream ballParametersFile ("parameters/ball-parameters.txt");
	if (ballParametersFile.is_open()) {
		while (getline(ballParametersFile,line)) {
		    vector<string> strSplit; 
			strSplit = split(line, "="); 
			if (strSplit[0] == "areaL") {
				areaBallL = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaH") {
				areaBallH = stoi(strSplit[1]);
			} else if (strSplit[0] == "hL") {
				hBallL = stoi(strSplit[1]);
			} else if (strSplit[0] == "hH") {
				hBallH = stoi(strSplit[1]);
			} else if (strSplit[0] == "sL") {
				sBallL = stoi(strSplit[1]);
			} else if (strSplit[0] == "sH") {
				sBallH = stoi(strSplit[1]);
			} else if (strSplit[0] == "vL") {
				vBallL = stoi(strSplit[1]);
			} else if (strSplit[0] == "vH") {
				vBallH = stoi(strSplit[1]);
			} else {
				cout << "[WARNING] Parameter " << strSplit[0] << ", from BALL file, not found." << endl;
			}
	    }
	    ballParametersFile.close();
	} else {
		cout << "[ERROR] Unable to open file BALL parameters." << endl; 
	}
}

/**
 *     Função: initAreaParameters
 * Parâmetros: -
 *    Retorno: -
 *  Descrição: Inicializa os parâmetros da Área do Campo baseado no arquivo "area-parameters.txt".
 *    Exemplo: -
 **/
void initAreaParameters() {
	string line;
	ifstream areaParametersFile ("parameters/area-parameters.txt");
	if (areaParametersFile.is_open()) {
		while (getline(areaParametersFile,line)) {
		    vector<string> strSplit; 
			strSplit = split(line, "="); 
			if (strSplit[0] == "areaL") {
				areaAreaL = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaH") {
				areaAreaH = stoi(strSplit[1]);
			} else {
				cout << "[WARNING] Parameter " << strSplit[0] << ", from AREA file, not found." << endl;
			}
	    }
	    areaParametersFile.close();
	} else {
		cout << "[ERROR] Unable to open file AREA parameters." << endl; 
	}
}

/**
 *     Função: initJogadoresParameters
 * Parâmetros: -
 *    Retorno: -
 *  Descrição: Inicializa os parâmetros dos jogadores baseado no arquivo "jogadores-parameters.txt".
 *    Exemplo: -
 **/
void initJogadoresParameters() {
	string line;
	ifstream jogadoresParametersFile ("parameters/jogadores-parameters.txt");
	if (jogadoresParametersFile.is_open()) {
		while (getline(jogadoresParametersFile, line)) {
		    vector<string> strSplit; 
			strSplit = split(line, "="); 

			if (strSplit[0] == "areaJogadorL") {
				areaJogadorL = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaJogadorH") {
				areaJogadorH = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaId1L") {
				areaId1L = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaId1H") {
				areaId1H = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaId2L") {
				areaId2L = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaId2H") {
				areaId2H = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaId3L") {
				areaId3L = stoi(strSplit[1]);
			} else if (strSplit[0] == "areaId3H") {
				areaId3H = stoi(strSplit[1]);
			} else {
				cout << "[WARNING] Parameter " << strSplit[0] << ", from JOGADORES file, not found." << endl;
			}
	    }
	    jogadoresParametersFile.close();
	} else {
		cout << "[ERROR] Unable to open file JOGADORES parameters." << endl; 
	}
}

/**
 *     Função: initBlueColorParameters
 * Parâmetros: -
 *    Retorno: -
 *  Descrição: Inicializa os parâmetros da Cor Azul baseado no arquivo "blue-color-parameters.txt".
 *    Exemplo: -
 **/
void initBlueColorParameters() {
	string line;
	ifstream blueParametersFile ("parameters/blue-color-parameters.txt");
	if (blueParametersFile.is_open()) {
		while (getline(blueParametersFile,line)) {
		    vector<string> strSplit; 
			strSplit = split(line, "="); 
			if (strSplit[0] == "hL") {
				hBlueL = stoi(strSplit[1]);
			} else if (strSplit[0] == "hH") {
				hBlueH = stoi(strSplit[1]);
			} else if (strSplit[0] == "sL") {
				sBlueL = stoi(strSplit[1]);
			} else if (strSplit[0] == "sH") {
				sBlueH = stoi(strSplit[1]);
			} else if (strSplit[0] == "vL") {
				vBlueL = stoi(strSplit[1]);
			} else if (strSplit[0] == "vH") {
				vBlueH = stoi(strSplit[1]);
			} else if (strSplit[0] == "rL") {
				rBlueL = stoi(strSplit[1]);
			} else if (strSplit[0] == "rH") {
				rBlueH = stoi(strSplit[1]);
			} else if (strSplit[0] == "gL") {
				gBlueL = stoi(strSplit[1]);
			} else if (strSplit[0] == "gH") {
				gBlueH = stoi(strSplit[1]);
			} else if (strSplit[0] == "bL") {
				bBlueL = stoi(strSplit[1]);
			} else if (strSplit[0] == "bH") {
				bBlueH = stoi(strSplit[1]);
			} else {
				cout << "[WARNING] Parameter " << strSplit[0] << ", from BLUE COLOR file, not found." << endl;
			}
	    }
	    blueParametersFile.close();
	} else {
		cout << "[ERROR] Unable to open file BLUE COLOR parameters." << endl; 
	}
}

/**
 *     Função: initYellowColorParameters
 * Parâmetros: -
 *    Retorno: -
 *  Descrição: Inicializa os parâmetros da Cor Amarela baseado no arquivo "yellow-color-parameters.txt".
 *    Exemplo: -
 **/
void initYellowColorParameters() {
	string line;
	ifstream yellowParametersFile ("parameters/yellow-color-parameters.txt");
	if (yellowParametersFile.is_open()) {
		while (getline(yellowParametersFile,line)) {
		    vector<string> strSplit; 
			strSplit = split(line, "="); 
			if (strSplit[0] == "hL") {
				hYellowL = stoi(strSplit[1]);
			} else if (strSplit[0] == "hH") {
				hYellowH = stoi(strSplit[1]);
			} else if (strSplit[0] == "sL") {
				sYellowL = stoi(strSplit[1]);
			} else if (strSplit[0] == "sH") {
				sYellowH = stoi(strSplit[1]);
			} else if (strSplit[0] == "vL") {
				vYellowL = stoi(strSplit[1]);
			} else if (strSplit[0] == "vH") {
				vYellowH = stoi(strSplit[1]);
			} else if (strSplit[0] == "rL") {
				rYellowL = stoi(strSplit[1]);
			} else if (strSplit[0] == "rH") {
				rYellowH = stoi(strSplit[1]);
			} else if (strSplit[0] == "gL") {
				gYellowL = stoi(strSplit[1]);
			} else if (strSplit[0] == "gH") {
				gYellowH = stoi(strSplit[1]);
			} else if (strSplit[0] == "bL") {
				bYellowL = stoi(strSplit[1]);
			} else if (strSplit[0] == "bH") {
				bYellowH = stoi(strSplit[1]);
			} else {
				cout << "[WARNING] Parameter " << strSplit[0] << ", from YELLOW COLOR file, not found." << endl;
			}
	    }
	    yellowParametersFile.close();
	} else {
		cout << "[ERROR] Unable to open file YELLOW COLOR parameters." << endl; 
	}
}

/**
 *     Função: initParameters
 * Parâmetros: -
 *    Retorno: -
 *  Descrição: Inicializa todos os parâmetros. Chama todas as funções de inicialização desse arquivo.
 *    Exemplo: -
 **/
void initParameters() {
	initBallParameters();
	initAreaParameters();
	initJogadoresParameters();
	initBlueColorParameters();
	initYellowColorParameters();
}
