#include "../headers/framework.h"
using namespace std;

/*
Fun��es de apoio
*/

int AlgoritmoGenetico::__categoria(string fardo) {

	// Exemplo de string: "123b"
	fardo.pop_back();		// Retira o �ltimo caracter ("b")
	return stoi(fardo);		// Retorna a string transformada em inteiro ("123" para 123)
}

double AlgoritmoGenetico::__faixas(int distancia) {

	const double dist = static_cast<const double>(distancia);
	const double col = static_cast<const double>(colunas);
	const double largura = ceil(sqrt(col));  // A largura � calculada de acordo com a regra de classes de histogramas
	const double tamanho = col / largura;

	if (dist >= col - largura)  // Minimiza a penaliza��o da maior dist�ncia poss�vel
		return 1.0;

	for (double i = 0.0; i < largura; i++) {
		double intervalo = i * tamanho;

		if (dist < intervalo + tamanho)
			return i + 1.0;
	}
	return 0.0;
}

bool AlgoritmoGenetico::__verifica_limites(int corte, int chr, bool checar_bloco = false) {

	if (!checar_bloco) {  // N�o verificar blocos
		for (int i = 0; i < linhas; i++) {
			int celula = corte + i;

			char letra = populacao[chr][celula].back();
			bool is_grande = fardos[__categoria(populacao[chr][celula])].is_grande;

			// Regra: se o fardo grande estiver em sua posi��o inicial ("a"), ele N�O est� sendo cortado
			if ((is_grande) && (letra != 'a'))
				return true;
		}
	}
	else {  // Verificar blocos
		for (int col = 0; col < bloco; col++) {
			for (int i = 0; i < linhas; i++) {
				int celula = corte + col * linhas + i;

				bool fardo_na_iteracao_is_grande = fardos[__categoria(populacao[chr][celula])].is_grande;
				bool fardo_no_corte_is_grande = fardos[__categoria(populacao[chr][corte])].is_grande;

				// Regra: como um fardo grande possui o mesmo tamanho do bloco,
				// um fardo pequeno na mesma coluna mostra que o fardo grande est� sendo cortado
				if ((!fardo_no_corte_is_grande) && fardo_na_iteracao_is_grande)
					return true;
			}
		}
	}
	return false;  // Quebra o loop caso nenhum fardo estiver sendo cortado ao meio
}

ZonasDeCorte AlgoritmoGenetico::__gerar_corte(int range, int chr, bool checar_bloco = false) {

	int corte_inf = 0, corte_sup = 0;

	while (corte_inf == corte_sup)  // Gera os pontos de corte aleatoriamente
		corte_inf = rand() % range, corte_sup = rand() % range;

	if (corte_sup < corte_inf)  // Caso a coluna superior seja menor do que a inferior, trocar os valores
		swap(corte_inf, corte_sup);

	if (checar_bloco) {  // Caso haja sobreposi��o de cortes em rela��o ao bloco de muta��o,
		if ((corte_sup - corte_inf) <= bloco) {
			// Se o corte superior estiver no fim do bloco e sem espaco para a invers�o de colunas,
			if ((corte_sup + bloco) >= (colunas - bloco))
				corte_inf -= bloco;
			else
				corte_sup += bloco;
		}
	}
	corte_inf *= linhas, corte_sup *= linhas;

	// Verificando se os limites gerados cortam algum fardo ao meio
	while (__verifica_limites(corte_inf, chr))
		corte_inf -= linhas;

	while (__verifica_limites(corte_sup, chr))
		corte_sup -= linhas;

	return { corte_inf, corte_sup };
}

vector<string> AlgoritmoGenetico::__preenchimento(vector<string> filho, vector<string> pequenos, vector<string> grandes, int corte = 0) {

	string variantes = { "abc" };
	int coluna_iterada = corte / linhas, linha_iterada = 0;

	// Misturando a ordem de fardos alocados
	std::random_device rd;
	std::mt19937 g(rd());

	shuffle(pequenos.begin(), pequenos.end(), g);
	shuffle(grandes.begin(), grandes.end(), g);

	while (!grandes.empty()) {
		bool continua = true;
		int celula_inicial = linha_iterada + coluna_iterada * linhas;

		for (int espaco_ocupado = 0; espaco_ocupado < 3; espaco_ocupado++) {
			int celula_iterada = celula_inicial + espaco_ocupado * linhas;

			// Regra: se a c�lula iterada n�o extrapolar os limites da matriz OU estiver preenchida, continua
			if (celula_iterada >= tamanho_matriz || !filho[celula_iterada].empty())
				continua = false;
		}

		if (continua) {
			for (int qtd = 0; qtd < 2; qtd++) {  // Fardos grandes s�o alocados em pares
				grandes[qtd].pop_back();  // Remove o identificador ("b" de "123b") para nova identifica��o

				for (int espaco_ocupado = 0; espaco_ocupado < 3; espaco_ocupado++) {
					int celula_iterada = (celula_inicial + qtd) + espaco_ocupado * linhas;
					filho[celula_iterada] = grandes[qtd] + variantes[espaco_ocupado];
				}
			}
			grandes.erase(grandes.begin(), grandes.begin() + 2);
		}
		coluna_iterada += 3;

		if (coluna_iterada >= colunas)
			linha_iterada += 2, coluna_iterada = 0;

		if (linha_iterada == linhas)
			linha_iterada = 0;
	}

	while (!pequenos.empty()) {
		bool continua = true;
		int celula_inicial = linha_iterada + coluna_iterada * linhas;

		for (int espaco_ocupado = 0; espaco_ocupado < 2; espaco_ocupado++) {
			int celula_iterada = celula_inicial + espaco_ocupado;

			// Regra: se a c�lula iterada n�o extrapolar os limites da matriz OU estiver preenchida, continua
			if (celula_iterada >= tamanho_matriz || !filho[celula_iterada].empty())
				continua = false;
		}

		if (continua) {  // Fardos pequenos s�o alocados individualmente
			pequenos[0].pop_back();  // Remove o identificador ("b" de "123b") para nova identifica��o

			for (int espaco_ocupado = 0; espaco_ocupado < 2; espaco_ocupado++) {
				int celula_iterada = celula_inicial + espaco_ocupado;
				filho[celula_iterada] = pequenos[0] + variantes[espaco_ocupado];
			}
			pequenos.erase(pequenos.begin());
		}
		linha_iterada += 2;

		if (linha_iterada == linhas)
			linha_iterada = 0, coluna_iterada++;

		if (coluna_iterada == colunas)
			linha_iterada = 0, coluna_iterada = 0;
	}
	return filho;
}

EstruturaDeFantasmas AlgoritmoGenetico::__preenchimento_fantasma(InputsFardos fardos, vector<string> individuo, bool is_grande) {

	int contagem = static_cast<int>(individuo.size()), parametro = is_grande ? 2 : 3, fantasmas = 0;

	while ((contagem % parametro != 0) || ((contagem / parametro) % 2 != 0))
		fantasmas++, contagem += fantasmas;

	if (fantasmas > 0) {
		fardos.push_back({ "NDA", "NDA", "NDA", fantasmas, is_grande, true });
		
		int idx = static_cast<const int>(fardos.size()) - 1;
		for (int j = 0; j < fantasmas; j++)
			individuo.push_back(to_string(idx) + 'a');
	}
	return { fardos, individuo };
}

/*
Fun��es do algoritmo gen�tico
*/

void AlgoritmoGenetico::init() {

	vector<string> grandes, pequenos;

	for (unsigned int idx = 0; idx < fardos.size(); idx++) {
		bool is_grande = fardos[idx].is_grande;
		int qtdade = fardos[idx].qtdade;

		for (int j = 0; j < qtdade; j++) {
			if (!is_grande)
				pequenos.push_back(to_string(idx) + 'a');
			else
				grandes.push_back(to_string(idx) + 'a');
		}
	}

	// Criando fardos fantasmas para melhor aproveitamento da matriz (se necess�rio)	
	EstruturaDeFantasmas fantasmas = __preenchimento_fantasma(fardos, pequenos, false);
	fardos = fantasmas.fardos, pequenos = fantasmas.individuo;

	fantasmas = __preenchimento_fantasma(fardos, grandes, true);
	fardos = fantasmas.fardos, grandes = fantasmas.individuo;

	// Calculando o tamanho necess�rio da matriz para acomodar todos os fardos
	AlgoritmoGenetico::tamanho_matriz = static_cast<int>(pequenos.size()) * 2 + static_cast<int>(grandes.size()) * 3;
	AlgoritmoGenetico::colunas = static_cast<int>(ceil(tamanho_matriz / linhas));

	// Inicializando a popula��o com indiv�duos de tamanho pr�-definido
	string2d populacao(tamanho_populacao, vector<string>(tamanho_matriz, ""));

	for (int individuo = 0; individuo < tamanho_populacao; individuo++)
		populacao[individuo] = __preenchimento(populacao[individuo], pequenos, grandes);

	AlgoritmoGenetico::populacao = populacao;
}

void AlgoritmoGenetico::fitness() {

	const int classes = static_cast<const int>(fardos.size());  // Quantidade de categorias de fardos
	vector<double> valores(tamanho_populacao, 0.0);				// Valor fitness de cada indiv�duo
	double col = static_cast<double>(colunas);					// Apenas transformando a quantidade de colunas em double

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {
		vector<double> fitness_por_categoria(classes, 0.0), colunas_ocupadas(classes, 0.0), ocupacao_ideal(classes, 0.0);
		vector<int> ultima_localizacao(classes, -1), localizacao_atual(classes);

		for (int celula = 0; celula < tamanho_matriz; celula++) {
			int coluna = celula / linhas;
			int categoria = __categoria(populacao[individuo][celula]);

			localizacao_atual[categoria] = coluna;

			if (ultima_localizacao[categoria] >= 0) {
				int distancia = localizacao_atual[categoria] - ultima_localizacao[categoria];
				fitness_por_categoria[categoria] += (distancia > 0) ? __faixas(distancia) : 0.0;
			}

			// A ocupa��o ideal guarda a ocupa��o m�xima dos fardos em colunas da matriz
			if (populacao[individuo][celula].back() == 'a')
				ocupacao_ideal[categoria] += fardos[categoria].is_grande ? 3.0 : 1.0;

			// A ocupa��o atual guarda quantas colunas foram de fato aproveitadas
			if (ultima_localizacao[categoria] != localizacao_atual[categoria])
				colunas_ocupadas[categoria]++;

			ultima_localizacao[categoria] = coluna;
		}
		
		for (int categoria = 0; categoria < classes; categoria++) {
			// A fun��o fitness penaliza a baixa dispers�o de fardos em colunas da matriz
			double peso = colunas_ocupadas[categoria] / min(col, ocupacao_ideal[categoria]);
			valores[individuo] += peso * fitness_por_categoria[categoria];
		}
	}
	AlgoritmoGenetico::valores_fitness = valores;
}

int AlgoritmoGenetico::selecao() {

	int vencedor = rand() % tamanho_populacao, desafiante = rand() % tamanho_populacao;

	while (vencedor == desafiante)
		desafiante = rand() % tamanho_populacao;

	if (valores_fitness[desafiante] >= valores_fitness[vencedor])  // Quem possuir MAIOR valor fitness, vence
		vencedor = desafiante;

	return vencedor;
}

void AlgoritmoGenetico::cruzamento() {

	string2d linhagem;

	for (vector<string> individuo : populacao) {

		int pai = selecao(), mae = selecao();
		vector<string> filho(tamanho_matriz, ""), pequenos, grandes;

		double numero_aleatorio = rand() / (double)RAND_MAX;

		if (numero_aleatorio >= probabilidade_cruzamento) {
			if (valores_fitness[pai] >= valores_fitness[mae])
				linhagem.push_back(populacao[pai]);
			else
				linhagem.push_back(populacao[mae]);
			
			continue;  // Pula a execu��o de cruzamento
		}

		// Popula o filho com a informa��o gen�tica da m�e na regi�o de corte
		ZonasDeCorte cortes = __gerar_corte(colunas, mae);
		copy(populacao[mae].begin() + cortes.inf, populacao[mae].begin() + cortes.sup, filho.begin() + cortes.inf);

		// Armazena quantos fardos de cada categoria j� foram alocados no filho
		vector<int> contagem(fardos.size(), 0);

		for (string fardo : filho) {
			if (!fardo.empty())
				if (fardo.back() == 'a')
					contagem[__categoria(fardo)] += 1;
		}
		
		for (int idx = 0; idx < contagem.size(); idx++) {
			int faltantes = fardos[idx].qtdade - contagem[idx];

			for (int qtd = 0; qtd < faltantes; qtd++) {
				if (fardos[idx].is_grande)
					grandes.push_back(to_string(idx) + 'a');
				else
					pequenos.push_back(to_string(idx) + 'a');
			}
		}

		// Popula o filho com a informa��o gen�tica do pai fora da regi�o de corte
		filho = __preenchimento(filho, pequenos, grandes, cortes.sup);
		linhagem.push_back(filho);
	}
	AlgoritmoGenetico::populacao = linhagem;
}

void AlgoritmoGenetico::mutacao() {

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {

		double numero_aleatorio = rand() / (double)RAND_MAX;

		if (numero_aleatorio <= probabilidade_mutacao) {
			ZonasDeCorte cortes = __gerar_corte(colunas - bloco, individuo, true);

			while (__verifica_limites(cortes.inf, individuo, true))
				cortes.inf += linhas;
				
			while (__verifica_limites(cortes.sup, individuo, true))
				cortes.sup += linhas;

			// Swap nos intervalos dos cortes inferior e superior de acordo com o tamanho do bloco de troca
			const int tamanho = bloco * linhas;
			swap_ranges(populacao[individuo].begin() + cortes.inf, populacao[individuo].begin() + cortes.inf + tamanho,
						populacao[individuo].begin() + cortes.sup);
		}
	}
}
