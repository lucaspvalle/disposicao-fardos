#pragma once
#include <cmath>
#include <fstream>
#include <chrono>
#include "ga.h"
using namespace std;

typedef struct {
	int classe, pequenos, grandes;
} instancia;

class testes
{
public:
	vector<planilha> gerarInstancias(int nivel_fardos, int nivel_proc, double proporcao, int classes);
	void principal(int populacaoTam, int geracaoTam, double mutacaoProb, unsigned int semente);
	void parametros();
};

