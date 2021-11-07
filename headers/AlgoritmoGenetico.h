#pragma once

class AlgoritmoGenetico
{
private:

	const int tamanho_populacao = 900;				// Tamanho da população de fardos
	const double probabilidade_mutacao = 0.05;		// Probabilidade de ocorrer mutação no indivíduo
	const double probabilidade_cruzamento = 0.75;	// Probabilidade de ocorrer cruzamento entre dois indivíduos
	
	/* Quebra da matriz em faixas de pontuação para distância
	* 
	* @param: distância de fardos entre colunas na matriz de referência
	* @return: peso para penalização
	*/
	double __faixas(int distancia);

	/* Verificação de zonas de corte para proteger limites de fardos grandes
	*
	* @param: local do corte
	* @param: indivíduo cortado
	* @param: verificar blocos (apenas para mutação)
	* @return: o corte respeita os limites dos fardos?
	*/
	bool __verifica_limites(int corte, int chr, bool checar_bloco);

	/* Gerador de zonas de corte para operadores genéticos
	*
	* @param: área de corte permitida
	* @param: indivíduo cortado
	* @param: verificar blocos (apenas para mutação)
	* @return: zonas de corte
	*/
	ZonasDeCorte __gerar_corte(int range, int chr, bool checar_bloco);

	/* Suporte de preenchimento de fardos para o operador OX
	* 
	* @param: indivíduo a ser preenchido
	* @param: quantidade de fardos pequenos a serem inseridos
	* @param: quantidade de fardos grandes a serem inseridos
	* @param: local de início do preenchimento
	* @return: filho com a sua matriz de fardos preenchida
	*/
	std::vector<std::string> __preenchimento(std::vector<std::string> filho, std::vector<std::string> pequenos, std::vector<std::string> grandes, int corte);

	/* Gerador de fardos fantasmas para aproveitamento completo da matriz
	
	@param: fardos a serem misturados
	@param: indivíduo a ser preenchido
	@param: identificador de fardos grandes
	*/
	EstruturaDeFantasmas __preenchimento_fantasma(InputsFardos fardos, std::vector<std::string> individuo, bool is_grande);

public:

	static const int linhas = 4;					// Largura da matriz de referência
	const int tamanho_geracao = 250;				// Tamanho de gerações a serem iteradas
	const int bloco = 3;							// Parâmetro para definir a região de mutação
	int tamanho_matriz;								// Tamanho (em células) da matriz de referência
	int colunas;									// Comprimento da matriz de referência
	string2d populacao;								// População de fardos
	std::vector<double> valores_fitness;			// Valores fitness da população
	InputsFardos fardos;							// Fardos a serem misturados

	AlgoritmoGenetico(InputsFardos dados) {
		fardos = dados;
	}

	/* Classificador
	*
	* Este método está como público pois é acessado pelo Integrador
	* 
	* @param: identificação do fardo na matriz de referência
	* @return: categoria do fardo
	*/
	static int __categoria(std::string fardo);

	// Inicializador de indivíduos para a população
	void init();
	
	// Avalia o desempenho objetivo da matriz
	void fitness();

	// Torneios binários de seleção
	int selecao();

	// Operador OX de cruzamento
	void cruzamento();

	// Mutação por troca (swap)
	void mutacao();
};