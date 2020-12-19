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
    //suporte de preenchimento de fardos para operador OX

    string variantes = { "abc" };

    for (int pos = 0; pos < mapa.size(); pos++) {
        if (inputFardos[categoria(mapa[pos])].tamanho != "grande") {

            for (int i = pos; i < mapa.size(); i++) {
                if (inputFardos[categoria(mapa[i])].tamanho == "grande") {

                    swap(mapa[pos], mapa[i]);
                    break;
                }
            }
        }
    }

    int i = 0, j = 0, pos = 0, var, varUm, varDois, varIter;
    while (inputFardos[categoria(mapa[pos])].tamanho == "grande") {

        var = j + (i * linhas);
        varUm = var + linhas, varDois = var + 2 * linhas;

        if (filho[var].empty() && filho[varUm].empty() && filho[varDois].empty()) {

            mapa[pos].pop_back();

            for (int d = 0; d < 3; d++) {
                varIter = var + d * linhas;
                filho[varIter] = mapa[pos] + variantes[d];
            }
            mapa.erase(mapa.begin() + pos);
        }
        j++;

        if (j == 2)
            j = 0, i++;

        if (i > colunas - 3)
            i = 0;
    }
    
    i = 0, j = 0;
    while (mapa.size() != 0) {

        var = j + (i * linhas);
        varUm = var + 1;

        if (filho[var].empty() && filho[varUm].empty()) {

            mapa[pos].pop_back();

            for (int d = 0; d < 2; d++) {
                varIter = var + d;
                filho[varIter] = mapa[pos] + variantes[d];
            }
            mapa.erase(mapa.begin() + pos);
        }
        j = j + 2;

        if (j == 4)
            j = 0, i++;

        if (j + (i * linhas) > matrizTam - 2)
            i = 0;
    }
    return filho;
}

/*
Funções do algoritmo genético
*/

string2d ga::init() {

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
    ga::colunas = matrizTam / linhas; //armazenando tamanho da coluna

    string2d populacao(populacaoTam, vector<string>(matrizTam, "")); //inicializando a populacao com <populacaoTam> individuos de tamanho <matrizTam>

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando os individuos (cromossomos)
        int id = 0; //variavel de iteracao para identificacao dos fardos

        shuffle(fardos.begin(), fardos.begin() + grandes, default_random_engine(time(NULL))); //misturando a ordem de fardos grandes a serem inicializados
        shuffle(fardos.begin() + grandes + 1, fardos.end(), default_random_engine(time(NULL))); //misturando a ordem de fardos pequenos a serem inicializados

        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (populacao[chr][i + j * linhas].empty()) { //se a posicao estiver vazia, preencher

                    if (id < grandes) { //preenchimento de fardos grandes
                        for (int d = 0; d < 3; d++) { //um fardo grande requer 3 posicoes verticais da matriz

                            int varUm = i + (j + d) * linhas; //adicionando o fardo na linha i+j de 3 colunas
                            int varDois = varUm + 1; //adicionando o fardo par na linha i+j+1 de 3 colunas

                            populacao[chr][varUm] = to_string(fardos[id]) + variantes[d]; //adicionando a identificacao do fardo (123) com a sua posicao (a, b ou c)
                            populacao[chr][varDois] = to_string(fardos[id + 1]) + variantes[d]; //adicionando a identificacao do fardo (123) com a sua posicao (a, b ou c)
                        }
                        id = id + 2; //atualizando o fardo a ser inicializado
                    }

                    else if ((id >= grandes) && (id < total)) {
                        for (int d = 0; d < 2; d++) { //um fardo pequeno requer 2 posicoes horizontais da matriz

                            int var = i + (j * linhas) + d; //adicionando o fardo nas linhas i e i+1
                            populacao[chr][var] = to_string(fardos[id]) + variantes[d]; //adicionando a identificacao do fardo (123) com a sua posicao (a, b ou c)
                        }
                        id++; //atualizando o fardo a ser inicializado
                    }
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
        vector<vector<int>> projecao(inputFardos.size(), vector<int>(colunas, 0));

        for (int i = 0; i < matrizTam; i++) {
            
            int col = i / 4;
            int boxFardo = inputFardos[categoria(populacao[chr][i])].box;

            for (int tipo = 0; tipo < inputFardos.size(); tipo++)
                if (inputFardos[tipo].box == boxFardo) {
                    projecao[tipo][col] = 1;
                    break;
                }
        }

        for (int tipo = 0; tipo < inputFardos.size(); tipo++) {
            int colBase = -1;
            for (int col = 0; col < colunas; col++) {

                if (colBase == -1 && projecao[tipo][col] == 1)
                    colBase = col;

                if (projecao[tipo][col] == 1) {
                    fitval[chr] += (col - colBase);
                    colBase = col;
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

    if (fitval[desafiante] > fitval[vencedor]) //o vencedor é quem tiver MAIOR valor fitness
        vencedor = desafiante;

    return vencedor;
}

string2d ga::cruzamento() {
    //Order Crossover (OX)

    string2d linhagem; //filhos gerados
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

        vector<string> filho(populacao[pai].size(), ""), filha(populacao[mae].size(), ""); //inicializando os filhos

        //cruzando os filhos com as informacoes geneticas dos pais em um intervalo pré-definido
        copy(populacao[pai].begin() + corteInf, populacao[pai].begin() + corteSup, filha.begin() + corteInf);
        copy(populacao[mae].begin() + corteInf, populacao[mae].begin() + corteSup, filho.begin() + corteInf);

        vector<string> mapaFilho, mapaFilha;

        for (int i = 0; i < matrizTam; i++) {

            if (populacao[pai][i].back() == 'a') {
                auto filhoIt = find(filho.begin() + corteInf, filho.begin() + corteSup, populacao[pai][i]); //iterando em busca de duplicatas
                
                if (filhoIt == filho.begin() + corteSup) //se nao há duplicatas,
                    mapaFilho.push_back(populacao[pai][i]);
            }

            if (populacao[mae][i].back() == 'a') {
                auto filhaIt = find(filha.begin() + corteInf, filha.begin() + corteSup, populacao[mae][i]); //iterando em busca de duplicatas
                
                if (filhaIt == filha.begin() + corteSup) //se nao há duplicatas,
                    mapaFilha.push_back(populacao[mae][i]);
            }
        }

        //preenchendo os espacos vazios de fardos que estavam duplicados
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
    //mutacao por inversao

    int tamanhoBloco = 6; //invertendo 6 colunas

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando individuos
        double num_aleatorio = rand() / (double)RAND_MAX; //numero aleatorio entre 0 e 1
        if (num_aleatorio <= mutacaoProb) { //ocorre apenas se o numero aleatorio for menor do que a probabilidade de mutacao

            int corteInf = (rand() % colunas) * linhas; //gerando um ponto de corte aleatorio
            char letraInf = populacao[chr][corteInf].back(); //analisando o ponto em que o fardo está sendo cortado
            int ajusteInf = (int)letraInf - (int)'a'; //ajustando o fardo para nao cortá-lo ao meio
            
            corteInf = corteInf - ajusteInf * linhas;

            if ((corteInf + tamanhoBloco * linhas) > matrizTam) //verificando se há espaco necessario para a inversao
                corteInf = corteInf - tamanhoBloco * linhas;

            for (int i = 0; i < (tamanhoBloco / 2); i++) { //invertendo...
                for (int j = 0; j < linhas; j++) { //iterando as linhas das colunas
                    int varUm = corteInf + j + (i * linhas); //primeiro fardo
                    int varDois = corteInf + j + ((tamanhoBloco / 2) + i) * linhas; //segundo fardo
                    swap(populacao[chr][varUm], populacao[chr][varDois]); //troca
                }
            }
        }
    }
    ga::populacao = populacao;
    return populacao;
}