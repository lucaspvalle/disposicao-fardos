#include "../headers/data.h"
#include <fstream>
#include <Windows.h>


input_csv Integrador::ler_csv() {  // Entrada: Informa��es de Fardos

    std::ifstream arquivo;

    arquivo.open("data/input.csv", std::ios::in);
    if (arquivo.is_open()) {

        input_csv info_fardos;
        const double criterio_peso = 220;

        // Modelo do arquivo: "box,procedencia,peso,qtdade\n"
        std::string box, procedencia, peso, qtdade, tamanho;

        while (getline(arquivo, box, ',')) {
            getline(arquivo, procedencia, ',');
            getline(arquivo, peso, ',');
            getline(arquivo, qtdade);
            
            // Regra: caso o peso seja menor do que o crit�rio, � classificado como pequeno
            if (stod(peso) < criterio_peso)
                tamanho = "pequeno";
            else
                tamanho = "grande";

            info_fardos.push_back({ stoi(box), stoi(qtdade), procedencia, tamanho });
        }
        arquivo.close();

        return info_fardos;
    }
    else {
        MessageBoxA(NULL, (LPCSTR)"Arquivo n�o encontrado!", (LPCSTR)"Disposi��o de Fardos", MB_ICONWARNING);
        exit(1);
    }
}

void Integrador::escrever_csv(std::vector<std::string> vencedor, ga algoritmo) {  // Sa�da: Mapa de Disposi��o de Fardos

    std::ofstream arquivo;

    arquivo.open("data/output.csv", std::ios::trunc);
    if (arquivo.is_open()) {

        int posicao = 0;
        for (std::string fardo : vencedor) {  // Iterando c�lula a c�lula da matriz
            if (!fardo.empty()) {

                int categoria = algoritmo.__categoria(fardo);

                std::string procedencia = algoritmo.info_fardos[categoria].procedencia;
                int box = algoritmo.info_fardos[categoria].box;
                std::string tamanho = algoritmo.info_fardos[categoria].tamanho;

                if (celula.back() == 'a') {  // Escrever apenas para a primeira ocorr�ncia do fardo na matriz
                    arquivo << procedencia << " (" << box << "),";
                }
                else {  // Identifica��o do tamanho do fardo para a constru��o do mapa em Excel
                    if (tamanho == "grande")
                        arquivo << "2" << ',';
                    else
                        arquivo << "1" << ',';
                }
            }
            if (((posicao - 3) % algoritmo.linhas) == 0)  // Quebra de linha do arquivo quando estiver na 4� linha da matriz
                arquivo << "\n";

            posicao++;
        }
        arquivo.close();
    }
    else {
        MessageBoxA(NULL, (LPCSTR)"Arquivo n�o encontrado!", (LPCSTR)"Disposi��o de Fardos", MB_ICONWARNING);
        exit(1);
    }
}