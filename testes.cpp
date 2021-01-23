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
    
    double max, min, largura, a, b;
    vector<int> bins; //vetor com as cores divididas em classes

    max = *max_element(cores.begin(), cores.end()); //valor MAXIMO da distribuicao de cores
    min = *min_element(cores.begin(), cores.end()); //valor MINIMO da distribuicao de cores

    largura = (max - min) / (double)classes; //range de cada intervalo

    for (int cor = 0; cor < cores.size(); cor++) { //iterando cada cor para definir sua classe pertencente

        a = min; //comecando a iteracao no primeiro intervalo (valor minimo ate o valor minimo + largura, e assim vai...)
        for (int i = 0; i < classes; i++) { //iterando as classes para cada cor, até encontrar a pertencente

            b = a + largura;
            
            if (cores[cor] >= a && cores[cor] <= b) { //se o valor de cor estiver dentro do intervalo, continuar
                bins.push_back(i);
                break;
            }
            a = b;
        }
    }
    return bins;
}

vector<planilha> testes::gerarInstancias(int nivel_fardos, int nivel_proc, double proporcao, int classes) {

    vector<planilha> inputFardos;
    vector<double> estoques, cores;
    vector<int> bins;

    int num_fardos, num_proc, qtd = 0, total = 0, a, b, pequenos, grandes;
    double cor = 0;

    default_random_engine generator; //mecanismo de geracao de numeros aleatorios

    switch (nivel_fardos) { //parametrizando o nivel de fardos a serem alocados
    case 1: //nivel normal
        num_fardos = floor(triangular(30, 130, 160));
        break;
    case 2: //nivel ampliado
        num_fardos = floor(triangular(300, 1300, 1600));
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
            qtd = floor(normalDistEst(generator));
        } while (qtd < 1 || qtd > 165);
        estoques.push_back(qtd); //armazenando a quantidade de estoque
        total += qtd; //armazenando o total de fardos em estoque

        do { //loop ate que o numero aleatorio gerado para cor satisfaca as condicoes
            cor = normalDistCor(generator);
        } while (cor < 7.2 || cor > 12.1);
        cores.push_back(cor); //armazenando as cores
    }
    bins = histograma(cores, classes); //dividindo as cores em classes

    for (int i = 0; i < num_proc; i++) { //iterando cada procedencia novamente, depois de dividir as cores em classes

        estoques[i] = floor((estoques[i] / total) * num_fardos); //atualizando o estoque para ser proporcional à qtdade que deve ser alocada na linha

        pequenos = floor(estoques[i] * proporcao); //obtendo a quantidade de fardos pequenos de acordo com a proporcao (25%, 50% ou 75%)
        grandes = estoques[i] - pequenos; //o mesmo para os fardos grandes

        while (pequenos % 3 != 0) //corrigindo a quantidade de fardos pequenos para o melhor aproveitamento da linha de abertura
            pequenos++;

        while (grandes % 2 != 0) //corrigindo a quantidade de fardos grandes para o melhor aproveitamento da linha de abertura
            grandes++;

        inputFardos.push_back({ 0, pequenos, 200, to_string(bins[i]), "pequenos" }); //input dos fardos pequenos para o algoritmo
        inputFardos.push_back({ 0, grandes, 230, to_string(bins[i]), "grandes" }); //input dos fardos grandes para o algoritmo
    }
    return inputFardos;
}

void testes::principal(int populacaoTam, int geracaoTam, double mutacaoProb) {

    ofstream arq;
    vector<planilha> inputFardos;
    vector<int> fitval;

    chrono::time_point<chrono::system_clock> comeco, fim; //cronometros
    srand(static_cast<unsigned int>(time(NULL))); //semente para geracao de numeros aleatorios

    /*
    Parametrização das instâncias
    */

    vector<int> nivel_fardos = { 1, 2 }, nivel_proc = { 1, 2, 3 }, proporcao = { 25, 50, 75 }, classes = { 2, 5, 10 };
    
    int f = nivel_fardos[rand() % nivel_fardos.size()]; //numero aleatorio de 0-1 para definir o nivel de fardos
    int proc = nivel_proc[rand() % nivel_proc.size()]; //numero aleatorio de 0-2 para definir o nivel de procedencias
    double prop = (double)proporcao[rand() % proporcao.size()] / 100; //numero aleatorio de 0-2 para definir a proporcao, em %
    int k = classes[rand() % classes.size()]; //numero aleatorio de 0-2 para definir as classes do histograma

    inputFardos = gerarInstancias(f, proc, prop, k); //simulando uma instancia de entrada para o algoritmo

    /*
    Inicialização do algoritmo
    */

    ga algoritmo(populacaoTam, mutacaoProb, inputFardos); //inicializando o algoritmo genético
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

    arq.open("resultados.csv", ios::out); //arquivo com os resultados dos testes
    if (arq.is_open()) { //se aberto, escrever os parametros
        arq << populacaoTam << ',' << geracaoTam << ',' << mutacaoProb << ',' << inicial << ',' << final << ',' << tempo << ',' << endl;
    }
}

void testes::parametros() {
    
    vector<int> populacaoTam = { 10, 100, 500, 1000 }; //parametros de tamanho da populacao a serem testados
    vector<int> geracaoTam = { 10, 100, 500, 1000 }; //parametros de tamanho da geracao a serem testados
    vector<double> mutacaoProb = { 0.005, 0.01, 0.05, 0.1 }; //parametros de probabilidade de mutacao a serem testados

    for (int i = 0; i < populacaoTam.size(); i++)
        for (int j = 0; j < geracaoTam.size(); j++)
            for (int k = 0; k < mutacaoProb.size(); k++)
                for (int l = 0; l < 2; l++)
                    principal(populacaoTam[i], geracaoTam[j], mutacaoProb[k]); //executando o algoritmo com os parametros testes
}