#pragma once

// Estrutura de dados para armazenamento de input via planilha
typedef struct {
	int qtdade;					// Quantidade de fardos a serem misturados
	std::string box;			// Box de localização dos fardos no estoque da empresa
	std::string procedencia;	// Procedência (origem) dos fardos
	std::string tamanho;		// Tamanho dos fardos (pequenos ou grandes)
	std::string cor;			// Cor dos fardos (branco, creme, ...)
	bool fantasma;				// Fardos de apoio à construção da matriz (mas que não existem)
} __input__;

// Vetor de dados de input via planilha
typedef std::vector<__input__> info;

// Vetor 2d da matriz de referência
typedef std::vector<std::vector<std::string>> string2d;

// Zonas de corte para cruzamento e mutação
typedef struct {
	int inf;  // Zona inferior de corte
	int sup;  // Zona superior de corte
} limites;

// Algoritmo Genético
class ga {
public:

	const int tamanho_populacao = 900;				// Tamanho da população de fardos
	const double probabilidade_mutacao = 0.05;		// Probabilidade de ocorrer mutação no indivíduo
	const double probabilidade_cruzamento = 0.75;	// Probabilidade de ocorrer cruzamento entre dois indivíduos
	const int linhas = 4;							// Largura da matriz de referência
	const int bloco = 3;							// Parâmetro para definir a região de mutação
	int tamanho_matriz;								// Tamanho da matriz de referência disposta na linha de abertura
	int colunas;									// Comprimento da matriz de referência
	string2d populacao;								// População de fardos
	std::vector<double> fitval;						// Valores fitness da população
	info fardos;									// Fardos a serem misturados


	/*
	Funções de integração
	*/

	// Leitura de dados
	//
	// Input: arquivo CSV com fardos a serem misturados
	// Output: estrutura legível para o algoritmo
	bool __ler_csv(std::string diretorio);

	// Escrita de dados
	//
	// Input: melhor solução de disposição do algoritmo
	// Output: arquivo CSV com estrutura legível para Macro do Excel construir o mapa
	void escrever_csv(std::string diretorio);

	/*
	Funções de apoio
	*/

	// Classificador de fardos
	//
	// Input: identificação do fardo na matriz de referência
	// Output: tipo do fardo a ser misturado (procedência, tamanho, ...)
	int __categoria(std::string fardo);

	// Quebra da matriz em faixas de pontuação para distância
	//
	// Input: distância entre fardos na função fitness
	// Output: peso para penalização da distância
	double __faixas(int distancia);

	// Verifica zonas de corte para proteger limites de fardos grandes
	//
	// Input: local do corte, indivíduo cortado e se deve verificar blocos (apenas para mutação)
	// Output: elemento booleano conforme respeito do corte aos limites dos fardos
	bool __verifica_limites(int corte, int chr, bool checar_bloco);

	// Gera zonas de corte aleatórias para operadores
	//
	// Input: área de corte permitida, indivíduo cortado e se deve verificar blocos (apenas para mutação)
	// Output: zonas de corte
	limites __gerar_corte(int range, int chr, bool checar_bloco);

	// Suporte de preenchimento de fardos para o operador OX
	//
	// Input: "filho" a ser preenchido, quantidade de fardos pequenos e grandes a serem inseridos e local de início de preenchimento
	// Output: filho com sua matriz de fardos preenchida
	std::vector<std::string> __preenchimento(std::vector<std::string> filho, std::vector<std::string> pequenos, std::vector<std::string> grandes, int corte);

	/*
	Funções do algoritmo genético
	*/

	// Inicializador de indivíduos para a população
	//
	// Input: estrutura de fardos para mistura
	// Output: população de fardos
	void init();
	
	// Avalia o desempenho objetivo da matriz
	//
	// Input: população de fardos
	// Output: valor fitness para cada indivíduo da população
	void fitness();

	// Torneios binários de seleção
	//
	// Input: população de fardos
	// Output: 2 vencedores do torneio para cruzamento
	int selecao();

	// Operador OX de cruzamento
	//
	// Input: população inicial de fardos
	// Output: nova linhagem de fardos
	void cruzamento();

	// Mutação por troca (swap)
	//
	// Input: nova linhagem de fardos
	// Output: nova linhagem modificada de fardos
	void mutacao();
};