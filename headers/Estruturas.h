#pragma once

// Estrutura de dados para armazenamento de input via planilha
typedef struct {
	std::string box;			// Box de localização dos fardos no estoque da empresa
	std::string procedencia;	// Procedência (origem) dos fardos
	std::string cor;			// Cor dos fardos (branco, creme, ...)
	int qtdade;					// Quantidade de fardos a serem misturados
	bool is_grande;				// Tamanho dos fardos (pequenos ou grandes)
	bool fantasma;				// Fardos de apoio à construção da matriz (mas que não existem)
} EstruturaDeDadosDeEntrada;

// Vetor de dados de input via planilha
typedef std::vector<EstruturaDeDadosDeEntrada> InputsFardos;

// Vetor 2d da matriz de referência
typedef std::vector<std::vector<std::string>> string2d;

// Zonas de corte para cruzamento e mutação
typedef struct {
	int inf;  // Zona inferior de corte
	int sup;  // Zona superior de corte
} ZonasDeCorte;

typedef struct {
	InputsFardos fardos;
	std::vector<std::string> individuo;
} EstruturaDeFantasmas;