#pragma once

// Estrutura de dados para armazenamento de input via planilha
typedef struct {
	std::string box;			// Box de localiza��o dos fardos no estoque da empresa
	std::string procedencia;	// Proced�ncia (origem) dos fardos
	std::string cor;			// Cor dos fardos (branco, creme, ...)
	int qtdade;					// Quantidade de fardos a serem misturados
	bool is_grande;				// Tamanho dos fardos (pequenos ou grandes)
	bool fantasma;				// Fardos de apoio � constru��o da matriz (mas que n�o existem)
} EstruturaDeDadosDeEntrada;

// Vetor de dados de input via planilha
typedef std::vector<EstruturaDeDadosDeEntrada> InputsFardos;

// Vetor 2d da matriz de refer�ncia
typedef std::vector<std::vector<std::string>> string2d;

// Zonas de corte para cruzamento e muta��o
typedef struct {
	int inf;  // Zona inferior de corte
	int sup;  // Zona superior de corte
} ZonasDeCorte;

typedef struct {
	InputsFardos fardos;
	std::vector<std::string> individuo;
} EstruturaDeFantasmas;