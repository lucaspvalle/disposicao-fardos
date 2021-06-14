#include "headers/ga.h"
#include <Windows.h>


int main() {

    FreeConsole();
    const int tamanho_geracao = 250;
    
    // Inicializando algoritmo genético
    ga algoritmo;
    algoritmo.init();

    for (int individuo = 0; individuo < tamanho_geracao; individuo++) {
        algoritmo.fitness();
        algoritmo.cruzamento();
        algoritmo.mutacao();
    }
    algoritmo.escrever_csv();

    return 0;
}