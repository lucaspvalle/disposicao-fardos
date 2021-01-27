#pragma once
#include <algorithm> //std::shuffle, std::max_element
#include <random> //std::default_random_engine, std::rand
#include <iostream> //std::time
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
	int inf, sup;
} limites;

class ga { //classe do algoritmo genetico
public:

	int populacaoTam; //tamanho da populacao
	double mutacaoProb; //probabilidade de mutacao
	const int linhas = 4; //quantidade de linhas da matriz
	int matrizTam, colunas;

	string2d populacao; //populacao de cromossomos
	vector<double> fitval; //valores fitness da populacao
	vector<planilha> inputFardos; //dados de inputs dos fardos

	ga(int pop, double probabilidade, vector<planilha> input) {
		populacaoTam = pop;
		mutacaoProb = probabilidade;
		inputFardos = input;
	}

	/*
	Fun��es de apoio
	*/

	void seed(unsigned int s);
	int categoria(string fardo);
	bool checarLimites(int corte, int chr);
	bool checarBloco(int corte, int chr);
	limites gerarCorte(int range, int chr, string operador);
	vector<string> popularFardos(vector<string> filho, vector<string> mapa, int corte);

	/*
	Fun��es do algoritmo gen�tico
	*/

	void init();
	vector<double> fitness();
	int selecao();
	void cruzamento();
	void mutacao();
};