#pragma once
#include <algorithm> //std::shuffle, std::max_element
#include <random> //std::default_random_engine, std::rand
#include <iostream> //std::time
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
using namespace std;

typedef vector<vector<string>> string2d;

typedef struct {
	//armazenamento de dados da planilha
	int box, qtdade;
	float peso;
	string procedencia, tamanho;
} planilha;

typedef struct {
	int fardo, classe;
	string procedencia, tamanho;
} id;

typedef struct {
	int inf, sup;
} limites;

class ga { //classe do algoritmo genetico
public:

	int populacaoTam; //tamanho da populacao
	double mutacaoProb, cruzamentoProb = 0.75; //probabilidade de mutacao
	const int linhas = 4; //quantidade de linhas da matriz
	int matrizTam, colunas, classes;

	string2d populacao; //populacao de cromossomos
	vector<double> fitval; //valores fitness da populacao
	vector<planilha> inputFardos; //dados de inputs dos fardos

	ga(int pop, double probabilidade, vector<planilha> input) {
		populacaoTam = pop;
		mutacaoProb = probabilidade;
		inputFardos = input;
	}

	/*
	Funções de apoio
	*/

	void seed(unsigned int s);
	int categoria(string fardo);
	double faixas(int distancia);
	bool checarLimites(int corte, int chr, int bloco);
	limites gerarCorte(int range, int chr, int bloco);
	vector<string> popularFardos(vector<string> filho, vector<string> pequenos, vector<string> grandes, int corte);

	/*
	Funções do algoritmo genético
	*/

	void init();
	vector<double> fitness(int classes);
	int selecao();
	void cruzamento();
	void mutacao();
};