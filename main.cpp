#include "headers/ga.h"
#include "headers/data.h"
#include <Windows.h>
#include <algorithm>


int main() {

    //FreeConsole();
    const int tamanho_geracao = 250;
    
    Integrador Data;
    input_csv info_fardos = Data.ler_csv();

    // Inicializando algoritmo genético
    ga algoritmo(info_fardos);
    std::vector<double> fitval = algoritmo.fitness();

    for (int individuo = 0; individuo < tamanho_geracao; individuo++) {
        fitval = algoritmo.fitness();
        algoritmo.cruzamento();
        algoritmo.mutacao();
    }

    const int idx_maior_fitness = static_cast<const int>(max_element(algoritmo.fitval.begin(), algoritmo.fitval.end()) - algoritmo.fitval.begin());
    std::vector<std::string> vencedor = algoritmo.populacao[idx_maior_fitness];

    Data.escrever_csv(vencedor, algoritmo);

    MessageBoxA(NULL, (LPCSTR)"Algoritmo executado com sucesso!", (LPCSTR)"Disposição de Fardos", MB_ICONINFORMATION);

    return 0;
}