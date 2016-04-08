/*
 * Robo.cpp
 */

class Robo {
	double getPosicao;

	public:
		double getPosicao();
		void setPosicao(double posicao);
};

double Robo::getPosicao() {
	return posicao;
}

void Robo::setPosicao(double posicao) {
	this.posicao = posicao;
}
