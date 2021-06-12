#pragma once
#include "ga.h"
using namespace std;

typedef struct {
	int classe, pequenos, grandes;
} instancia;

typedef struct {
	int fardos, classes, porcentagem;
} grupos;

typedef struct {
	int idx, pop, grc;
	double mut, fit_in, fit_out, tempo;
} relatorio;

class testes
{
public:
	vector<planilha> gerarInstancias(int fardos, int proporcao, int classes);
	vector<grupos> combinador();
	void resultados(vector<relatorio> saida);
};

