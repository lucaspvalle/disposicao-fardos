#include <Windows.h> //MessageBoxA
#include "headers/ga.h"
using namespace std;

vector<planilha> ler_planilha(float criterio_peso) {
    //leitura de arquivo CSV com inputs de fardos

    ifstream arq;
    vector<planilha> inputFardos;
    string box, procedencia, peso, qtdade, tamanho;

    arq.open("temp.csv", ios::in);
    if (arq.is_open()) {

        // TODO: atualizar modelo do arquivo: "SOBRA,QTDE A UTIL.,PROCEDENCIA,NC,PESO L,PESO B,BOX,68,54,COR\n"
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

void sumario(int populacaoTam, int geracaoTam, double mutacaoProb, vector<planilha> inputFardos, unsigned int semente) {

    vector<double> fitval;

    ga algoritmo(populacaoTam, mutacaoProb, inputFardos); //inicializando o algoritmo genético
    algoritmo.seed(semente); //semente para geracao de numeros aleatorios

    algoritmo.init(); //inicializando a populacao para evolucao
    fitval = algoritmo.fitness(); //avaliando a populacao inicializada

    for (int idx = 0; idx < geracaoTam; idx++) { //iteracao de geracoes
        fitval = algoritmo.fitness();
        algoritmo.cruzamento();
        algoritmo.mutacao();
    }

    mapa(algoritmo);
    MessageBoxA(NULL, (LPCSTR)"Algoritmo executado com sucesso!", (LPCSTR)"Disposição de Fardos", MB_ICONINFORMATION);
}

int main() {

    //FreeConsole();  //fechar o prompt de comando durante a execucao
    vector<planilha> inputFardos;

    inputFardos = ler_planilha(220); //leitura de planilha para input

    sumario(900, 250, 0.05, inputFardos, static_cast<double>(time(NULL)));

    return 0;
}