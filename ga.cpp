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

    int i = 0, pos = 0; //iteradores
    string variantes = { "abc" };

    while (i != matrizTam) { //enquanto nao percorrer todo o individuo

        if (filho[i].empty()) { //se a posicao estiver vazia, 

            if (inputFardos[categoria(mapa[pos])].tamanho == "grande") { //caso os fardos sejam grandes,

                mapa[pos].pop_back(); //retirar o identificador do fardo (o "b" de "123b")
                for (int d = 0; d < 3; d++) { //iterar 3 posicoes verticais para preenchimento de fardos grandes

                    int var = i + d * linhas; //atualizando posicao a ser preenchida (a mesma linha nas colunas posteriores)
                    filho[var] = mapa[pos] + variantes[d]; //preenchendo
                }
            }
            else { //caso os fardos sejam pequenos,

                mapa[pos].pop_back(); //retirar o identificador do fardo (o "b" de "123b")
                for (int d = 0; d < 2; d++) { //iterar 2 posicoes horizontais para preenchimento de fardos grandes

                    int var = i + d; //atualizando posicao a ser preenchida (a linha ao lado)
                    filho[var] = mapa[pos] + variantes[d]; //preenchendo
                }
            }
            mapa.erase(mapa.begin() + pos); //apagando o fardo que foi alocado da lista de preenchimento
        }
        i++; //proxima linha
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

                    int varOrigem = i + x, varComp = i + y; //linhas de origem e a que está sendo comparada
                    int fardoUm = categoria(populacao[chr][varOrigem]), fardoDois = categoria(populacao[chr][varComp]); //fardos que estão sendo comparados
                    int diferenca = abs(y - x); //distancia entre fardos

                    if (inputFardos[fardoUm].box == inputFardos[fardoDois].box) //se iguais,
                        fitval[chr] += (linhas - diferenca); //somar a distancia destes fardos no valor fitness
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

    if (fitval[desafiante] < fitval[vencedor]) //o vencedor é quem tiver MENOR valor fitness
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
    //mutacao por troca (swap)

    char variantes[4] = { "abc" };

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando individuos
        for (int i = 0; i < matrizTam; i++) { //iterando todas as posicoes do individuo

            double num_aleatorio = rand() / (double)RAND_MAX; //numero aleatorio entre 0 e 1
            if (num_aleatorio <= mutacaoProb) { //ocorre apenas se o numero aleatorio for menor do que a probabilidade de mutacao

                int fardoSwap = rand() % matrizTam; //escolhendo um fardo aleatorio para troca
                string tamSwap = inputFardos[categoria(populacao[chr][fardoSwap])].tamanho; //tamanho (pequeno/grande) do fardo para troca
                string tamLoc = inputFardos[categoria(populacao[chr][i])].tamanho; //tamanho (pequeno/grande) do fardo original

                while (tamSwap != tamLoc) { //garantindo que a troca ocorra entre fardos de mesmo tamanho
                    fardoSwap = rand() % matrizTam;
                    tamSwap = inputFardos[categoria(populacao[chr][fardoSwap])].tamanho;
                }

                //ajustando o fardo para trocas sempre entre posicoes a-a, b-b e c-c
                char letraLoc = populacao[chr][i].back(), letraSwap = populacao[chr][fardoSwap].back();
                int ajusteLoc = (int)letraLoc, ajusteSwap = (int)letraSwap;

                if (tamSwap == "pequeno") { //se forem fardos pequenos,
                    for (int c = 0; c < 2; c++) { //variando as 2 posicoes que fardos pequenos ocupam

                        int varSwap = fardoSwap + (int)variantes[c] - ajusteSwap;
                        int varLoc = i + (int)variantes[c] - ajusteLoc;
                        swap(populacao[chr][varLoc], populacao[chr][varSwap]); //trocando os fardos de lugar
                    }
                }

                else if (tamSwap == "grande") { //se grandes,
                    for (int c = 0; c < 3; c++) { //variando as 3 posicoes que fardos grandes ocupam

                        int varSwap = fardoSwap + ((int)variantes[c] - ajusteSwap) * linhas;
                        int varLoc = i + ((int)variantes[c] - ajusteLoc) * linhas;
                        swap(populacao[chr][varLoc], populacao[chr][varSwap]); //trocando os fardos de lugar
                    }
                }
            }
        }
    }
    ga::populacao = populacao;
    return populacao;
}
