#include "ga.h"

/*
Funções de apoio
*/
int ga::categoria(string fardo) {
    //classificacao dos fardos de acordo com a procedencia
    int id, cumsum = 0; //soma acumulada

    fardo.pop_back(); //retirar o ultimo caracter da string (o "b" de "123b", por ex.)
    id = stoi(fardo); //converter a string "123" para int 123 (por ex.)

    for (int tipo = 0; tipo < inputFardos.size(); tipo++) { //iterando a struct
        cumsum += inputFardos[tipo].qtdade; //acumulando a soma

        if (id <= cumsum)
            return tipo; //retornando o indice para identificacao do fardo
    }
    return 99; //caso esteja errado
}

vector<string> ga::popularFardos(vector<string> filho, vector<string> mapa) {

    int i = 0, pos = 0;
    string variantes = { "abc" };

    while (i != matrizTam) {

        if (filho[i].empty()) {

            if (inputFardos[categoria(mapa[pos])].tamanho == "grande") {

                mapa[pos].pop_back();
                for (int d = 0; d < 3; d++) {

                    int var = i + d * linhas;
                    filho[var] = mapa[pos] + variantes[d];
                }
            }
            else {

                mapa[pos].pop_back();
                for (int d = 0; d < 2; d++) {

                    int var = i + d;
                    filho[var] = mapa[pos] + variantes[d];
                }
            }
            mapa.erase(mapa.begin() + pos);
        }
        i++;
    }
    return filho;
}

/*
Funções do algoritmo genético
*/

string2d ga::init() {
    //inicializacao dos cromossomos

    vector<int> fardos; //controle de fardos a serem misturados
    int total, grandes = 0, pequenos = 0; //quantidade de fardos classificados como grandes e pequenos
    string variantes = { "abc" }; //identificador de posicoes do fardo na matriz de acordo com seu tamanho

    for (int i = 0; i < inputFardos.size(); i++) { //iterando a lista de fardos a serem misturados
        if (inputFardos[i].tamanho == "pequeno") //se o fardo for classificado como pequeno,
            pequenos = pequenos + inputFardos[i].qtdade;
        else //se o fardo for classificado como grande,
            grandes = grandes + inputFardos[i].qtdade;
    }
    total = pequenos + grandes; //quantidade total de fardos no sistema

    for (int i = 1; i <= total; i++)
        fardos.push_back(i); //adicionando os fardos no vetor de controle

    ga::matrizTam = pequenos * 2 + grandes * 3; //calculando o tamanho necessario matriz para acomodar todos os fardos

    string2d populacao(populacaoTam, vector<string>(matrizTam, "")); //inicializando a populacao com <populacaoTam> individuos de tamanho <matrizTam>

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando os individuos (cromossomos)
        int id = 0; //variavel de iteracao para identificacao dos fardos

        shuffle(fardos.begin(), fardos.begin() + grandes, default_random_engine(time(NULL))); //misturando a ordem de fardos grandes a serem inicializados
        shuffle(fardos.begin() + grandes + 1, fardos.end(), default_random_engine(time(NULL))); //misturando a ordem de fardos pequenos a serem inicializados

        for (int i = 0; i < matrizTam; i++) {

            if (populacao[chr][i].empty()) { //se a posicao estiver vazia, preencher

                if (id < grandes) { //preenchimento de fardos grandes

                    for (int d = 0; d < 3; d++) { //um fardo grande requer 3 posicoes verticais da matriz

                        int var = i + d * linhas; //adicionando o fardo na linha i de 3 colunas
                        populacao[chr][var] = to_string(fardos[id]) + variantes[d]; //adicionando a identificacao do fardo (123) com a sua posicao (a, b ou c)
                    }
                    id++; //atualizando o fardo a ser inicializado
                }

                else if ((id >= grandes) && (id < total)) {

                    for (int d = 0; d < 2; d++) { //um fardo pequeno requer 2 posicoes horizontais da matriz

                        int var = i + d; //adicionando o fardo nas linhas i e i+1
                        populacao[chr][var] = to_string(fardos[id]) + variantes[d]; //adicionando a identificacao do fardo (123) com a sua posicao (a, b ou c)
                    }
                    id++; //atualizando o fardo a ser inicializado
                }
            }
        }
    }
    ga::populacao = populacao;
    return populacao;
}

vector<int> ga::fitness() {
    //valor fitness dos cromossomos
    
    vector<int> fitval(populacaoTam, 0); //inicializando o valor fitness de cada cromossomo igual a 0

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando individuos
        for (int i = 0; i < populacao[chr].size(); i = i + linhas) { //iterando colunas
            for (int x = 0; x < linhas; x++) { //iterando linhas
                for (int y = x + 1; y < linhas; y++) { //iterando da linha x até a ultima linha da matriz

                    int varOrigem = i + x;
                    int varComp = i + y; //linha que está sendo comparada com a linha x atual
                    int fardoUm = categoria(populacao[chr][varOrigem]), fardoDois = categoria(populacao[chr][varComp]); //fardos que estão sendo comparados
                    int diferenca = abs(y - x);

                    if (inputFardos[fardoUm].box == inputFardos[fardoDois].box) //se iguais,
                        fitval[chr] += linhas - diferenca; //somar a distancia destes fardos no valor fitness
                }
            }
        }
    }
    ga::fitval = fitval;
    return fitval;
}

int ga::selecao() {
    //selecao de individuos para linhagem

    int vencedor = rand() % populacaoTam, desafiante = rand() % populacaoTam; //gerando dois individuos aleatorios

    while (vencedor == desafiante) //caso os individuos sejam os mesmos, gerar um novo
        desafiante = rand() % populacaoTam;

    if (fitval[desafiante] < fitval[vencedor]) //o vencedor é quem tiver MAIOR valor fitness
        vencedor = desafiante;

    return vencedor;
}

string2d ga::cruzamento() {
    //Order Crossover (OX)

    string2d linhagem; //filhos gerados
    int colunas = matrizTam / linhas;
    int idx = 0;

    while (linhagem.size() < populacaoTam) {

        int pai = selecao(), mae = selecao(); //selecionando dois genitores para linhagem
        int corteInf = rand() % colunas, corteSup = rand() % colunas; //gerando dois pontos de corte aleatorios

        while (pai == mae) //caso os individuos sejam os mesmos, gerar um novo
            mae = selecao();

        while (corteSup == corteInf) //caso as colunas sejam as mesmas, gerar uma nova
            corteSup = rand() % colunas;

        if (corteSup < corteInf) //caso a coluna superior seja menor do que a inferior, trocar os valores
            swap(corteInf, corteSup);

        corteInf = corteInf * linhas, corteSup = corteSup * linhas;

        //ajustando para nao cortar um fardo ao meio
        char letraInf = populacao[pai][corteInf].back(), letraSup = populacao[mae][corteSup].back(); //verificando a posicao do fardo
        int ajusteInf = (int)letraInf - (int)'a', ajusteSup = (int)letraSup - (int)'a'; //ajustando o corte para o inicio do fardo
        corteSup = corteSup - ajusteSup * linhas, corteInf = corteInf - ajusteInf * linhas;

        int tamanho = corteSup - corteInf; //tamanho do corte
        vector<string> filho(populacao[pai].size(), ""), filha(populacao[mae].size(), ""); //inicializando os filhos
        vector<string> cortePai(tamanho), corteMae(tamanho); //inicializando o vetor de corte

        //cortando os pais para realizar a troca genetica em um intervalo pré-definido
        copy(populacao[pai].begin() + corteInf, populacao[pai].begin() + corteSup, cortePai.begin());
        copy(populacao[mae].begin() + corteInf, populacao[mae].begin() + corteSup, corteMae.begin());

        //cruzando os filhos com as informacoes geneticas dos pais em um intervalo pré-definido
        copy(corteMae.begin(), corteMae.end(), filho.begin() + corteInf);
        copy(cortePai.begin(), cortePai.end(), filha.begin() + corteInf);

        vector<string> mapaFilho, mapaFilha;

        for (int i = 0; i < matrizTam; i++) {

            if (populacao[pai][i].back() == 'a') {
                auto filhoIt = find(corteMae.begin(), corteMae.end(), populacao[pai][i]); //iterando em busca de duplicatas
                
                if (filhoIt == corteMae.end()) //se nao há duplicatas,
                    mapaFilho.push_back(populacao[pai][i]);
            }

            if (populacao[mae][i].back() == 'a') {
                auto filhaIt = find(cortePai.begin(), cortePai.end(), populacao[mae][i]); //iterando em busca de duplicatas
                
                if (filhaIt == cortePai.end()) //se nao há duplicatas,
                    mapaFilha.push_back(populacao[mae][i]);
            }
        }

        filho = popularFardos(filho, mapaFilho);
        filha = popularFardos(filha, mapaFilha);

        //adicionando os filhos na linhagem
        linhagem.push_back(vector<string>());
        linhagem[idx] = filho;
        idx++;

        linhagem.push_back(vector<string>());
        linhagem[idx] = filha;
        idx++;
    }
    ga::populacao = linhagem;
    return linhagem;
}

string2d ga::mutacao() {
    //mutacao por troca (swap)

    char variantes[4] = { "abc" };

    for (int chr = 0; chr < populacaoTam; chr++) {
        for (int i = 0; i < matrizTam; i++) {

            double num_aleatorio = rand() / (double)RAND_MAX; //numero aleatorio entre 0 e 1
            if (num_aleatorio <= mutacaoProb) { //ocorre apenas se o numero aleatorio for menor do que a probabilidade de mutacao

                int parada = 1; //criterio de parada para o loop
                while (parada != 0) { //continuar iterando ate ocorrer uma troca de fardos na matriz

                    int fardoSwap = rand() % matrizTam; //escolhendo um fardo aleatorio para troca
                    string tamSwap = inputFardos[categoria(populacao[chr][fardoSwap])].tamanho;
                    string tamLoc = inputFardos[categoria(populacao[chr][i])].tamanho;

                    if (tamSwap == tamLoc) {

                        char letraLoc = populacao[chr][i].back(), letraSwap = populacao[chr][fardoSwap].back(); //pegando o indice de variacao dos fardos (a, b ou c)
                        int ajusteLoc = (int)letraLoc, ajusteSwap = (int)letraSwap;

                        if (tamSwap == "pequeno") { //se forem fardos pequenos,

                            for (int c = 0; c < 2; c++) {
                                
                                int varSwap = fardoSwap + (int)variantes[c] - ajusteSwap;
                                int varLoc = i + (int)variantes[c] - ajusteLoc;
                                swap(populacao[chr][varLoc], populacao[chr][varSwap]); //trocando os fardos de lugar
                            }
                        }

                        else if (tamSwap == "grande") { //grande

                            for (int c = 0; c < 3; c++) { //variando quantas posicoes fardos grandes ocupam na matriz

                                int varSwap = fardoSwap + ((int)variantes[c] - ajusteSwap) * linhas;
                                int varLoc = i + ((int)variantes[c] - ajusteLoc) * linhas;
                                swap(populacao[chr][varLoc], populacao[chr][varSwap]); //trocando os fardos de lugar
                            }
                        }
                        parada = 0;
                    }
                }
            }
        }
    }
    ga::populacao = populacao;
    return populacao;
}
