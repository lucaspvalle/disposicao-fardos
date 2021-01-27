#pragma once
#include <cmath>
#include "ga.h"
using namespace std;

typedef struct {
	int classe, pequenos, grandes;
} instancia;

typedef struct {
	int fardos, procedencia, classes;
	double porcentagem;
} grupos;

class testes
{
public:
	vector<planilha> gerarInstancias(int nivel_fardos, int nivel_proc, double proporcao, int classes);
	vector<grupos> combinador();
};

