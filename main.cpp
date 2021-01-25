#include <Windows.h> //MessageBoxA
#include "ga.h"
#include "testes.h"
using namespace std;

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

            if (stof(peso) < criterio_peso) //caso o peso do fardo seja menor, ele � classificado como pequeno
                tamanho = "pequeno";
            else //caso contrario, � classificado como grande
                tamanho = "grande";

            inputFardos.push_back({ stoi(box), stoi(qtdade), stof(peso), procedencia, tamanho }); //armazenando as informacoes
        }
        arq.close();
    }
    else { //caso o arquivo nao seja aberto,
        MessageBoxA(NULL, (LPCSTR)"Arquivo n�o encontrado!", (LPCSTR)"Disposi��o de Fardos", MB_ICONWARNING);
        exit(1);
    }
    return inputFardos;
}

void mapa(ga algoritmo) {
    //saida do mapa de disposicao de fardos

    ofstream arq;
    int melhor, tipo;

    melhor = max_element(algoritmo.fitval.begin(), algoritmo.fitval.end()) - algoritmo.fitval.begin(); //indice do individuo com maior valor fitness
    
    arq.open("temp.csv", ios::trunc);
    if (arq.is_open()) {

        for (int i = 0; i < algoritmo.populacao[melhor].size(); i++) { //iterando todos os espacos da matriz do melhor individuo
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
            if (((i - 3) % algoritmo.linhas) == 0) //pular linha quando o mapa j� estiver na 4� linha da coluna
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
//    Par�metros do algoritmo
//    */
//
//    int populacaoTam = 20, geracaoTam = 20;
//    double mutacaoProb = 0.05;
//
//    /*
//    Inicializa��o de vari�veis
//    */
//
//    vector<planilha> inputFardos;
//    vector<int> fitval;
//
//    chrono::time_point<chrono::system_clock> comeco, fim; //cronometros
//
//    float criterio_peso = 220; //classificacao de tamanhos de fardos
//    srand(static_cast<unsigned int>(time(NULL))); //semente para geracao de numeros aleatorios
//    
//    /*
//    Inicializa��o do algoritmo
//    */
//
//    inputFardos = ler_planilha(criterio_peso); //leitura de planilha para input
//
//    ga algoritmo(populacaoTam, mutacaoProb, inputFardos); //inicializando o algoritmo gen�tico
//    algoritmo.init(); //inicializando a populacao para evolucao
//    fitval = algoritmo.fitness(); //avaliando a populacao inicializada
//
//    /*
//    Evolu��o
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
//    Cron�metro
//    */
//
//    chrono::duration<double> segundos = fim - comeco; //calculando tempo de execucao
//
//    cout << endl << "=== Tempo do AG ===" << endl;
//    cout << segundos.count() << " segundos" << endl;
//
//    /*
//    Sa�da
//    */
//
//    //mapa(algoritmo);
//    //MessageBoxA(NULL, (LPCSTR)"Algoritmo executado com sucesso!", (LPCSTR)"Disposi��o de Fardos", MB_ICONINFORMATION);
//
//    return 0;
//}

int main() {

    testes Teste;
    Teste.parametros();

    return 0;
}