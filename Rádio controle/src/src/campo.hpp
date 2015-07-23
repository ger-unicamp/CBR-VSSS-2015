// campo.hpp

// Funções relativas à interpretação espacial do campo.

#include "campo.cpp"

void defineABCD (vector<Point> campo, Point* A, Point* B, Point* C, Point* D);
void defineCantos (Point aEsq, Point cEsq, Point bDir, Point dDir, Point* superiorEsq, Point* inferiorEsq, Point* superiorDir, Point* inferiorDir);
