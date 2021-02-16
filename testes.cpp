#include "testes.h"

int triangular(double minn, double maxx, double moda) {
    //geracao de numeros aleatorios com distribuicao triangular

    double U = ((double)rand()) / RAND_MAX;
    double F = (moda - minn) / (maxx - minn);

    if (U <= F)
        return static_cast<int>(floor(moda + sqrt(U * (maxx - minn) * (moda - minn))));
    else
        return static_cast<int>(floor(maxx - sqrt((1 - U) * (maxx - minn) * (maxx - moda))));
}

vector<planilha> testes::gerarInstancias(int fardos, int pct, int classes) {
    //geracao aleatoria de instancias para simular o conjunto de dados

    vector<planilha> inputFardos;
    vector<instancia> instancias;
    vector<double> estoques;

    double alocacao, porcentagem = static_cast<double>(pct) / 100;
    int num_fardos, qtd = 0, total = 0, pequenos = 0, grandes = 0, p, g, var;

    default_random_engine generator; //mecanismo de geracao de numeros aleatorios

    switch (fardos) { //parametrizando o nivel de fardos a serem alocados
    case 1: //nivel normal
        num_fardos = triangular(30, 160, 130);
        break;
    case 2: //nivel ampliado
        num_fardos = triangular(300, 1600, 1300);
        break;
    }

    for (int i = 0; i < classes; i++) {

        normal_distribution<double> normalDistEst(75.0, 49.5); //distribuicao normal para geracao de estoques
        do {
            qtd = static_cast<int>(floor(normalDistEst(generator)));
        } while (qtd < 1 || qtd > 165);

        estoques.push_back(qtd);
        total += qtd;
    }

    for (int i = 0; i < classes; i++) {
        alocacao = estoques[i] * static_cast<double>(num_fardos) / static_cast<double>(total); //quantidade de fardos a serem alocados por classes

        //quantidade de fardos pequenos, de acordo com a porcentagem (em %), e de grandes
        p = static_cast<int>(floor(alocacao * porcentagem)), g = static_cast<int>(floor(alocacao - p));
        pequenos += p, grandes += g; //total de fardos pequenos e grandes

        instancias.push_back({ i, p, g }); //armazenando as quantidades por classe
    }

    while (pequenos % 3 != 0 || (pequenos / 3) % 2 != 0) { //corrigindo a quantidade de fardos pequenos para o melhor aproveitamento da linha de abertura
        var = rand() % instancias.size(); //gerando um indice aleatorio para corrigir a quantidade
        instancias[var].pequenos++, pequenos++;
    }

    while (grandes % 2 != 0 || (grandes / 2) % 2 != 0) { //corrigindo a quantidade de fardos grandes para o melhor aproveitamento da linha de abertura
        var = rand() % instancias.size(); //gerando um indice aleatorio para corrigir a quantidade
        instancias[var].grandes++, grandes++;
    }

    for (int i = 0; i < classes; i++) { //transformando a instancia na estrutura utilizada pelo algoritmo
        inputFardos.push_back({ i, instancias[i].pequenos, 200, to_string(i), "pequeno" });
        inputFardos.push_back({ i, instancias[i].grandes, 230, to_string(i), "grande" });
    }
    return inputFardos;
}

vector<grupos> testes::combinador() {

    vector<grupos> parametros;
    vector<int> fardos = { 1, 2 }, classes = { 5, 10, 15 }, porcentagem = { 25, 50, 75 };

    for (int nivel = 0; nivel < fardos.size(); nivel++)
        for (int tipo = 0; tipo < classes.size(); tipo++)
            for (int pct = 0; pct < porcentagem.size(); pct++)
                parametros.push_back({ fardos[nivel], classes[tipo], porcentagem[pct] });

    return parametros;
}

void testes::resultados(vector<relatorio> saida) {

    ofstream arq;
    arq.open("resultados.csv", ios::out); //arquivo com os resultados dos testes

    if (arq.is_open()) { //se aberto, escrever os parametros
        for (int i = 0; i < saida.size(); i++) {
            arq << saida[i].idx << ',' << saida[i].pop << ',' << saida[i].grc << ',' << saida[i].mut << ',' << saida[i].fit_in << ',' << saida[i].fit_out << ',' << saida[i].tempo << endl;
        }
    }
    arq.close();
}