#include <fstream> //std::open
#include <algorithm> //std::shuffle, std::min_element
#include <Windows.h> //MessageBoxA
#include "ga.h"
using namespace std;

vector<planilha> ler_planilha(int criterio_peso) {
    //leitura de arquivo CSV com inputs de fardos

    fstream arq;
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
/*
void mapa(string2d individuo, vector<int> fitval) {
    //funcao para escrita do mapa de disposicao de fardos

    ofstream arq;
    int tipo, k = min_element(fitval.begin(), fitval.end()) - fitval.begin(); //indice do individuo com menor valor fitness

    arq.open("temp.csv", ios::trunc); //inicializando arquivo csv a ser escrito
    if (arq.is_open()) { //apenas escrever se o arquivo estiver aberto

        for (int j = 0; j < colunas; j++) {
            for (int i = 0; i < linhas; i++) {

                if (!individuo[k][j][i].empty()) { //se o espaco nao estiver vazio,

                    tipo = clf(individuo[k][j][i]); //classificando o fardo
                    if (individuo[k][j][i].back() == 'a') //apenas escrever no arquivo o primeiro fardo identificado
                        arq << dados[tipo].procedencia << " (" << dados[tipo].box << "),"; //escrevendo a procedencia e o box de localizacao do fardo

                    else //se for a identificacao de referencia para a ocupacao do tamanho do fardo na matriz, ignora
                        arq << dados[tipo].tamanho << ',';
                }
            }
            arq << endl; //pulando linha no arquivo para ser consistente com as linhas da matriz
        }
        arq.close(); //encerrando o arquivo
    }
}
*/

int main() {

    /*
    Parâmetros do algoritmo
    */

    int populacaoTam = 100, geracaoTam = 10;
    double mutacaoProb = 0.05;

    /*
    Inicialização de variáveis
    */

    vector<planilha> inputFardos;
    string2d populacao, linhagem;
    vector<int> fitval;

    int criterio_peso = 220;

    srand(time(NULL)); //semente para geracao de numeros aleatorios
    //FreeConsole();  //fechar o prompt de comando durante a execucao

    inputFardos = ler_planilha(criterio_peso); //leitura de planilha para input

    ga algoritmo(populacaoTam, mutacaoProb, inputFardos); 
    populacao = algoritmo.init();

    fitval = algoritmo.fitness();
    cout << "Fitval inicial: " << *max_element(fitval.begin(), fitval.end()) << endl;

    /*
    Evolução
    */

    int geracaoNum = 0;
    while (geracaoNum != geracaoTam) {

        fitval = algoritmo.fitness();
        linhagem = algoritmo.cruzamento();
        linhagem = algoritmo.mutacao();

        geracaoNum++;
    }

    fitval = algoritmo.fitness();
    cout << "Fitval final: " << *max_element(fitval.begin(), fitval.end()) << endl;

    return 0;
}
