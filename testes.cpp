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

vector<planilha> testes::gerarInstancias(int nivel_fardos, int nivel_proc, double proporcao, int classes) {
    //geracao aleatoria de instancias para simular o conjunto de dados

    vector<int> bins;
    vector<planilha> inputFardos;
    vector<instancia> instancias;
    vector<double> estoques, cores, participacao(classes, 0);

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
        participacao[bins[i]] += estoques[i] / total;

    for (int i = 0; i < classes; i++) {
        alocacao = participacao[i] * num_fardos; //quantidade de fardos a serem alocados por classes

        //quantidade de fardos pequenos, de acordo com a proporcao (em %), e de grandes
        p = static_cast<int>(floor(alocacao * proporcao)), g = static_cast<int>(floor(alocacao - p));
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

void testes::principal(int populacaoTam, int geracaoTam, double mutacaoProb, unsigned int semente) {

    ofstream arq;
    vector<planilha> inputFardos;
    vector<int> fitval;

    chrono::time_point<chrono::system_clock> comeco, fim; //cronometros
    srand(static_cast<unsigned int>(time(NULL))); //semente para geracao de numeros aleatorios

    /*
    Parametrização das instâncias
    */

    //vector<int> nivel_fardos = { 1, 2 }, nivel_proc = { 1, 2, 3 }, proporcao = { 25, 50, 75 }, classes = { 2, 5, 10 };
    vector<int> nivel_fardos = { 1 }, nivel_proc = { 1, 2, 3 }, proporcao = { 25, 50, 75 }, classes = { 2, 5, 10 };

    int f = nivel_fardos[rand() % nivel_fardos.size()]; //numero aleatorio de 0-1 para definir o nivel de fardos
    int proc = nivel_proc[rand() % nivel_proc.size()]; //numero aleatorio de 0-2 para definir o nivel de procedencias
    double prop = (double)proporcao[rand() % proporcao.size()] / 100; //numero aleatorio de 0-2 para definir a proporcao, em %
    int k = classes[rand() % classes.size()]; //numero aleatorio de 0-2 para definir as classes do histograma

    inputFardos = gerarInstancias(f, proc, prop, k); //simulando uma instancia de entrada para o algoritmo

    /*
    Inicialização do algoritmo
    */

    ga algoritmo(populacaoTam, mutacaoProb, inputFardos); //inicializando o algoritmo genético
    algoritmo.seed(semente);

    algoritmo.init(); //inicializando a populacao para evolucao
    fitval = algoritmo.fitness(); //avaliando a populacao inicializada
    int inicial = *max_element(fitval.begin(), fitval.end()); //valor fitness inicial da heuristica construtiva

    /*
    Evolução
    */

    comeco = chrono::system_clock::now(); //iniciando cronometro
    for (int idx = 0; idx < geracaoTam; idx++) { //iteracao de geracoes

        fitval = algoritmo.fitness();
        algoritmo.cruzamento();
        algoritmo.mutacao();
    }
    fim = chrono::system_clock::now(); //parando cronometro

    fitval = algoritmo.fitness();
    int final = *max_element(fitval.begin(), fitval.end()); //valor fitness final do algoritmo

    /*
    Cronômetro
    */

    chrono::duration<double> segundos = fim - comeco; //calculando tempo de execucao
    double tempo = segundos.count();

    arq.open("resultados.csv", ios::app); //arquivo com os resultados dos testes
    if (arq.is_open()) { //se aberto, escrever os parametros
        arq << populacaoTam << ',' << geracaoTam << ',' << mutacaoProb << ',' << inicial << ',' << final << ',' << tempo << ',' << endl;
    }
    arq.close();
}

void testes::parametros() {

    /*
    Parâmetros testados
    */

    vector<double> mutacaoProb = { 0.005, 0.01, 0.05, 0.1 };
    vector<int> populacaoTam = { 10, 100, 200 }, geracaoTam = { 10, 100, 200 };
    
    unsigned int semente = 0;

    //executando o algoritmo para todas as combinacoes de parametros
    for (unsigned int i = 0; i < populacaoTam.size(); i++)
        for (unsigned int j = 0; j < geracaoTam.size(); j++)
            for (unsigned int k = 0; k < mutacaoProb.size(); k++)
                for (unsigned int l = 0; l < 2; l++) {
                    semente++;
                    srand(static_cast<unsigned int>(time(NULL)) * semente);
                    principal(populacaoTam[i], geracaoTam[j], mutacaoProb[k], semente); //executando o algoritmo com os parametros testes
                }
}