#include "../headers/ga.h"
#include <algorithm>
#include <random>
#include <fstream>
#include <Windows.h>
using namespace std;


/*
Funções de integração
*/

bool ga::__ler_csv(string diretorio) {

	ifstream arquivo;

	arquivo.open(diretorio, ios::in);
	if (arquivo.is_open()) {

		info info_fardos;
		const double criterio_peso = 220.0;

		// Modelo do arquivo: "qtdade,procedencia,peso,box,cor\n"
		string integridade, box, procedencia, peso, qtdade, tamanho, cor;
		bool fantasma = false;

		// Confere se a estrutura do arquivo está correta
		getline(arquivo, integridade, ',');
		if (integridade != "Quantidade") {
			MessageBoxA(
				NULL,
				(LPCSTR)"A estrutura do arquivo de entrada não corresponde ao esperado!\nProcedimento desabilitado.",
				(LPCSTR)"Disposição de Fardos",
				MB_ICONWARNING);
			return false;
		}

		// Deleta o cabeçalho
		getline(arquivo, integridade);

		while (getline(arquivo, qtdade, ',')) {
			getline(arquivo, procedencia, ',');
			getline(arquivo, peso, ',');
			getline(arquivo, box, ',');
			getline(arquivo, cor);

			// Regra: caso o peso seja menor do que o critério, é classificado como pequeno
			if (stod(peso) < criterio_peso)
				tamanho = "pequeno";
			else
				tamanho = "grande";

			info_fardos.push_back({ stoi(qtdade), box, procedencia, tamanho, cor, fantasma });
		}
		arquivo.close();

		ga::fardos = info_fardos;
		return true;
	}
	else {
		MessageBoxA(NULL, (LPCSTR)"Arquivo não encontrado!", (LPCSTR)"Disposição de Fardos", MB_ICONWARNING);
		return false;
	}
}

void ga::escrever_csv(string diretorio) {

	const int idx_maior_fitness = static_cast<const int>(max_element(fitval.begin(), fitval.end()) - fitval.begin());
	vector<string> vencedor = populacao[idx_maior_fitness];

	ofstream arquivo;

	arquivo.open(diretorio, ios::trunc);
	if (arquivo.is_open()) {

		int posicao = 0;
		for (string fardo : vencedor) {  // Iterando célula a célula da matriz
			if (!fardo.empty()) {

				int categoria = __categoria(fardo);

				string procedencia = fardos[categoria].procedencia;
				string box = fardos[categoria].box;
				string tamanho = fardos[categoria].tamanho;
				string cor = fardos[categoria].cor;
				bool fantasma = fardos[categoria].fantasma;

				if (fardo.back() == 'a') {  // Escrever apenas para a primeira ocorrência do fardo na matriz
					if (!fantasma)
						arquivo << procedencia << " (" << cor << "),";
					else
						arquivo << "NDA,";
				}
				else {  // Identificação do tamanho do fardo para a construção do mapa em Excel
					if (tamanho == "grande")
						arquivo << "2" << ',';
					else
						arquivo << "1" << ',';
				}
			}
			if (((posicao - 3) % linhas) == 0)  // Quebra de linha do arquivo quando estiver na 4ª linha da matriz
				arquivo << "\n";

			posicao++;
		}
		arquivo.close();
	}
	else {
		MessageBoxA(NULL, (LPCSTR)"Arquivo não encontrado!", (LPCSTR)"Disposição de Fardos", MB_ICONWARNING);
	}
	MessageBoxA(NULL, (LPCSTR)"Algoritmo executado com sucesso!", (LPCSTR)"Disposição de Fardos", MB_ICONINFORMATION);
}


/*
Funções de apoio
*/

int ga::__categoria(string fardo) {

	// Exemplo de string: "123b"
	fardo.pop_back();  // Retira o último caracter ("b")
	int id = stoi(fardo);  // Converte a string para inteiro

	int cumsum = 0;
	for (unsigned int tipo = 0; tipo < fardos.size(); tipo++) {
		cumsum += fardos[tipo].qtdade;

		if (id <= cumsum)
			return tipo;
	}
	return 0;
}

double ga::__faixas(int distancia) {

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

bool ga::__verifica_limites(int corte, int chr, bool checar_bloco = false) {

	if (!checar_bloco) {  // Não verificar blocos
		for (int i = 0; i < linhas; i++) {
			int celula = corte + i;

			char letra = populacao[chr][celula].back();
			string tamanho = fardos[__categoria(populacao[chr][celula])].tamanho;

			// Regra: se o fardo grande estiver em sua posição inicial ("a"), ele NÃO está sendo cortado
			if ((tamanho == "grande") && (letra != 'a'))
				return true;
		}
	}
	else {  // Verificar blocos
		for (int col = 0; col < bloco; col++) {
			for (int i = 0; i < linhas; i++) {
				int celula = corte + col * linhas + i;

				string tamanho_fardo_na_iteracao = fardos[__categoria(populacao[chr][celula])].tamanho;
				string tamanho_fardo_no_corte = fardos[__categoria(populacao[chr][corte])].tamanho;

				// Regra: como um fardo grande possui o mesmo tamanho do bloco,
				// um fardo pequeno na mesma coluna mostra que o fardo grande está sendo cortado
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

vector<string> ga::__preenchimento(vector<string> filho, vector<string> pequenos, vector<string> grandes, int corte = 0) {

	string variantes = { "abc" };
	int coluna_iterada = corte / linhas, linha_iterada = 0;

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

void ga::init() {

	int idx = 0;
	vector<string> grandes, pequenos;

	for (unsigned int i = 0; i < fardos.size(); i++) {
		string tamanho = fardos[i].tamanho;
		int qtdade = fardos[i].qtdade;

		for (int j = 0; j < qtdade; j++) {
			if (tamanho == "pequeno")
				idx++, pequenos.push_back(to_string(idx) + 'a');
			else
				idx++, grandes.push_back(to_string(idx) + 'a');
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
		fardos.push_back({ pequenos_fantasmas, "NDA", "NDA", "pequeno", "NDA", true });

		for (int j = 0; j < pequenos_fantasmas; j++)
				idx++, pequenos.push_back(to_string(idx) + 'a');
	}

	if (grandes_fantasmas > 0) {
		fardos.push_back({ grandes_fantasmas, "NDA", "NDA", "grande", "NDA", true });

		for (int j = 0; j < grandes_fantasmas; j++)
				idx++, grandes.push_back(to_string(idx) + 'a');
	}

	// Calculando o tamanho necessário da matriz para acomodar todos os fardos
	ga::tamanho_matriz = contagem_pequenos * 2 + contagem_grandes * 3;
	ga::colunas = static_cast<int>(ceil(tamanho_matriz / linhas));

	// Inicializando a população com indivíduos de tamanho pré-definido
	string2d populacao(tamanho_populacao, vector<string>(tamanho_matriz, ""));

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {

		// Misturando a ordem de fardos alocados
		random_shuffle(pequenos.begin(), pequenos.end());
		random_shuffle(grandes.begin(), grandes.end());

		populacao[individuo] = __preenchimento(populacao[individuo], pequenos, grandes);
	}
	ga::populacao = populacao;
}

void ga::fitness() {

	const int classes = static_cast<const int>(fardos.size());  // Quantidade de categorias de fardos
	vector<double> valores(tamanho_populacao, 0.0);  // Valor fitness de cada indivíduo

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
				if (fardos[categoria].tamanho == "grande")
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
	ga::fitval = valores;
}

int ga::selecao() {

	int vencedor = rand() % tamanho_populacao, desafiante = rand() % tamanho_populacao;

	while (vencedor == desafiante)
		desafiante = rand() % tamanho_populacao;

	if (fitval[desafiante] >= fitval[vencedor])  // Quem possuir MAIOR valor fitness, vence
		vencedor = desafiante;

	return vencedor;
}

void ga::cruzamento() {

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
			continue;  // Pula a execução de cruzamento
		}

		// Popula o filho com a informação genética da mãe na região de corte
		limites cortes = __gerar_corte(colunas, mae);
		copy(populacao[mae].begin() + cortes.inf, populacao[mae].begin() + cortes.sup, filho.begin() + cortes.inf);

		for (string fardo : populacao[pai]) {
			if (fardo.back() == 'a') {  // Caso o fardo esteja em sua posição inicial,
				auto iterador = find(filho.begin() + cortes.inf, filho.begin() + cortes.sup, fardo);  // O fardo do pai já está alocado no filho?

				if (iterador == filho.begin() + cortes.sup) {  // Se não,
					if (fardos[__categoria(fardo)].tamanho == "pequeno")
						pequenos.push_back(fardo);
					else
						grandes.push_back(fardo);
				}
			}
		}
		// Popula o filho com a informação genética do pai fora da região de corte
		filho = __preenchimento(filho, pequenos, grandes, cortes.sup);

		linhagem.push_back(filho);
	}
	ga::populacao = linhagem;
}

void ga::mutacao() {

	for (int individuo = 0; individuo < tamanho_populacao; individuo++) {
		double numero_aleatorio = rand() / (double)RAND_MAX;

		if (numero_aleatorio <= probabilidade_mutacao) {
			limites cortes = __gerar_corte(colunas - bloco, individuo, true);

			while (__verifica_limites(cortes.inf, individuo, true))
				cortes.inf += linhas;

			while (__verifica_limites(cortes.sup, individuo, true))
				cortes.sup += linhas;

			// Swap nos intervalos dos cortes inferior e superior de acordo com o tamanho do bloco de troca
			const int tamanho = bloco * linhas;
			swap_ranges(populacao[individuo].begin() + cortes.inf, populacao[individuo].begin() + cortes.inf + tamanho, populacao[individuo].begin() + cortes.sup);
		}
	}
}
