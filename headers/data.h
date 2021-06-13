#pragma once
#include <vector>
#include <string>
#include "ga.h"


typedef struct {  // Armazenamento de input via planilha
    int box;
    int qtdade;
    std::string procedencia;
    std::string tamanho;
} __input;

typedef std::vector<__input> input_csv;

class Integrador
{
public:

	input_csv ler_csv();
    void escrever_csv(std::vector<std::string> vencedor, ga algoritmo);
};
