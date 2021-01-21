#include "testes.h"

double testes::triangular(double a, double b, double c) {

    double U = rand() / (double)RAND_MAX;
    double F = (c - a) / (b - a);

    if (U <= F)
        return a + sqrt(U * (b - a) * (c - a));
    else
        return b - sqrt((1 - U) * (b - a) * (b - c));
}

vector<int> histograma(vector<double> cores, int classes) {

    vector<int> disc_cores;
    int max, min, alcance, largura, a, b;

    max = max_element(cores.begin(), cores.end()) - cores.begin(); //valor maximo da distribuicao de cores
    min = min_element(cores.begin(), cores.end()) - cores.begin(); //valor minimo da distribuicao de cores

    alcance = max - min;
    largura = alcance / classes;

    for (int cor = 0; cor < cores.size(); cor++) {
        a = min;
        for (int i = 0; i < classes; i++) {

            b = a + largura;

            if (cores[i] >= a && cores[i] <= b) {
                disc_cores.push_back(i);
                break;
            }
            a = b;
        }
    }
    return disc_cores;
}

vector<planilha> testes::gerarInstancias(int nivel_fardos, int nivel_proc, double proporcao, int classes) {

    vector<planilha> inputFardos;
    vector<double> cores;
    vector<int> disc_cores, estoques;

    int num_fardos, num_proc, qtd = 0, total = 0, a, b, pequenos, grandes;
    double cor = 0;

    default_random_engine generator;

    switch (nivel_fardos) {
    case 1:
        num_fardos = floor(triangular(30, 130, 160)); //numero de fardos a serem alocados
        break;
    case 2:
        num_fardos = floor(triangular(300, 1300, 1600)); //numero de fardos a serem alocados
        break;
    }
    
    //parametrizacao do nivel de procedencias
    switch (nivel_proc) {
    case 1:
        a = 60, b = 74;
        break;
    case 2:
        a = 45, b = 59;
        break;
    case 3:
        a = 30, b = 44;
        break;
    }

    uniform_int_distribution<int> uniDist(a, b); //parametrizando distribuicao uniforme para geracao de nivel de procedencias
    num_proc = uniDist(generator); //gerando nivel de procedencias

    for (int i = 0; i < num_proc; i++) {

        normal_distribution<int> normalDistEst(75.0, 49.5); //parametrizando distribuicao normal para geracao de estoques
        normal_distribution<double> normalDistCor(9.44, 0.938); //parametrizando distribuicao normal para geracao de atributo cor

        do { //loop ate que o numero aleatorio gerado para estoque satisfaca as condicoes
            qtd = normalDistEst(generator);
        } while (qtd < 1 || qtd > 165);

        estoques.push_back(qtd);
        total += qtd;

        do { //loop ate que o numero aleatorio gerado para cor satisfaca as condicoes
            cor = normalDistCor(generator);
        } while (cor < 7.2 || cor > 12.1);

        cores.push_back(cor);
    }
    disc_cores = histograma(cores, classes);

    for (int i = 0; i < num_proc; i++) {

        estoques[i] = (estoques[i] / total) * num_fardos;

        pequenos = floor(estoques[i] * proporcao);
        grandes = estoques[i] - pequenos;

        while (pequenos % 3 != 0)
            pequenos++;

        while (grandes % 2 != 0)
            grandes++;

        inputFardos.push_back({ 0, pequenos, 200, to_string(disc_cores[i]), "pequenos" });
        inputFardos.push_back({ 0, grandes, 230, to_string(disc_cores[i]), "grandes" });
    }
    return inputFardos;
}

void testes::parametros() {
    
    vector<int> populacaoTam = { 10, 100, 500, 1000 };
    vector<int> geracaoTam = { 10, 100, 500, 1000 };
    vector<double> mutacaoProb = { 0.005, 0.01, 0.05, 0.1 };

    for (int pop = 0; pop < populacaoTam.size(); pop++) {
        for (int ger = 0; ger < geracaoTam.size(); ger++) {
            for (int mut = 0; mut < mutacaoProb.size(); mut++) {
                for (int i = 0; i < 2; i++) {
                    continue;
                }
            }
        }
    }



}