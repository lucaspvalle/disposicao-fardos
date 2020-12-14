#pragma once
#include <algorithm> //std::shuffle, std::max_element
#include <random> //std::default_random_engine, std::rand
#include <iostream> //std::cout, std::time
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

class ga { //classe do algoritmo genetico

public:

	int populacaoTam; //tamanho da populacao
	double mutacaoProb; //probabilidade de mutacao
	const int linhas = 4; //quantidade de linhas da matriz
	int matrizTam;

	string2d populacao; //populacao de cromossomos
	vector<int> fitval; //valores fitness da populacao
	vector<planilha> inputFardos; //dados de inputs dos fardos

	ga(int pop, double probabilidade, vector<planilha> input) {

		populacaoTam = pop;
		mutacaoProb = probabilidade;
		inputFardos = input;

	}

	/*
	Funções de apoio
	*/
	int categoria(string fardo);
	vector<string> popularFardos(vector<string> filho, vector<string> mapa);

	/*
	Funções do algoritmo genético
	*/

	string2d init();
	vector<int> fitness();
	int selecao();
	string2d cruzamento();
	string2d mutacao();

};