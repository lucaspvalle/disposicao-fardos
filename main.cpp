#include <fstream> //std::open
#include <Windows.h> //MessageBoxA
#include <chrono>
#include "ga.h"
#include "testes.h"
using namespace std;

vector<planilha> ler_planilha(int criterio_peso) {
    //leitura de arquivo CSV com inputs de fardos

    ifstream arq;
    vector<planilha> inputFardos;
    string box, procedencia, peso, qtdade;

    arq.open("temp.csv", ios::in); //abertura do arquivo
    if (arq.is_open()) { //verificacao de abertura

        int i = 0; //iteracao de linhas

        //modelo do arquivo: "box,procedencia,peso,qtdade\n"
        while (getline(arq, box, ',')) { //obtendo a primeira informacao do arquivo (id do box)
            inputFardos.push_back(planilha());

            inputFardos[i].box = stoi(box);

            getline(arq, procedencia, ','); //obtendo a proxima informacao do arquivo (procedencia do fardo)
            inputFardos[i].procedencia = procedencia;

            getline(arq, peso, ',');
            inputFardos[i].peso = stof(peso);

            getline(arq, qtdade);
            inputFardos[i].qtdade = stoi(qtdade);

            if (inputFardos[i].peso < criterio_peso) //caso o peso do fardo seja menor, ele é classificado como pequeno
                inputFardos[i].tamanho = "pequeno";
            else //caso contrario, é classificado como grande
                inputFardos[i].tamanho = "grande";

            i++; //iterando o contador de fardos para armazenar no proximo espaco do struct
        }
        arq.close(); //encerrando o arquivo
    }
    else {
        MessageBoxA(NULL, (LPCSTR)"Arquivo não encontrado!", (LPCSTR)"Disposição de Fardos", MB_ICONWARNING);
        exit(1);
    }
    return inputFardos;
}

void mapa(ga algoritmo) {
    //saida do mapa de disposicao de fardos

    ofstream arq;
    auto melhor = max_element(algoritmo.fitval.begin(), algoritmo.fitval.end()) - algoritmo.fitval.begin(); //indice do individuo com maior valor fitness
    
    arq.open("temp.csv", ios::trunc); //inicializando arquivo csv a ser escrito
    if (arq.is_open()) { //apenas escrever se o arquivo estiver aberto

        for (int i = 0; i < algoritmo.populacao[melhor].size(); i++) { //iterando todos os espacos da matriz do melhor individuo
            if (!algoritmo.populacao[melhor][i].empty()) { //caso o espaco esteja preenchido,

                int tipo = algoritmo.categoria(algoritmo.populacao[melhor][i]); //obter a categoria do fardo

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

//int main() {
//
//    //FreeConsole();  //fechar o prompt de comando durante a execucao
//
//    /*
//    Parâmetros do algoritmo
//    */
//
//    int populacaoTam = 20, geracaoTam = 10;
//    double mutacaoProb = 0.05;
//
//    /*
//    Inicialização de variáveis
//    */
//
//    vector<planilha> inputFardos;
//    vector<int> fitval;
//
//    chrono::time_point<chrono::system_clock> comeco, fim; //cronometros
//
//    int criterio_peso = 220; //classificacao de tamanhos de fardos
//    srand(static_cast<unsigned int>(time(NULL))); //semente para geracao de numeros aleatorios
//    
//    /*
//    Inicialização do algoritmo
//    */
//
//    inputFardos = ler_planilha(criterio_peso); //leitura de planilha para input
//
//    ga algoritmo(populacaoTam, mutacaoProb, inputFardos); //inicializando o algoritmo genético
//    algoritmo.init(); //inicializando a populacao para evolucao
//    fitval = algoritmo.fitness(); //avaliando a populacao inicializada
//
//    /*
//    Evolução
//    */
//
//    cout << "=== Valores Fitness ===" << endl;
//    cout << "Inicial: " << *max_element(fitval.begin(), fitval.end()) << endl;
//
//    comeco = chrono::system_clock::now(); //iniciando cronometro
//
//    for (int idx = 0; idx < geracaoTam; idx++) { //iteracao de geracoes
//
//        fitval = algoritmo.fitness();
//        algoritmo.cruzamento();
//        algoritmo.mutacao();
//    }
//
//    fim = chrono::system_clock::now(); //parando cronometro
//
//    fitval = algoritmo.fitness();
//    cout << "Final: " << *max_element(fitval.begin(), fitval.end()) << endl;
//
//    /*
//    Cronômetro
//    */
//
//    chrono::duration<double> segundos = fim - comeco; //calculando tempo de execucao
//
//    cout << endl << "=== Tempo do AG ===" << endl;
//    cout << segundos.count() << " segundos" << endl;
//
//    /*
//    Saída
//    */
//
//    //mapa(algoritmo);
//    //MessageBoxA(NULL, (LPCSTR)"Algoritmo executado com sucesso!", (LPCSTR)"Disposição de Fardos", MB_ICONINFORMATION);
//
//    return 0;
//}

int main() {

    testes Teste;
    Teste.parametros();

    return 0;
}