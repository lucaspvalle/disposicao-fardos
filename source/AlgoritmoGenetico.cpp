#include "../headers/framework.h"
using namespace std;

/*
Funções de apoio
*/

int AlgoritmoGenetico::__categoria(string fardo) {

	// Exemplo de string: "123b"
	fardo.pop_back();		// Retira o último caracter ("b")
	return stoi(fardo);		// Retorna a string transformada em inteiro ("123" para 123)
}

double AlgoritmoGenetico::__faixas(int distancia) {

	const double dist = static_cast<const double>(distancia);
	const double col = static_cast<const double>(colunas);
	const double largura = ceil(sqrt(col));  // A largura é calculada de acordo com a regra de classes de histogramas
	const double tamanho = col / largura;

	if (dist >= col - largura)  // Minimiza a penalização da maior distância possível
		return 1.0;

	double intervalo = 0;
	for (double i = 0; i < largura; i++) {

		intervalo = i * tamanho;
		if ((dist >= intervalo) && (dist < intervalo + tamanho))
			return (i + 1.0);
	}
	return 0.0;
}

bool AlgoritmoGenetico::__verifica_limites(int corte, int chr, bool checar_bloco = false) {

	if (!checar_bloco) {  // Não verificar blocos
		for (int i = 0; i < linhas; i++) {
			int celula = corte + i;

			char letra = populacao[chr][celula].back();
			bool is_grande = fardos[__categoria(populacao[chr][celula])].is_grande;

			// Regra: se o fardo grande estiver em sua posição inicial ("a"), ele NÃO está sendo cortado
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
				// um fardo pequeno na mesma coluna mostra que o fardo grande está sendo cortado
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

	if (checar_bloco) {  // Caso haja sobreposição de cortes em relação ao bloco de mutação,
		if ((corte_sup - corte_inf) <= bloco) {
			// Se o corte superior estiver no fim do bloco e sem espaco para a inversão de colunas,
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

			// Regra: se a célula iterada não extrapolar os limites da matriz OU estiver preenchida, continua
			if (celula_iterada >= tamanho_matriz || !filho[celula_iterada].empty())
				continua = false;
		}

		if (continua) {
			for (int qtd = 0; qtd < 2; qtd++) {  // Fardos grandes são alocados em pares
				grandes[qtd].pop_back();  // Remove o identificador ("b" de "123b") para nova identificação

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

			// Regra: se a célula iterada não extrapolar os limites da matriz OU estiver preenchida, continua
			if (celula_iterada >= tamanho_matriz || !filho[celula_iterada].empty())
				continua = false;
		}

		if (continua) {  // Fardos pequenos são alocados individualmente
			pequenos[0].pop_back();  // Remove o identificador ("b" de "123b") para nova identificação

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

/*
Funções do algoritmo genético
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

	// Criando fardos fantasmas para melhor aproveitamento da matriz (se necessário)
	int contagem_pequenos = static_cast<int>(pequenos.size()), pequenos_fantasmas = 0;
	while ((contagem_pequenos % 3 != 0) || ((contagem_pequenos / 3) % 2 != 0)) {
		pequenos_fantasmas++;
		contagem_pequenos += pequenos_fantasmas;
	}

	int contagem_grandes = static_cast<int>(grandes.size()), grandes_fantasmas = 0;
	while ((contagem_grandes % 2 != 0) || ((contagem_grandes / 2) % 2 != 0)) {
		grandes_fantasmas++;
		contagem_grandes += grandes_fantasmas;
	}

	if (pequenos_fantasmas > 0) {
		fardos.push_back({ "NDA", "NDA", "NDA", pequenos_fantasmas, false, true });
		int idx = static_cast<const int>(fardos.size()) - 1;

		for (int j = 0; j < pequenos_fantasmas; j++)
			pequenos.push_back(to_string(idx) + 'a');
	}

	if (grandes_fantasmas > 0) {
		fardos.push_back({ "NDA", "NDA","NDA", grandes_fantasmas, true, true });
		int idx = static_cast<const int>(fardos.size()) - 1;

		for (int j = 0; j < grandes_fantasmas; j++)
			grandes.push_back(to_string(idx) + 'a');
	}

	// Calculando o tamanho necessário da matriz para acomodar todos os fardos
	AlgoritmoGenetico::tamanho_matriz = contagem_pequenos * 2 + contagem_grandes * 3;
	AlgoritmoGenetico::colunas = static_cast<int>(ceil(tamanho_matriz / linhas));

	// Inicializando a população com indivíduos de tamanho pré-definido
	string2d populacao(tamanho_populacao, vector<string>(tamanho_matriz, ""));

	for (int individuo = 0; individuo < tamanho_populacao; individuo++)
		populacao[individuo] = __preenchimento(populacao[individuo], pequenos, grandes);

	AlgoritmoGenetico::populacao = populacao;
}

void AlgoritmoGenetico::fitness() {

	const int classes = static_cast<const int>(fardos.size());  // Quantidade de categorias de fardos
	vector<double> valores(tamanho_populacao, 0.0);				// Valor fitness de cada indivíduo

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {
		vector<double> ponderado(classes, 0.0);
		vector<vector<int>> localizacao(classes);  // Localização de cada tipo de fardo na matriz

		for (int celula = 0; celula < tamanho_matriz; celula++) {
			int coluna = celula / linhas;
			int categoria = __categoria(populacao[individuo][celula]);

			// Condição para inserir colunas repetidas no vetor
			if (localizacao[categoria].size() == 0)  // Necessário pois não é possível obter .back() de um vetor vazio
				localizacao[categoria].push_back(coluna);

			if (localizacao[categoria].back() != coluna)
				localizacao[categoria].push_back(coluna);

			// Armazenando espaço (em colunas) ocupado por cada categoria
			if (populacao[individuo][celula].back() == 'a') {
				if (fardos[categoria].is_grande)
					ponderado[categoria] += 3.0;
				else
					ponderado[categoria] += 1.0;
			}
		}

		for (int categoria = 0; categoria < classes; categoria++) {
			if (localizacao[categoria].size() > 1) {  // Há distância para cálculo apenas se o fardo ocupar mais de duas colunas

				double col = static_cast<double>(colunas);
				double espaco_ocupado = static_cast<double>(localizacao[categoria].size());

				// Penalização por % de colunas ocupadas
				double peso = espaco_ocupado / min(col, ponderado[categoria]);

				for (int coluna_iterada = 0; coluna_iterada < localizacao[categoria].size() - 1; coluna_iterada++) {
					int proxima_coluna_iterada = coluna_iterada + 1;

					int distancia = localizacao[categoria][proxima_coluna_iterada] - localizacao[categoria][coluna_iterada];
					valores[individuo] += peso * __faixas(distancia);
				}
			}
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
			
			continue;  // Pula a execução de cruzamento
		}

		// Popula o filho com a informação genética da mãe na região de corte
		ZonasDeCorte cortes = __gerar_corte(colunas, mae);
		copy(populacao[mae].begin() + cortes.inf, populacao[mae].begin() + cortes.sup, filho.begin() + cortes.inf);

		// Armazena quantos fardos de cada categoria já foram alocados no filho
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

		// Popula o filho com a informação genética do pai fora da região de corte
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
