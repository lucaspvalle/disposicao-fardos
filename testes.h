#pragma once
#include <cmath>
#include <chrono>
#include <fstream>
#include "ga.h"
using namespace std;

class testes
{
public:

	double triangular(double a, double b, double c);
	vector<planilha> gerarInstancias(int nivel_fardos, int nivel_proc, double proporcao, int classes);
	void principal(int populacaoTam, int geracaoTam, double mutacaoProb);
	void parametros();
};

