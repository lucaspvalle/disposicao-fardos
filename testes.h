#pragma once
#include <random>
#include <vector>
#include <cmath>
#include <string>
using namespace std;

typedef struct {
	//armazenamento de dados da planilha
	int box, qtdade;
	float peso;
	string procedencia, tamanho;
} planilha;

class testes
{
public:

	double triangular(double a, double b, double c);
	vector<planilha> gerarInstancias(int nivel_fardos, int nivel_proc, double proporcao, int classes);
	void parametros();

};

