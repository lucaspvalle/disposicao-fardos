#include <fstream> //std::open
#include <Windows.h> //MessageBoxA
#include "ga.h"
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
    int melhor = min_element(algoritmo.fitval.begin(), algoritmo.fitval.end()) - algoritmo.fitval.begin(); //indice do individuo com menor valor fitness

    arq.open("temp.csv", ios::trunc); //inicializando arquivo csv a ser escrito
    if (arq.is_open()) { //apenas escrever se o arquivo estiver aberto

        for (int i = 0; i < algoritmo.populacao[melhor].size(); i++) {
            if (!algoritmo.populacao[melhor][i].empty()) {

                int tipo = algoritmo.categoria(algoritmo.populacao[melhor][i]);

                if (algoritmo.populacao[melhor][i].back() == 'a')
                    arq << algoritmo.inputFardos[tipo].procedencia << " (" << algoritmo.inputFardos[tipo].box << "),";
                else
                    arq << algoritmo.inputFardos[tipo].tamanho << ',';
            }
            if ((i - 3) % algoritmo.linhas == 0)
                cout << endl;
        }
    }
    arq.close();
}

int main() {

    /*
    Parâmetros do algoritmo
    */

    int populacaoTam = 1000, geracaoTam = 100;
    double mutacaoProb = 0.02;

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
    cout << "Fitval inicial: " << *min_element(fitval.begin(), fitval.end()) << endl;

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
    cout << "Fitval final: " << *min_element(fitval.begin(), fitval.end()) << endl;

    //mapa(algoritmo);
    //MessageBoxA(NULL, (LPCSTR)"Algoritmo executado com sucesso!", (LPCSTR)"Disposição de Fardos", MB_ICONINFORMATION);

    return 0;
}
