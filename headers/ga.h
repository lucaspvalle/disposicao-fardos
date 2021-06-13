#pragma once
#include <string>
#include <vector>
#include "data.h"


typedef std::vector<std::vector<std::string>> string2d;

typedef struct {  // Armazenamento de input via planilha
	int box;
	int qtdade;
	std::string procedencia;
	std::string tamanho;
} __input;

typedef std::vector<__input> input_csv;

typedef struct {
	int inf, sup;
} limites;

class ga {  // Algoritmo Gen�tico
public:

	const int tamanho_populacao = 900;
	const double probabilidade_mutacao = 0.05;
	const double probabilidade_cruzamento = 0.75;
	const int linhas = 4;  // Largura da matriz de refer�ncia
	const int bloco = 3;  // Par�metro para definir a regi�o de muta��o
	int tamanho_matriz, colunas, classes;  // TODO: const?

	string2d populacao;  // Popula��o (cromossomo)
	std::vector<double> fitval;  // Valores fitness da popula��o

	ga(input_csv input) {
		input_csv info_fardos = input;
		__seed();
		init();
	}

	/*
	Fun��es de apoio
	*/

	void __seed();
	int __categoria(std::string fardo);
	double __faixas(int distancia);
	bool __checar_limites(int corte, int chr, bool checar_bloco);
	limites __gerar_corte(int range, int chr, bool checar_bloco);
	std::vector<std::string> __preenchimento(std::vector<std::string> filho, std::vector<std::string> pequenos, std::vector<std::string> grandes, int corte);

	/*
	Fun��es do algoritmo gen�tico
	*/

	void init();
	std::vector<double> fitness();
	int selecao();
	void cruzamento();
	void mutacao();
};