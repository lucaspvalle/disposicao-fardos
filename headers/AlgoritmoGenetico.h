#pragma once

class AlgoritmoGenetico
{
private:

	const int tamanho_populacao = 900;				// Tamanho da popula��o de fardos
	const double probabilidade_mutacao = 0.05;		// Probabilidade de ocorrer muta��o no indiv�duo
	const double probabilidade_cruzamento = 0.75;	// Probabilidade de ocorrer cruzamento entre dois indiv�duos
	
	/* Quebra da matriz em faixas de pontua��o para dist�ncia
	* 
	* @param: dist�ncia de fardos entre colunas na matriz de refer�ncia
	* @return: peso para penaliza��o
	*/
	double __faixas(int distancia);

	/* Verifica��o de zonas de corte para proteger limites de fardos grandes
	*
	* @param: local do corte
	* @param: indiv�duo cortado
	* @param: verificar blocos (apenas para muta��o)
	* @return: o corte respeita os limites dos fardos?
	*/
	bool __verifica_limites(int corte, int chr, bool checar_bloco);

	/* Gerador de zonas de corte para operadores gen�ticos
	*
	* @param: �rea de corte permitida
	* @param: indiv�duo cortado
	* @param: verificar blocos (apenas para muta��o)
	* @return: zonas de corte
	*/
	ZonasDeCorte __gerar_corte(int range, int chr, bool checar_bloco);

	/* Suporte de preenchimento de fardos para o operador OX
	* 
	* @param: indiv�duo a ser preenchido
	* @param: quantidade de fardos pequenos a serem inseridos
	* @param: quantidade de fardos grandes a serem inseridos
	* @param: local de in�cio do preenchimento
	* @return: filho com a sua matriz de fardos preenchida
	*/
	std::vector<std::string> __preenchimento(std::vector<std::string> filho, std::vector<std::string> pequenos, std::vector<std::string> grandes, int corte);

	/* Gerador de fardos fantasmas para aproveitamento completo da matriz
	
	@param: fardos a serem misturados
	@param: indiv�duo a ser preenchido
	@param: identificador de fardos grandes
	*/
	EstruturaDeFantasmas __preenchimento_fantasma(InputsFardos fardos, std::vector<std::string> individuo, bool is_grande);

public:

	static const int linhas = 4;					// Largura da matriz de refer�ncia
	const int tamanho_geracao = 250;				// Tamanho de gera��es a serem iteradas
	const int bloco = 3;							// Par�metro para definir a regi�o de muta��o
	int tamanho_matriz;								// Tamanho (em c�lulas) da matriz de refer�ncia
	int colunas;									// Comprimento da matriz de refer�ncia
	string2d populacao;								// Popula��o de fardos
	std::vector<double> valores_fitness;			// Valores fitness da popula��o
	InputsFardos fardos;							// Fardos a serem misturados

	AlgoritmoGenetico(InputsFardos dados) {
		fardos = dados;
	}

	/* Classificador
	*
	* Este m�todo est� como p�blico pois � acessado pelo Integrador
	* 
	* @param: identifica��o do fardo na matriz de refer�ncia
	* @return: categoria do fardo
	*/
	static int __categoria(std::string fardo);

	// Inicializador de indiv�duos para a popula��o
	void init();
	
	// Avalia o desempenho objetivo da matriz
	void fitness();

	// Torneios bin�rios de sele��o
	int selecao();

	// Operador OX de cruzamento
	void cruzamento();

	// Muta��o por troca (swap)
	void mutacao();
};