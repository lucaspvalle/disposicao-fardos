#include "headers/ga.h"
#include <Windows.h>
#include <random>
#include <iostream>


int main() {

    FreeConsole();
    const int tamanho_geracao = 250;
    
    srand(static_cast<unsigned int>(time(NULL)));

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