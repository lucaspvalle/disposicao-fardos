#pragma once

// Estrutura de dados para armazenamento de input via planilha
typedef struct {
	int qtdade;					// Quantidade de fardos a serem misturados
	std::string box;			// Box de localiza��o dos fardos no estoque da empresa
	std::string procedencia;	// Proced�ncia (origem) dos fardos
	std::string tamanho;		// Tamanho dos fardos (pequenos ou grandes)
	std::string cor;			// Cor dos fardos (branco, creme, ...)
	bool fantasma;				// Fardos de apoio � constru��o da matriz (mas que n�o existem)
} __input__;

// Vetor de dados de input via planilha
typedef std::vector<__input__> info;

// Vetor 2d da matriz de refer�ncia
typedef std::vector<std::vector<std::string>> string2d;

// Zonas de corte para cruzamento e muta��o
typedef struct {
	int inf;  // Zona inferior de corte
	int sup;  // Zona superior de corte
} limites;

// Algoritmo Gen�tico
class ga {
public:

	const int tamanho_populacao = 900;				// Tamanho da popula��o de fardos
	const double probabilidade_mutacao = 0.05;		// Probabilidade de ocorrer muta��o no indiv�duo
	const double probabilidade_cruzamento = 0.75;	// Probabilidade de ocorrer cruzamento entre dois indiv�duos
	const int linhas = 4;							// Largura da matriz de refer�ncia
	const int bloco = 3;							// Par�metro para definir a regi�o de muta��o
	int tamanho_matriz;								// Tamanho da matriz de refer�ncia disposta na linha de abertura
	int colunas;									// Comprimento da matriz de refer�ncia
	string2d populacao;								// Popula��o de fardos
	std::vector<double> fitval;						// Valores fitness da popula��o
	info fardos;									// Fardos a serem misturados


	/*
	Fun��es de integra��o
	*/

	// Leitura de dados
	//
	// Input: arquivo CSV com fardos a serem misturados
	// Output: estrutura leg�vel para o algoritmo
	bool __ler_csv(std::string diretorio);

	// Escrita de dados
	//
	// Input: melhor solu��o de disposi��o do algoritmo
	// Output: arquivo CSV com estrutura leg�vel para Macro do Excel construir o mapa
	void escrever_csv(std::string diretorio);

	/*
	Fun��es de apoio
	*/

	// Classificador de fardos
	//
	// Input: identifica��o do fardo na matriz de refer�ncia
	// Output: tipo do fardo a ser misturado (proced�ncia, tamanho, ...)
	int __categoria(std::string fardo);

	// Quebra da matriz em faixas de pontua��o para dist�ncia
	//
	// Input: dist�ncia entre fardos na fun��o fitness
	// Output: peso para penaliza��o da dist�ncia
	double __faixas(int distancia);

	// Verifica zonas de corte para proteger limites de fardos grandes
	//
	// Input: local do corte, indiv�duo cortado e se deve verificar blocos (apenas para muta��o)
	// Output: elemento booleano conforme respeito do corte aos limites dos fardos
	bool __verifica_limites(int corte, int chr, bool checar_bloco);

	// Gera zonas de corte aleat�rias para operadores
	//
	// Input: �rea de corte permitida, indiv�duo cortado e se deve verificar blocos (apenas para muta��o)
	// Output: zonas de corte
	limites __gerar_corte(int range, int chr, bool checar_bloco);

	// Suporte de preenchimento de fardos para o operador OX
	//
	// Input: "filho" a ser preenchido, quantidade de fardos pequenos e grandes a serem inseridos e local de in�cio de preenchimento
	// Output: filho com sua matriz de fardos preenchida
	std::vector<std::string> __preenchimento(std::vector<std::string> filho, std::vector<std::string> pequenos, std::vector<std::string> grandes, int corte);

	/*
	Fun��es do algoritmo gen�tico
	*/

	// Inicializador de indiv�duos para a popula��o
	//
	// Input: estrutura de fardos para mistura
	// Output: popula��o de fardos
	void init();
	
	// Avalia o desempenho objetivo da matriz
	//
	// Input: popula��o de fardos
	// Output: valor fitness para cada indiv�duo da popula��o
	void fitness();

	// Torneios bin�rios de sele��o
	//
	// Input: popula��o de fardos
	// Output: 2 vencedores do torneio para cruzamento
	int selecao();

	// Operador OX de cruzamento
	//
	// Input: popula��o inicial de fardos
	// Output: nova linhagem de fardos
	void cruzamento();

	// Muta��o por troca (swap)
	//
	// Input: nova linhagem de fardos
	// Output: nova linhagem modificada de fardos
	void mutacao();
};