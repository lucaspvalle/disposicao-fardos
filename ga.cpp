#include "ga.h"

/*
Fun��es de apoio
*/

int ga::categoria(string fardo) {
    //classificacao dos fardos de acordo com a procedencia por meio da soma acumulada

    int id, cumsum = 0;

    fardo.pop_back(); //retirar o ultimo caracter da string (o "b" de "123b", por ex.)
    id = stoi(fardo); //converter a string "123" para int 123 (por ex.)

    for (int tipo = 0; tipo < inputFardos.size(); tipo++) { //iterando a struct de tipos de fardos
        cumsum += inputFardos[tipo].qtdade;

        if (id <= cumsum)
            return tipo;
    }
    return 99;
}

limites ga::gerarCorte(int rangeCol, int chr, string operador) {
    //gerador de cortes para os operadores

    char letraInf, letraSup;
    int corteInf = 0, corteSup = 0, bloco = 3, ajusteInf, ajusteSup;;

    do { //gerando dois pontos de corte aleatorios
        corteInf = rand() % rangeCol, corteSup = rand() % rangeCol;
    } while (corteInf == corteSup);

    if (corteSup < corteInf) //caso a coluna superior seja menor do que a inferior, trocar os valores
        swap(corteInf, corteSup);

    if (operador == "m") { //caso os cortes sejam para o operador de mutacao,
        if (corteSup - corteInf <= bloco) { //caso haja sobreposicao dos cortes,
            if (corteSup + bloco >= colunas - bloco) //se o corte superior estiver no fim do bloco e sem espaco para a inversao de colunas,
                corteInf -= bloco;
            else //caso haja espaco no corte superior para empurrar o corte para a frente,
                corteSup += bloco;
        }
    }
    corteInf *= linhas, corteSup *= linhas;

    //ajustando o corte para nao pegar um fardo ao meio
    letraInf = populacao[chr][corteInf].back(), letraSup = populacao[chr][corteSup].back(); //obtendo a identificacao de posicao do fardo (a, b ou c)
    ajusteInf = (int)letraInf - (int)'a', ajusteSup = (int)letraSup - (int)'a'; //calculando a distancia do fardo em relacao � posicao inicial (a)
    corteInf -= ajusteInf * linhas, corteSup -= ajusteSup * linhas; //ajustando

    return { corteInf, corteSup };
}

vector<string> ga::popularFardos(vector<string> filho, vector<string> mapa, int corte) {
    //suporte de preenchimento de fardos para operador OX
    
    string variantes = { "abc" }; //identificador de posicoes do fardo na matriz
    vector<string> pequenos, grandes; //vetores de apoio para alocacao dos fardos

    for (int idx = 0; idx < mapa.size(); idx++) { //classificando os fardos a serem alocados
        if (inputFardos[categoria(mapa[idx])].tamanho == "grande") //se grandes,
            grandes.push_back(mapa[idx]);
        else //se pequenos,
            pequenos.push_back(mapa[idx]);
    }

    int i = corte / linhas, j = 0, idx = 0; //iniciando o preenchimento a partir do corte superior na linha 0
    while (grandes.size() != 0) { //enquanto houver fardos grandes a serem alocados,

        int var = j + (i * linhas); //ponto de insercao
        int varUm = var + linhas, varDois = var + (2 * linhas); //ponto de insercao dos outros espacos do fardo

        if (varUm >= matrizTam || varDois >= matrizTam) { //se a matriz estiver no final, voltar para o comeco
            i = 0; //j = 2;
            continue;
        }

        if (filho[var].empty() && filho[varUm].empty() && filho[varDois].empty()) { //se os espacos estiverem vazios (disponiveis),
            
            grandes[idx].pop_back(); //retirar o identificador de posicao (o "a" de "123a")

            for (int d = 0; d < 3; d++) { //iterar a quantidade de posicoes necessarias para o fardo
                int varIter = var + d * linhas;
                filho[varIter] = grandes[idx] + variantes[d];
            }
            grandes.erase(grandes.begin()); //apagar da lista o fardo ja alocado
        }
        j++; //proxima linha

        if (j == 2 && grandes.size() != 0)
            j = 0, i = i + 3; //se o par de fardos grandes ja estiverem alocados, ir para a proxima coluna disponivel
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

        if (j == linhas)
            j = 0, i++; //se estiver no fim da largura da matriz, voltar para o comeco
        if (i == colunas)
            i = 0; //se estiver no fim do comprimento da matriz, voltar para o comeco
    }
    return filho;
}

/*
Fun��es do algoritmo gen�tico
*/

void ga::init() {
    //inicializador de populacao para o algoritmo

    vector<string> fardos; //controle de fardos a serem misturados
    int grandes = 0, pequenos = 0; //quantidade de fardos classificados como grandes e pequenos
    unsigned int semente; //semente para geracao de numeros aleatorios

    for (int i = 0; i < inputFardos.size(); i++) { //iterando a lista de fardos a serem misturados
        if (inputFardos[i].tamanho == "pequeno") //se o fardo for classificado como pequeno,
            pequenos += inputFardos[i].qtdade;
        else //se o fardo for classificado como grande,
            grandes += inputFardos[i].qtdade;
    }

    for (int i = 1; i <= pequenos + grandes; i++) //vetor de apoio para a alocacao de fardos
        fardos.push_back(to_string(i) + 'a');

    ga::matrizTam = pequenos * 2 + grandes * 3; //calculando o tamanho necessario matriz para acomodar todos os fardos
    ga::colunas = matrizTam / linhas; //calculando tamanho da coluna

    string2d populacao(populacaoTam, vector<string>(matrizTam, "")); //inicializando a populacao com <populacaoTam> individuos de tamanho <matrizTam>
    for (int chr = 0; chr < populacaoTam; chr++) { //iterando os individuos (cromossomos)

        semente = static_cast<unsigned int>(time(NULL));
        shuffle(fardos.begin(), fardos.end(), default_random_engine(semente)); //misturando a ordem de fardos a serem alocados
        populacao[chr] = popularFardos(populacao[chr], fardos, 0); //preenchendo os espacos vazios do individuo
    }
    ga::populacao = populacao;
}

vector<int> ga::fitness() {
    //valor fitness dos cromossomos

    int col, base, box;
    vector<int> valores(populacaoTam, 0); //inicializando o valor fitness de cada cromossomo igual a 0
    
    for (int chr = 0; chr < populacaoTam; chr++) { //iterando individuos
        
        //vetor de incidencia dos tipos de fardos (sim/nao (1/0)) nas colunas da matriz
        vector<vector<int>> projecao(inputFardos.size(), vector<int>(colunas, 0));

        for (int i = 0; i < matrizTam; i++) {
            
            col = i / 4; //armazenando a coluna da posicao iterada
            box = inputFardos[categoria(populacao[chr][i])].box;

            for (int tipo = 0; tipo < inputFardos.size(); tipo++) //iterando a struct de tipos de fardos em busca do indice do box
                if (inputFardos[tipo].box == box) {
                    projecao[tipo][col] = 1; //1 caso haja um fardo <tipo> na coluna <col>, 0 caso contrario 
                    break;
                }
        }

        for (int tipo = 0; tipo < inputFardos.size(); tipo++) { //iterando a struct para calcular a distancia projetada de cada tipo de fardo
            base = -1; //inicializando a coluna base como -1 para ser sobrescrita pela primeira coluna com incidencia do fardo <tipo>
            for (int col = 0; col < colunas; col++) { //iterando todas as colunas da matriz

                if (projecao[tipo][col] == 1) { //somar a distancia entre as incidencias de fardos de mesmo tipo
                    if (base == -1) //se for a primeira coluna do vetor com incidencia,
                        base = col;

                    valores[chr] += (col - base);
                    base = col; //atualizando a coluna base para comparacao
                }
            }
        }
    }
    ga::fitval = valores;
    return valores;
}

int ga::selecao() {
    //selecao de individuos por torneios binarios

    int vencedor = 0, desafiante = 0;

    do { //gerando dois individuos aleatorios
        vencedor = rand() % populacaoTam, desafiante = rand() % populacaoTam;
    } while (vencedor == desafiante);

    return max(vencedor, desafiante); //o vencedor � quem possuir MAIOR valor fitness
}

void ga::cruzamento() {
    //Order Crossover (OX)

    limites cortes;
    string2d linhagem;
    int melhor, pai, mae;

    //elitismo :: continuar com o melhor individuo na proxima geracao
    melhor = max_element(fitval.begin(), fitval.end()) - fitval.begin();
    linhagem.push_back(populacao[melhor]);

    for (int chr = 1; chr < populacaoTam; chr++) { //iterando ate que a linhagem tenha o tamanho da populacao

        pai = selecao(), mae = selecao(); //selecionando dois genitores para linhagem                  
        cortes = gerarCorte(colunas, mae, "c"); //cortes de apoio para o cruzamento
        vector<string> filho(matrizTam, ""), mapaFilho;
        
        //cruzando o filho com a informacao genetica da MAE em um intervalo pr�-definido de corte
        copy(populacao[mae].begin() + cortes.inf, populacao[mae].begin() + cortes.sup, filho.begin() + cortes.inf);

        for (int i = 0; i < matrizTam; i++) { //para todas as posicoes da matriz do PAI,

            if (populacao[pai][i].back() == 'a') { //caso o fardo esteja em sua posicao "a" (inicial),
                auto filhoIt = find(filho.begin() + cortes.inf, filho.begin() + cortes.sup, populacao[pai][i]); //iterar em busca de duplicatas do corte proveniente da MAE
                
                if (filhoIt == filho.begin() + cortes.sup) //se nao h� duplicatas,
                    mapaFilho.push_back(populacao[pai][i]); //adicionar ao mapa para preenchimento dos espacos vazios do filho
            }
        }
        filho = popularFardos(filho, mapaFilho, cortes.sup); //populando o filho fora da area de corte
        linhagem.push_back(filho);
    }
    ga::populacao = linhagem;
}

void ga::mutacao() {
    //mutacao por inversao

    limites cortes;
    double num_aleatorio;
    int varInf, varSup, tipoInf, tipoSup, bloco = 3, tamanho = bloco * linhas;

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando individuos
        num_aleatorio = rand() / (double)RAND_MAX; //numero aleatorio entre 0 e 1
        if (num_aleatorio <= mutacaoProb) { //ocorre apenas se o numero aleatorio for menor do que a probabilidade de mutacao

            cortes = gerarCorte(colunas - bloco, chr, "m"); //cortes de apoio para a mutacao

            for (int d = 0; d < bloco; d++) { //verificando se o intervalo est� cortando ao meio algum fardo grande
                varInf = cortes.inf + d * linhas, tipoInf = categoria(populacao[chr][varInf]); //verificacao do corte inferior
                varSup = cortes.sup + d * linhas, tipoSup = categoria(populacao[chr][varSup]); //verificacao do corte superior

                //se o primeiro fardo for pequeno e houver um fardo grande no intervalo, reajustar o intervalo de corte
                if (inputFardos[categoria(populacao[chr][cortes.inf])].tamanho == "pequeno" && inputFardos[tipoInf].tamanho == "grande")
                    cortes.inf = varInf;

                if (inputFardos[categoria(populacao[chr][cortes.sup])].tamanho == "pequeno" && inputFardos[tipoSup].tamanho == "grande")
                    cortes.sup = varSup;
            }
            //swap nos intervalos dos cortes inferior e superior de acordo com o tamanho do bloco de troca
            swap_ranges(populacao[chr].begin() + cortes.inf, populacao[chr].begin() + cortes.inf + tamanho, populacao[chr].begin() + cortes.sup);
        }
    }
}
