/**
 *   Arquivo: utils.cpp
 * Descrição: Funções de utilidade geral
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

// Namespaces
using namespace std;
using namespace cv;

/**
 *     Função: split
 * Parâmetros: string str - Texto que será dividido
 *             string sep - Caracter que será como separador
 *    Retorno: vector<string> pMedio - Vetor das strings resultantes da divisão feita pelo separador
 *  Descrição: Utilizando um caracter, quando este é encontrado na string, divide esta naquele ponto. O caracter não fica em nenhuma das strings resultantes.
 *    Exemplo: split("Laranja, maçã, banana", ",")
 *             Saída: ["Laranja", "maçã", "banana"]
 **/
vector<string> split(string str, string sep) {
    char* cstr = const_cast<char*>(str.c_str());
    char* current;
    vector<string> arr;
    current = strtok(cstr,sep.c_str());
    while (current != NULL) {
        arr.push_back(current);
        current = strtok(NULL, sep.c_str());
    }
    return arr;
}

/**
 *     Função: calculaPontoMedio
 * Parâmetros: vector<Point> pontos - Vetor de pontos
 *    Retorno: Point pMedio - Ponto médio de todos os pontos.
 *  Descrição: Calcula o ponto médio baseado em todos os pontos passados para a função.
 *    Exemplo: calculaPontoMedio([10,13], [1,2], [4,6])
 *             Saída: [5,7]
 **/
Point calculaPontoMedio(vector<Point> pontos) {
	Point pMedio;
	pMedio.x = 0;
	pMedio.y = 0;
	for (int i = 0; i < pontos.size(); i++) {
		pMedio += pontos[i];
	}
	pMedio.x = pMedio.x / pontos.size();
	pMedio.y = pMedio.y / pontos.size();
	return pMedio;
}