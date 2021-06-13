#include "headers/ga.h"


int main() {

    //FreeConsole();
    const int tamanho_geracao = 250;
    
    ga algoritmo;  // Inicializando algoritmo genético
    std::vector<double> fitval = algoritmo.fitness();

    for (int individuo = 0; individuo < tamanho_geracao; individuo++) {
        fitval = algoritmo.fitness();
        algoritmo.cruzamento();
        algoritmo.mutacao();
    }
    algoritmo.escrever_csv();

    return 0;
}