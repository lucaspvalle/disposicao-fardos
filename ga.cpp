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

limites ga::gerarCorte(int rangeCol, int chr, string operador) {

    int corteInf = 0, corteSup = 0;

    do { //gerando dois pontos de corte aleatorios
        corteInf = rand() % rangeCol, corteSup = rand() % rangeCol;
    } while (corteInf == corteSup);

    if (corteSup < corteInf) //caso a coluna superior seja menor do que a inferior, trocar os valores
        swap(corteInf, corteSup);

    if (operador == "m") {
        if (corteSup - corteInf <= colunasBloco) { //caso haja sobreposicao dos cortes
            if (corteSup + colunasBloco >= colunas - colunasBloco)
                corteInf -= colunasBloco;
            else
                corteSup += colunasBloco;
        }
    }

    corteInf *= linhas, corteSup *= linhas;

    char letraInf = populacao[chr][corteInf].back(), letraSup = populacao[chr][corteSup].back();
    int ajusteInf = (int)letraInf - (int)'a', ajusteSup = (int)letraSup - (int)'a';
    
    corteInf -= ajusteInf * linhas, corteSup -= ajusteSup * linhas;

    limites cortes = { corteInf, corteSup };
    return cortes;
}

vector<string> ga::popularFardos(vector<string> filho, vector<string> mapa, int corte) {
    //suporte de preenchimento de fardos para operador OX
    
    string variantes = { "abc" };
    vector<string> pequenos, grandes;

    for (int idx = 0; idx < mapa.size(); idx++) { //classificando os fardos a serem alocados
        if (inputFardos[categoria(mapa[idx])].tamanho == "grande") //se grandes,
            grandes.push_back(mapa[idx]);
        else //se pequenos,
            pequenos.push_back(mapa[idx]);
    }

    int i = corte / linhas, j = 0, idx = 0;
    while (grandes.size() != 0) { //enquanto houver fardos grandes a serem alocados,

        int var = j + (i * linhas); //ponto de insercao
        int varUm = var + linhas, varDois = var + (2 * linhas); //ponto de insercao dos outros espacos do fardo

        if (varUm >= matrizTam || varDois >= matrizTam) { i = 0; continue; } //se a matriz estiver no final, voltar para o comeco

        if (filho[var].empty() && filho[varUm].empty() && filho[varDois].empty()) { //se os espacos estiverem vazios (disponiveis),
            
            grandes[idx].pop_back(); //retirar o identificador de posicao (o "a" de "123a")

            for (int d = 0; d < 3; d++) { //iterar a quantidade de posicoes necessarias para o fardo
                int varIter = var + d * linhas;
                filho[varIter] = grandes[idx] + variantes[d];
            }
            grandes.erase(grandes.begin()); //apagar da lista o fardo ja alocado
        }
        j++; //proxima linha

        if (j == 2 && grandes.size() != 0) { j = 0, i = i + 3; } //se o par de fardos grandes ja estiverem alocados, ir para a proxima coluna disponivel
    }

    while (pequenos.size() != 0) { //enquanto houver fardos pequenos a serem alocados,

        int var = j + i * linhas; //ponto de insercao
        int varUm = var + 1; //ponto de insercao dos outros espacos do fardo

        if (varUm == matrizTam) { i = 0; continue; } //se a matriz estiver no final, voltar para o comeco

        if (filho[var].empty() && filho[varUm].empty()) { //se os espacos estiverem vazios (disponiveis),

            pequenos[idx].pop_back(); //retirar o identificador de posicao (o "a" de "123a")

            for (int d = 0; d < 2; d++) { //iterar a quantidade de posicoes necessarias para o fardo
                int varIter = var + d;
                filho[varIter] = pequenos[idx] + variantes[d];
            }
            pequenos.erase(pequenos.begin()); //apagar da lista o fardo ja alocado
        }
        j = j + 2; //proxima posicao disponivel

        if (j == linhas) { j = 0, i++; } //se estiver no fim da largura da matriz, voltar para o comeco
        if (i == colunas) { i = 0; } //se estiver no fim do comprimento da matriz, voltar para o comeco
    }
    return filho;
}

/*
Funções do algoritmo genético
*/

void ga::init() {

    vector<int> fardos; //controle de fardos a serem misturados
    int grandes = 0, pequenos = 0, total; //quantidade de fardos classificados como grandes e pequenos
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
                int var = i + j * linhas;
                if (populacao[chr][var].empty()) { //se a posicao estiver vazia, preencher

                    if (id < grandes) { //preenchimento de fardos grandes
                        for (int d = 0; d < 3; d++) { //um fardo grande requer 3 posicoes verticais da matriz

                            int varUm = i + (j + d) * linhas; //adicionando o fardo na linha i+j de 3 colunas
                            int varDois = varUm + 1, fardoId = id + 1; //adicionando o fardo par na linha i+j+1 de 3 colunas

                            populacao[chr][varUm] = to_string(fardos[id]) + variantes[d]; //adicionando a identificacao do fardo (123) com a sua posicao (a, b ou c)
                            populacao[chr][varDois] = to_string(fardos[fardoId]) + variantes[d]; //adicionando a identificacao do fardo (123) com a sua posicao (a, b ou c)
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
}

vector<int> ga::fitness() {
    //valor fitness dos cromossomos
    
    vector<int> val(populacaoTam, 0); //inicializando o valor fitness de cada cromossomo igual a 0
    
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
                    val[chr] += (col - colBase);
                    colBase = col;
                }
            }
        }
    }

    ga::fitval = val;
    return val;
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

void ga::cruzamento() {
    //Order Crossover (OX)

    int idx = 0;
    string2d linhagem;

    //elitismo :: continuar com o melhor individuo na proxima geracao
    int melhor = max_element(fitval.begin(), fitval.end()) - fitval.begin();

    linhagem.push_back(vector<string>());
    linhagem[idx] = populacao[melhor];
    idx++;

    while (linhagem.size() < populacaoTam) { //enquanto a linhagem nao for do tamanho desejado,

        int pai = selecao(), mae = selecao(); //selecionando dois genitores para linhagem
                                              
        limites cortes = gerarCorte(colunas, mae, "c");
        int corteInf = cortes.inf, corteSup = cortes.sup;

        vector<string> filho(matrizTam, ""), mapaFilho; //inicializando o filho e mapa de apoio
        copy(populacao[mae].begin() + corteInf, populacao[mae].begin() + corteSup, filho.begin() + corteInf); //cruzando o filho com a informacao genetica da mae

        for (int i = 0; i < matrizTam; i++) { //para todas as posicoes da matriz do PAI,

            if (populacao[pai][i].back() == 'a') { //caso o fardo esteja em sua posicao "a" (inicial),
                auto filhoIt = find(filho.begin() + corteInf, filho.begin() + corteSup, populacao[pai][i]); //iterar em busca de duplicatas do corte proveniente da MAE
                
                if (filhoIt == filho.begin() + corteSup) //se nao há duplicatas,
                    mapaFilho.push_back(populacao[pai][i]); //adicionar ao mapa para preenchimento dos espacos vazios do filho
            }
        }
        filho = popularFardos(filho, mapaFilho, corteSup); //populando o filho fora da area de corte

        //adicionando os filhos na linhagem
        linhagem.push_back(vector<string>());
        linhagem[idx] = filho;
        idx++;
    }
    ga::populacao = linhagem;
}

void ga::mutacao() {
    //mutacao por inversao

    int tamanhoBloco = colunasBloco * linhas;

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando individuos
        double num_aleatorio = rand() / (double)RAND_MAX; //numero aleatorio entre 0 e 1
        if (num_aleatorio <= mutacaoProb) { //ocorre apenas se o numero aleatorio for menor do que a probabilidade de mutacao

            limites cortes = gerarCorte(colunas - colunasBloco, chr, "m");
            int corteInf = cortes.inf, corteSup = cortes.sup;

            for (int d = 0; d < colunasBloco; d++) {
                int varInf = corteInf + d * linhas, tipoInf = categoria(populacao[chr][varInf]);
                int varSup = corteSup + d * linhas, tipoSup = categoria(populacao[chr][varSup]);

                if (inputFardos[categoria(populacao[chr][corteInf])].tamanho == "pequeno" && inputFardos[tipoInf].tamanho == "grande")
                    corteInf = varInf;

                if (inputFardos[categoria(populacao[chr][corteSup])].tamanho == "pequeno" && inputFardos[tipoSup].tamanho == "grande")
                    corteSup = varSup;
            }

            vector<string> temp(tamanhoBloco, "");
            copy(populacao[chr].begin() + corteInf, populacao[chr].begin() + corteInf + tamanhoBloco, temp.begin()); //corte 1
            copy(populacao[chr].begin() + corteSup, populacao[chr].begin() + corteSup + tamanhoBloco, populacao[chr].begin() + corteInf); //corte 2
            copy(temp.begin(), temp.end(), populacao[chr].begin() + corteSup);
        }
    }
}
