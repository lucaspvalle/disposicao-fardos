#include <Windows.h> //MessageBoxA
#include <chrono>
#include "ga.h"
#include "testes.h"
using namespace std;
using namespace chrono;

vector<planilha> ler_planilha(float criterio_peso) {
    //leitura de arquivo CSV com inputs de fardos

    ifstream arq;
    vector<planilha> inputFardos;
    string box, procedencia, peso, qtdade, tamanho;

    arq.open("temp.csv", ios::in);
    if (arq.is_open()) {

        //modelo do arquivo: "box,procedencia,peso,qtdade\n"
        while (getline(arq, box, ',')) {
            getline(arq, procedencia, ',');
            getline(arq, peso, ',');
            getline(arq, qtdade);

            if (stof(peso) < criterio_peso) //caso o peso do fardo seja menor, ele é classificado como pequeno
                tamanho = "pequeno";
            else //caso contrario, é classificado como grande
                tamanho = "grande";

            inputFardos.push_back({ stoi(box), stoi(qtdade), stof(peso), procedencia, tamanho }); //armazenando as informacoes
        }
        arq.close();
    }
    else { //caso o arquivo nao seja aberto,
        MessageBoxA(NULL, (LPCSTR)"Arquivo não encontrado!", (LPCSTR)"Disposição de Fardos", MB_ICONWARNING);
        exit(1);
    }
    return inputFardos;
}

void mapa(ga algoritmo) {
    //saida do mapa de disposicao de fardos

    ofstream arq;
    int melhor, tipo;

    //indice do individuo com maior valor fitness
    melhor = static_cast<int>(max_element(algoritmo.fitval.begin(), algoritmo.fitval.end()) - algoritmo.fitval.begin());
    
    arq.open("temp.csv", ios::trunc);
    if (arq.is_open()) {

        for (unsigned int i = 0; i < algoritmo.populacao[melhor].size(); i++) { //iterando todos os espacos da matriz do melhor individuo
            if (!algoritmo.populacao[melhor][i].empty()) { //caso o espaco esteja preenchido,

                tipo = algoritmo.categoria(algoritmo.populacao[melhor][i]); //obter a categoria do fardo

                if (algoritmo.populacao[melhor][i].back() == 'a') //se a identificao for "a" (primeira posicao do fardo,
                    arq << algoritmo.inputFardos[tipo].procedencia << " (" << algoritmo.inputFardos[tipo].box << "),"; //escrever a procedencia e o box do fardo
                
                else //caso contrario (se a identificacao for "b" ou "c"),
                    if (algoritmo.inputFardos[tipo].tamanho == "grande") //e o fardo seja grande,
                        arq << "2" << ','; //escrever "2" para identificacao posterior na elaboracao do mapa no excel
                    else //e o fardo seja pequeno,
                        arq << "1" << ','; //escrever "1" para identificacao posterior na elaboracao do mapa no excel
            }
            if (((i - 3) % algoritmo.linhas) == 0) //pular linha quando o mapa já estiver na 4ª linha da coluna
                arq << endl;
        }
    }
    arq.close();
}

relatorio sumario(int idx, int populacaoTam, int geracaoTam, double mutacaoProb, vector<planilha> inputFardos, unsigned int semente, int classes) {

    vector<double> fitval;
    double fit_in, fit_out;
    time_point<system_clock> comeco, fim; //cronometros

    comeco = system_clock::now(); //iniciando cronometro

    ga algoritmo(populacaoTam, mutacaoProb, inputFardos); //inicializando o algoritmo genético
    algoritmo.seed(semente); //semente para geracao de numeros aleatorios

    algoritmo.init(); //inicializando a populacao para evolucao
    fitval = algoritmo.fitness(classes); //avaliando a populacao inicializada

    fit_in = *max_element(fitval.begin(), fitval.end()); //valor fitness inicial do algoritmo
    
    for (int idx = 0; idx < geracaoTam; idx++) { //iteracao de geracoes
        fitval = algoritmo.fitness(classes);
        algoritmo.cruzamento();
        algoritmo.mutacao();
    }

    fit_out = *max_element(fitval.begin(), fitval.end()); //valor fitness final do algoritmo

    fim = system_clock::now(); //parando cronometro
    duration<double> segundos = fim - comeco; //calculando tempo de execucao

    cout << fit_out << " com diferenca de " << fit_out - fit_in << endl;

    //mapa(algoritmo);
    //MessageBoxA(NULL, (LPCSTR)"Algoritmo executado com sucesso!", (LPCSTR)"Disposição de Fardos", MB_ICONINFORMATION);
    return { idx, populacaoTam, geracaoTam, mutacaoProb, fit_in, fit_out, segundos.count() };
}

void testar() {

    vector<relatorio> saida;
    vector<grupos> parametros;
    vector<planilha> inputFardos;
    unsigned int semente = 0, rodada = 0;

    vector<double> mut = { 0.05 };
    vector<int> pop = { 500, 600, 700, 800, 900, 1000 }, grc = { 250 };

    testes Iniciador;
    parametros = Iniciador.combinador();

    for (unsigned int n = 0; n < parametros.size(); n++) {
        for (unsigned int inst = 0; inst < 2; inst++) {

            rodada++;
            cout << "Rodada " << rodada << endl;

            inputFardos = Iniciador.gerarInstancias(parametros[n].fardos, parametros[n].porcentagem, parametros[n].classes);

            for (unsigned int i = 0; i < pop.size(); i++)
                for (unsigned int j = 0; j < grc.size(); j++)
                    for (unsigned int k = 0; k < mut.size(); k++) {
                        semente++;
                        saida.push_back(sumario(rodada, pop[i], grc[j], mut[k], inputFardos, semente, parametros[n].classes)); //executando o algoritmo com os parametros testes
                    }
        }
    }
    Iniciador.resultados(saida);
}

int main() {

    //FreeConsole();  //fechar o prompt de comando durante a execucao
    srand(static_cast<unsigned int>(time(NULL)));

    //vector<planilha> inputFardos;
    //double mutacaoProb = 0.1;
    //int populacaoTam = 400, geracaoTam = 400;
    //float criterio_peso = 220; //classificacao de tamanhos de fardos

    //inputFardos = ler_planilha(criterio_peso); //leitura de planilha para input
    //sumario(0, populacaoTam, geracaoTam, mutacaoProb, inputFardos, static_cast<double>(time(NULL)), 100);

    testar();

    return 0;
}