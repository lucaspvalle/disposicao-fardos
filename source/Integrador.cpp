#include "../headers/framework.h"
using namespace std;

InputsFardos Integrador::input_csv() {

	ifstream arquivo;

	arquivo.open(dir_entrada, ios::in);
	if (arquivo.is_open()) {

		InputsFardos DadosFardos;
		const double criterio_peso = 220.0;

		// Modelo do arquivo: "qtdade,procedencia,peso,box,cor\n"
		string integridade, box, procedencia, peso, qtdade, cor;
		bool is_grande, fantasma = false;

		// Confere se a estrutura do arquivo está correta
		getline(arquivo, integridade);
		if (integridade.substr(0, 10) != "Quantidade") {
			MessageBoxA(
				NULL,
				(LPCSTR)"A estrutura do arquivo de entrada não corresponde ao esperado!\nProcedimento desabilitado.",
				(LPCSTR)"Disposição de Fardos",
				MB_ICONWARNING);
			return InputsFardos{};
		}

		while (getline(arquivo, qtdade, ',')) {
			getline(arquivo, procedencia, ',');
			getline(arquivo, peso, ',');
			getline(arquivo, box, ',');
			getline(arquivo, cor);

			// Regra: caso o peso seja menor do que o critério, é classificado como pequeno
			if (stod(peso) < criterio_peso)
				is_grande = false;
			else
				is_grande = true;

			DadosFardos.push_back({ box, procedencia, cor, stoi(qtdade), is_grande, fantasma });
		}
		arquivo.close();

		return DadosFardos;
	}
	else {
		MessageBoxA(NULL, (LPCSTR)"Arquivo não encontrado!", (LPCSTR)"Disposição de Fardos", MB_ICONWARNING);
		return InputsFardos{};
	}
}

void Integrador::output_csv(vector<string> vencedor, InputsFardos fardos) {

	ofstream arquivo;

	arquivo.open(dir_saida, ios::trunc);
	if (arquivo.is_open()) {

		int posicao = 0;
		for (string fardo : vencedor) {  // Iterando célula a célula da matriz

			int categoria = AlgoritmoGenetico::__categoria(fardo);

			string procedencia = fardos[categoria].procedencia;
			string box = fardos[categoria].box;
			string cor = fardos[categoria].cor;
			bool is_grande = fardos[categoria].is_grande;
			bool fantasma = fardos[categoria].fantasma;

			if (fardo.back() == 'a') {  // Escrever apenas para a primeira ocorrência do fardo na matriz
				if (!fantasma)
					arquivo << procedencia << " (" << cor << "),";
				else
					arquivo << "NDA,";
			}
			else {  // Identificação do tamanho do fardo para a construção do mapa em Excel
				if (is_grande)
					arquivo << "2" << ',';
				else
					arquivo << "1" << ',';
			}

			if (((posicao - 3) % AlgoritmoGenetico::linhas) == 0)  // Quebra de linha do arquivo quando estiver na 4ª linha da matriz
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


