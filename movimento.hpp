// movimento.hpp

// Funções relativas às movimentações tomadas pelos robôs.

#include "movimento.cpp"

// Recebe a distancia em pixels do robo ao objetivo, deois endereços de inteiros para sentido das rodas e dois endereços apara os potenciais.
void andaFrente(Point origem, Point destino, int* sentidoEsquerda, int* sentidoDireita, int* potenciaEsquerda, int* potenciaDireita);

// Recebe o Ponto do centro do objeto, recebe o ponto ad sua frente e o destino, calcula o angulo entre os vetores e seta os atributos.
void giraEixo(Point origem, Point frente, Point destino, int* sentidoEsquerda, int* sentidoDireita, int* potenciaEsquerda, int* potenciaDireita);
