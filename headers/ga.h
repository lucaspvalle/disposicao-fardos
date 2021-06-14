#pragma once
#include <string>
#include <vector>


typedef struct {  // Armazenamento de input via planilha
	int qtdade;
	std::string box;
	std::string procedencia;
	std::string tamanho;
	std::string cor;
} __input__;

typedef std::vector<__input__> info;

typedef std::vector<std::vector<std::string>> string2d;

typedef struct {
	int inf, sup;
} limites;

class ga {  // Algoritmo Genético
public:

	const int tamanho_populacao = 900;
	const double probabilidade_mutacao = 0.05;
	const double probabilidade_cruzamento = 0.75;
	const int linhas = 4;  // Largura da matriz de referência
	const int bloco = 3;  // Parâmetro para definir a região de mutação
	int tamanho_matriz, colunas;  // TODO: const?

	string2d populacao;  // População (cromossomo)
	std::vector<double> fitval;  // Valores fitness da população

	info fardos;

	ga() {
		__ler_csv();
	}

	/*
	Funções de integração
	*/

	void __ler_csv();
	void escrever_csv();

	/*
	Funções de apoio
	*/

	int __categoria(std::string fardo);
	double __faixas(int distancia);
	bool __checar_limites(int corte, int chr, bool checar_bloco);
	limites __gerar_corte(int range, int chr, bool checar_bloco);
	std::vector<std::string> __preenchimento(std::vector<std::string> filho, std::vector<std::string> pequenos, std::vector<std::string> grandes, int corte);

	/*
	Funções do algoritmo genético
	*/

	void init();
	void fitness();
	int selecao();
	void cruzamento();
	void mutacao();
};