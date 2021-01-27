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

vector<int> histograma(vector<double> cores, int classes) {
    //histograma para classificar os fardos em intervalos de acordo com a cor
    
    double max, min, largura, intervalo;
    vector<int> bins; //vetor com as cores divididas em classes

    max = *max_element(cores.begin(), cores.end()) + 0.000001; //valor MAXIMO da distribuicao de cores
    min = *min_element(cores.begin(), cores.end()); //valor MINIMO da distribuicao de cores

    largura = ((max - min) / (double)classes); //range de cada intervalo

    for (unsigned int cor = 0; cor < cores.size(); cor++) { //iterando cada cor para definir sua classe pertencente
        for (int i = 0; i < classes; i++) { //iterando as classes para cada cor, até encontrar a pertencente

            intervalo = min + i * largura;
            if (cores[cor] >= intervalo && cores[cor] < intervalo + largura) { //se o valor de cor estiver dentro do intervalo, continuar
                bins.push_back(i);
                break;
            }
        }
    }
    return bins;
}

vector<planilha> testes::gerarInstancias(int nivel_fardos, int nivel_proc, double porcentagem, int classes) {
    //geracao aleatoria de instancias para simular o conjunto de dados

    vector<int> bins;
    vector<planilha> inputFardos;
    vector<instancia> instancias;
    vector<double> estoques, cores, proporcao(classes, 0);

    double cor = 0, total = 0, alocacao;
    int num_fardos, num_proc, qtd = 0, a, b, pequenos = 0, grandes = 0, p, g, var;

    default_random_engine generator; //mecanismo de geracao de numeros aleatorios

    switch (nivel_fardos) { //parametrizando o nivel de fardos a serem alocados
    case 1: //nivel normal
        num_fardos = triangular(30, 160, 130);
        break;
    case 2: //nivel ampliado
        num_fardos = triangular(300, 1600, 1300);
        break;
    }
    
    switch (nivel_proc) { //parametrizando o nivel de procedencias
    case 1: //nivel alto
        a = 60, b = 74;
        break;
    case 2: //nivel medio
        a = 45, b = 59;
        break;
    case 3: //nivel baixo
        a = 30, b = 44;
        break;
    }
    uniform_int_distribution<int> uniDist(a, b); //definindo distribuicao uniforme para o nivel de procedencias
    num_proc = uniDist(generator); //gerando numero aleatorio para o nivel de procedencias

    for (int i = 0; i < num_proc; i++) { //iterando cada procedencia

        normal_distribution<double> normalDistEst(75.0, 49.5); //distribuicao normal para geracao de estoques
        normal_distribution<double> normalDistCor(9.44, 0.938); //distribuicao normal para geracao de atributo cor

        do { //loop ate que o numero aleatorio gerado para estoque satisfaca as condicoes
            qtd = static_cast<int>(floor(normalDistEst(generator)));
        } while (qtd < 1 || qtd > 165);
        estoques.push_back(qtd); //armazenando a quantidade de estoque
        total += qtd; //armazenando o total de fardos em estoque

        do { //loop ate que o numero aleatorio gerado para cor satisfaca as condicoes
            cor = normalDistCor(generator);
        } while (cor < 7.2 || cor > 12.1);
        cores.push_back(cor); //armazenando as cores
    }
    bins = histograma(cores, classes); //dividindo as cores em classes

    for (int i = 0; i < num_proc; i++) //proporcao (em %) dos fardos de acordo com a quantidade a ser alocada, agrupados por classes
        proporcao[bins[i]] += estoques[i] / total;

    for (int i = 0; i < classes; i++) {
        alocacao = proporcao[i] * num_fardos; //quantidade de fardos a serem alocados por classes

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
    vector<double> porcentagem = { 0.25, 0.50, 0.75 };
    vector<int> fardos = { 1 }, proc = { 1, 2, 3 }, classes = { 5, 10, 15 };

    for (int i = 0; i < fardos.size(); i++)
        for (int j = 0; j < proc.size(); j++)
            for (int k = 0; k < porcentagem.size(); k++)
                for (int l = 0; l < classes.size(); l++)
                    parametros.push_back({ fardos[i], proc[j], classes[l], porcentagem[k] });

    return parametros;
}
