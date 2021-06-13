#include "../headers/ga.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <cmath>
#include <Windows.h>
using namespace std;


/*
Fun��es de integra��o
*/

__data__ ga::__ler_csv() {  // Entrada: Informa��es de Fardos

	ifstream arquivo;

	arquivo.open("data/input.csv", ios::in);
	if (arquivo.is_open()) {

		__data__ info_fardos;
		const double criterio_peso = 220;

		// Modelo do arquivo: "qtdade,procedencia,peso,box,cor\n"
		string box, procedencia, peso, qtdade, tamanho, cor;

		while (getline(arquivo, qtdade, ',')) {
			getline(arquivo, procedencia, ',');
			getline(arquivo, peso, ',');
			getline(arquivo, box, ',');
			getline(arquivo, cor);

			// Regra: caso o peso seja menor do que o crit�rio, � classificado como pequeno
			if (stod(peso) < criterio_peso)
				tamanho = "pequeno";
			else
				tamanho = "grande";

			info_fardos.push_back({ stoi(qtdade), box, procedencia, tamanho, cor });
		}
		arquivo.close();

		return info_fardos;
	}
	else {
		MessageBoxA(NULL, (LPCSTR)"Arquivo n�o encontrado!", (LPCSTR)"Disposi��o de Fardos", MB_ICONWARNING);
		exit(1);
	}
}

void ga::escrever_csv() {  // Sa�da: Mapa de Disposi��o de Fardos

	const int idx_maior_fitness = static_cast<const int>(max_element(fitval.begin(), fitval.end()) - fitval.begin());
	vector<string> vencedor = populacao[idx_maior_fitness];

	ofstream arquivo;

	arquivo.open("data/output.csv", ios::trunc);
	if (arquivo.is_open()) {

		int posicao = 0;
		for (string fardo : vencedor) {  // Iterando c�lula a c�lula da matriz
			if (!fardo.empty()) {

				int categoria = __categoria(fardo);

				string procedencia = info_fardos[categoria].procedencia;
				string box = info_fardos[categoria].box;
				string tamanho = info_fardos[categoria].tamanho;

				if (fardo.back() == 'a') {  // Escrever apenas para a primeira ocorr�ncia do fardo na matriz
					arquivo << procedencia << " (" << box << "),";
				}
				else {  // Identifica��o do tamanho do fardo para a constru��o do mapa em Excel
					if (tamanho == "grande")
						arquivo << "2" << ',';
					else
						arquivo << "1" << ',';
				}
			}
			if (((posicao - 3) % linhas) == 0)  // Quebra de linha do arquivo quando estiver na 4� linha da matriz
				arquivo << "\n";

			posicao++;
		}
		arquivo.close();
	}
	else {
		MessageBoxA(NULL, (LPCSTR)"Arquivo n�o encontrado!", (LPCSTR)"Disposi��o de Fardos", MB_ICONWARNING);
		exit(1);
	}
	MessageBoxA(NULL, (LPCSTR)"Algoritmo executado com sucesso!", (LPCSTR)"Disposi��o de Fardos", MB_ICONINFORMATION);
}


/*
Fun��es de apoio
*/

int ga::__categoria(string fardo) {  // Obten��o dos dados de entrada do fardo

	// Exemplo de string: "123b"
	fardo.pop_back();  // Retira o �ltimo caracter ("b")
	int id = stoi(fardo);  // Converte a string para inteiro

	int cumsum = 0;
	for (unsigned int tipo = 0; tipo < info_fardos.size(); tipo++) {
		cumsum += info_fardos[tipo].qtdade;

		if (id <= cumsum)
			return tipo;
	}
	return 0;
}

double ga::__faixas(int distancia) {  // Quebra da matriz em faixas de pontua��o para dist�ncia

	const double largura = ceil(sqrt(colunas));  // A largura � calculada de acordo com a regra de classes de histogramas
	const double tamanho = colunas / largura;
	const double dist = static_cast<const double>(distancia);
	const double col = static_cast<const double>(colunas);

	if (dist >= col - largura)  // Minimiza a penaliza��o da maior dist�ncia poss�vel
		return 1.0;

	double intervalo = 0;
	for (double i = 0; i < largura; i++) {

		intervalo = i * tamanho;
		if ((dist >= intervalo) && (dist < intervalo + tamanho))
			return (i + 1.0);
	}
	return 0.0;
}

bool ga::__checar_limites(int corte, int chr, bool checar_bloco = false) {  // Verifica��o de cortes ao meio de um fardo grande

	if (!checar_bloco) {  // N�o verificar blocos
		for (int i = 0; i < linhas; i++) {
			int celula = corte + i;

			char letra = populacao[chr][celula].back();
			string tamanho = info_fardos[__categoria(populacao[chr][celula])].tamanho;

			// Regra: se o fardo grande estiver em sua posi��o inicial ("a"), ele N�O est� sendo cortado
			if ((tamanho == "grande") && (letra != 'a'))
				return true;
		}
	}
	else {  // Verificar blocos
		for (int col = 0; col < bloco; col++) {
			for (int i = 0; i < linhas; i++) {
				int celula = corte + col * linhas + i;

				string tamanho_fardo_na_iteracao = info_fardos[__categoria(populacao[chr][celula])].tamanho;
				string tamanho_fardo_no_corte = info_fardos[__categoria(populacao[chr][corte])].tamanho;

				// Regra: como um fardo grande possui o mesmo tamanho do bloco,
				// um fardo pequeno na mesma coluna mostra que o fardo grande est� sendo cortado
				if (tamanho_fardo_no_corte == "pequeno" && tamanho_fardo_na_iteracao == "grande")
					return true;
			}
		}
	}
	return false;  // Quebra o loop caso nenhum fardo estiver sendo cortado ao meio
}

limites ga::__gerar_corte(int range, int chr, bool checar_bloco = false) {

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
	while (__checar_limites(corte_inf, chr))
		corte_inf -= linhas;

	while (__checar_limites(corte_sup, chr))
		corte_sup -= linhas;

	return { corte_inf, corte_sup };
}

vector<string> ga::__preenchimento(vector<string> filho, vector<string> pequenos, vector<string> grandes, int corte = 0) {
	// Suporte de preenchimento de fardos para o operador OX

	int var, var_aux, quebra, i = corte / linhas, j = 0;
	string variantes = { "abc" };

	while (grandes.size() != 0) {
		quebra = 0, var = j + i * linhas;

		for (int d = 0; d < 3; d++) {
			var_aux = var + d * linhas;
			if (var_aux >= tamanho_matriz || !filho[var_aux].empty())
				quebra = 1;
		}

		if (quebra != 1) {
			for (int n = 0; n < 2; n++) {
				grandes[n].pop_back();

				for (int d = 0; d < 3; d++) {
					var_aux = (var + n) + d * linhas;
					filho[var_aux] = grandes[n] + variantes[d];
				}
			}
			grandes.erase(grandes.begin(), grandes.begin() + 2);
		}
		i += 3;

		if (i >= colunas)
			j += 2, i = 0;
		if (j == linhas)
			j = 0;
	}

	while (pequenos.size() != 0) {
		quebra = 0, var = j + i * linhas;

		for (int d = 0; d < 2; d++) {
			var_aux = var + d;
			if (var_aux >= tamanho_matriz || !filho[var_aux].empty())
				quebra = 1;
		}

		if (quebra != 1) {
			pequenos[0].pop_back();
			for (int d = 0; d < 2; d++) {
				var_aux = var + d;
				filho[var_aux] = pequenos[0] + variantes[d];
			}
			pequenos.erase(pequenos.begin()); // Apaga da lista o fardo j� alocado
		}
		j += 2;

		if (j == linhas)
			j = 0, i++;
		if (i == colunas)
			j = 0, i = 0;
	}
	return filho;
}


/*
Fun��es do algoritmo gen�tico
*/

void ga::init() {  // Inicializador de indiv�duos para a popula��o

	int idx = 0;
	vector<string> grandes, pequenos;

	for (unsigned int i = 0; i < info_fardos.size(); i++) {
		string tamanho = info_fardos[i].tamanho;
		int qtdade = info_fardos[i].qtdade;

		for (int j = 0; j < qtdade; j++) {
			if (tamanho == "pequeno")
				idx++, pequenos.push_back(to_string(idx) + 'a');
			else
				idx++, grandes.push_back(to_string(idx) + 'a');
		}
	}

	// Calculando o tamanho necess�rio da matriz para acomodar todos os fardos
	ga::tamanho_matriz = static_cast<int>(pequenos.size()) * 2 + static_cast<int>(grandes.size()) * 3;
	ga::colunas = tamanho_matriz / linhas;

	// Inicializando a popula��o com indiv�duos de tamanho pr�-definido
	string2d populacao(tamanho_populacao, vector<string>(tamanho_matriz, ""));

	srand(static_cast<unsigned int>(time(NULL)));

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {

		// Misturando a ordem de fardos alocados
		random_shuffle(pequenos.begin(), pequenos.end());
		random_shuffle(grandes.begin(), grandes.end());

		populacao[individuo] = __preenchimento(populacao[individuo], pequenos, grandes);
	}
	ga::populacao = populacao;
}

vector<double> ga::fitness() {  // Quantifica��o do desempenho da matriz

	const int classes = static_cast<const int>(info_fardos.size());  // Quantidade de categorias de fardos
	vector<double> valores(tamanho_populacao, 0.0);  // Valor fitness de cada indiv�duo

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {
		vector<double> ponderado(classes, 0.0);
		vector<vector<int>> localizacao(classes);  // Localiza��o de cada tipo de fardo na matriz

		for (int celula = 0; celula < tamanho_matriz; celula++) {
			int coluna = celula / linhas;
			int categoria = __categoria(populacao[individuo][celula]);

			// Condi��o para inserir colunas repetidas no vetor
			if (localizacao[categoria].size() == 0)  // Necess�rio pois n�o � poss�vel obter .back() de um vetor vazio
				localizacao[categoria].push_back(coluna);

			if (localizacao[categoria].back() != coluna)
				localizacao[categoria].push_back(coluna);

			// Armazenando espa�o (em colunas) ocupado por cada categoria
			if (populacao[individuo][celula].back() == 'a') {
				if (info_fardos[categoria].tamanho == "grande")
					ponderado[categoria] += 3.0;
				else
					ponderado[categoria] += 1.0;
			}
		}

		for (int categoria = 0; categoria < classes; categoria++) {
			if (localizacao[categoria].size() > 1) {  // H� dist�ncia para c�lculo apenas se o fardo ocupar mais de duas colunas

				double col = static_cast<double>(colunas);
				double espaco_ocupado = static_cast<double>(localizacao[categoria].size());

				// Penaliza��o por % de colunas ocupadas
				double peso = espaco_ocupado / min(col, ponderado[categoria]);

				for (int coluna_iterada = 0; coluna_iterada < localizacao[categoria].size() - 1; coluna_iterada++) {
					int proxima_coluna_iterada = coluna_iterada + 1;

					int distancia = localizacao[categoria][proxima_coluna_iterada] - localizacao[categoria][coluna_iterada];
					valores[individuo] += peso * __faixas(distancia);
				}
			}
		}
	}
	ga::fitval = valores;
	return valores;
}

int ga::selecao() {  // Torneios bin�rios de sele��o

	int vencedor = rand() % tamanho_populacao, desafiante = rand() % tamanho_populacao;

	while (vencedor == desafiante)
		desafiante = rand() % tamanho_populacao;

	if (fitval[desafiante] >= fitval[vencedor])  // Quem possuir MAIOR valor fitness, vence
		vencedor = desafiante;

	return vencedor;
}

void ga::cruzamento() {  // Order Crossover (OX)

	string2d linhagem;

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {

		vector<string> filho(tamanho_matriz, ""), pequenos, grandes;
		int pai = selecao(), mae = selecao();  // Genitores para a linhagem

		double numero_aleatorio = rand() / (double)RAND_MAX;
		if (numero_aleatorio >= probabilidade_cruzamento) {
			if (fitval[pai] >= fitval[mae])
				linhagem.push_back(populacao[pai]);
			else
				linhagem.push_back(populacao[mae]);
			continue;  // Pula a execu��o de cruzamento
		}

		// Popula o filho com a informa��o gen�tica da m�e na regi�o de corte
		limites cortes = __gerar_corte(colunas, mae);
		copy(populacao[mae].begin() + cortes.inf, populacao[mae].begin() + cortes.sup, filho.begin() + cortes.inf);

		for (string fardo : populacao[pai]) {
			if (fardo.back() == 'a') {  // Caso o fardo esteja em sua posi��o inicial,
				auto iterador = find(filho.begin() + cortes.inf, filho.begin() + cortes.sup, fardo);  // O fardo do pai j� est� alocado no filho?

				if (iterador == filho.begin() + cortes.sup) {  // Se n�o,
					if (info_fardos[__categoria(fardo)].tamanho == "pequeno")
						pequenos.push_back(fardo);
					else
						grandes.push_back(fardo);
				}
			}
		}
		// Popula o filho com a informa��o gen�tica do pai fora da regi�o de corte
		filho = __preenchimento(filho, pequenos, grandes, cortes.sup);

		linhagem.push_back(filho);
	}
	ga::populacao = linhagem;
}

void ga::mutacao() {  // Muta��o por troca (swap)

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {
		double numero_aleatorio = rand() / (double)RAND_MAX;

		if (numero_aleatorio <= probabilidade_mutacao) {
			limites cortes = __gerar_corte(colunas - bloco, individuo, true);

			while (__checar_limites(cortes.inf, individuo, true))
				cortes.inf += linhas;

			while (__checar_limites(cortes.sup, individuo, true))
				cortes.sup += linhas;

			// Swap nos intervalos dos cortes inferior e superior de acordo com o tamanho do bloco de troca
			const int tamanho = bloco * linhas;
			swap_ranges(populacao[individuo].begin() + cortes.inf, populacao[individuo].begin() + cortes.inf + tamanho, populacao[individuo].begin() + cortes.sup);
		}
	}
}
