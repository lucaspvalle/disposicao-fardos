#include "ga.h"

/*
Funções de apoio
*/

void ga::seed(unsigned int s) {
    //definindo sementes diferentes para a execucao do algoritmo em testes
    srand(static_cast<unsigned int>(time(NULL)) * s);
}

int ga::categoria(string fardo) {
    //classificacao dos fardos de acordo com a procedencia por meio da soma acumulada

    int id, cumsum = 0;

    fardo.pop_back(); //retirar o ultimo caracter da string (o "b" de "123b", por ex.)
    id = stoi(fardo); //converter a string "123" para int 123 (por ex.)

    for (unsigned int tipo = 0; tipo < inputFardos.size(); tipo++) { //iterando a struct de tipos de fardos
        cumsum += inputFardos[tipo].qtdade;

        if (id <= cumsum)
            return tipo;
    }
    return NULL;
}

bool ga::checarLimites(int corte, int chr) {
    //checar se o limite do corte nao está pegando um fardo ao meio

    char letra;
    int var, tipo;

    for (int i = 0; i < linhas; i++) { //checar em todas as linhas da coluna cortada
        var = corte + i; //variavel armazenando a linha iterada
        letra = populacao[chr][var].back(), tipo = categoria(populacao[chr][var]); //letra de identificacao (o "b" de "123b") e o tipo do fardo (classe)

        if (inputFardos[tipo].tamanho == "grande" && letra != 'a') //se o fardo for grande e nao estiver em sua posicao inicial ("a"), está sendo cortado
            return true; //portanto, retornar verdadeiro e continuar o loop
    }
    return false; //se nenhum fardo estiver sendo cortado ao meio, parar o loop
}

bool ga::checarBloco(int corte, int chr) {
    //checar se o limite do corte nao está pegando um fardo ao meio

    int var, tipo, bloco = 3;

    for (int d = 0; d < bloco; d++) { //iterando todas as colunas dentro de um bloco
        for (int i = 0; i < linhas; i++) { //iterando todas as linhas da coluna em questao
            var = corte + d * linhas + i, tipo = categoria(populacao[chr][var]);

            //como um fardo grande tem o mesmo tamanho do bloco, um fardo pequeno na mesma coluna mostra que o fardo grande está sendo cortado
            if (inputFardos[categoria(populacao[chr][corte])].tamanho == "pequeno" && inputFardos[tipo].tamanho == "grande")
                return true; //portanto, retornar verdadeiro e continuar o loop
        }
    }
    return false; //se nenhum fardo estiver sendo cortado ao meio, parar o loop
}

limites ga::gerarCorte(int range, int chr, string operador) {
    //gerador de cortes para os operadores
    
    int corteInf = 0, corteSup = 0, bloco = 3;

    do { //gerando dois pontos de corte aleatorios
        corteInf = rand() % range, corteSup = rand() % range;
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

    while (checarLimites(corteInf, chr)) //enquanto algum fardo estiver sendo cortado, continuar
        corteInf -= linhas;
    
    while (checarLimites(corteSup, chr)) //enquanto algum fardo estiver sendo cortado, continuar
        corteSup -= linhas;

    return { corteInf, corteSup };
}

vector<string> ga::popularFardos(vector<string> filho, vector<string> mapa, int corte) {
    //suporte de preenchimento de fardos para operador OX
    
    int var, varUm, varDois, varIterUm, varIterDois, i, j;
    string variantes = { "abc" }; //identificador de posicoes do fardo na matriz
    vector<string> pequenos, grandes; //vetores de apoio para alocacao dos fardos

    for (unsigned int idx = 0; idx < mapa.size(); idx++) { //classificando os fardos a serem alocados
        if (inputFardos[categoria(mapa[idx])].tamanho == "grande") //se grandes,
            grandes.push_back(mapa[idx]);
        else //se pequenos,
            pequenos.push_back(mapa[idx]);
    }

    i = corte / linhas, j = 0; //iniciando o preenchimento a partir do corte superior na linha 0
    while (grandes.size() != 0) { //enquanto houver fardos grandes a serem alocados,

        var = j + (i * linhas), varUm = var + linhas, varDois = var + (2 * linhas); //pontos de insercao
        if (varUm >= matrizTam || varDois >= matrizTam) { //se a matriz estiver no final, voltar para o comeco
            i = 0, j = 0;
            continue;
        }

        if (filho[var].empty() && filho[varUm].empty() && filho[varDois].empty()) { //se os espacos estiverem vazios (disponiveis),
            grandes[0].pop_back(), grandes[1].pop_back(); //retirar o identificador de posicao (o "a" de "123a")
            
            for (int d = 0; d < 3; d++) { //iterar a quantidade de posicoes necessarias para o fardo
                varIterUm = var + d * linhas, varIterDois = varIterUm + 1;
                filho[varIterUm] = grandes[0] + variantes[d];
                filho[varIterDois] = grandes[1] + variantes[d];
            }
            grandes.erase(grandes.begin(), grandes.begin() + 2); //apagar da lista o fardo ja alocado
        }
        i += 3;

        if (i >= colunas)
            j += 2, i = 0;
        if (j == linhas)
            j = 0;
    }

    while (pequenos.size() != 0) { //enquanto houver fardos pequenos a serem alocados,

        var = j + i * linhas, varUm = var + 1; //pontos de insercao
        if (varUm >= matrizTam) { //se a matriz estiver no final, voltar para o comeco
            i = 0, j = 0;
            continue;
        }

        if (filho[var].empty() && filho[varUm].empty()) { //se os espacos estiverem vazios (disponiveis),
            pequenos[0].pop_back(); //retirar o identificador de posicao (o "a" de "123a")

            for (int d = 0; d < 2; d++) { //iterar a quantidade de posicoes necessarias para o fardo
                varIterUm = var + d;
                filho[varIterUm] = pequenos[0] + variantes[d];
            }
            pequenos.erase(pequenos.begin()); //apagar da lista o fardo ja alocado
        }
        j += 2; //proxima posicao disponivel

        if (j == linhas)
            j = 0, i++; //se estiver no fim da largura da matriz, ir para a proxima coluna
        if (i == colunas)
            i = 0; //se estiver no fim da matriz, voltar para o comeco
    }
    return filho;
}

/*
Funções do algoritmo genético
*/

void ga::init() {
    //inicializador de populacao para o algoritmo

    vector<string> fardos; //controle de fardos a serem misturados
    int grandes = 0, pequenos = 0; //quantidade de fardos classificados como grandes e pequenos

    for (unsigned int i = 0; i < inputFardos.size(); i++) { //iterando a lista de fardos a serem misturados
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

        random_shuffle(fardos.begin(), fardos.end()); //misturando a ordem de fardos a serem alocados
        populacao[chr] = popularFardos(populacao[chr], fardos, 0); //preenchendo os espacos vazios do individuo
    }
    ga::populacao = populacao;
}

vector<double> ga::fitness() {
    //funcao fitness de avaliacao

    int tipo, coluna, j;
    vector<double> valores(populacaoTam, 0.0); //vetor com os valores fitness de cada individuo
    vector<vector<double>> loc_fardos(inputFardos.size()); //vetor com a localizacao dos tipos de fardos ao longo das colunas

    //construcao do vetor com localizacao dos tipos de fardos
    for (int chr = 0; chr < populacaoTam; chr++) { //para cada individuo,
        for (int i = 0; i < matrizTam; i++) { //iterar todas as posicoes da matriz

            coluna = i / 4; //armazenando a coluna atual
            tipo = categoria(populacao[chr][i]); //armazenando o tipo do fardo

            if (loc_fardos[tipo].size() == 0) //se for o primeiro elemento do vetor (necessario pois nao é possivel utilizar .back() em vetor vazio),
                loc_fardos[tipo].push_back(static_cast<double>(coluna)); //inserir a coluna no vetor

            if (loc_fardos[tipo].back() != (static_cast<double>(coluna))) //condicao para inserir colunas repetidas no vetor
                loc_fardos[tipo].push_back(static_cast<double>(coluna)); //inserir a coluna no vetor
        }

        for (tipo = 0; tipo < inputFardos.size(); tipo++) //iterando os tipos de fardos
            if (loc_fardos[tipo].size() > 1) //se o fardo ocupar mais de duas colunas, continuar (c.c., nao há distancia para calcular)
                for (unsigned int i = 0; i < loc_fardos[tipo].size() - 1; i++) { //iterando as colunas de localizacao
                    j = i + 1;
                    valores[chr] += loc_fardos[tipo][j] - loc_fardos[tipo][i];
                }
    }
    ga::fitval = valores; //atualizando o valor fitness do algoritmo
    return valores; //retornando o valor fitness do algoritmo
}
int ga::selecao() {
    //selecao de individuos por torneios binarios

    int vencedor = 0, desafiante = 0;

    do { //gerando dois individuos aleatorios
        vencedor = rand() % populacaoTam, desafiante = rand() % populacaoTam;
    } while (vencedor == desafiante);

    return max(vencedor, desafiante); //o vencedor é quem possuir MAIOR valor fitness
}

void ga::cruzamento() {
    //Order Crossover (OX)

    limites cortes;
    string2d linhagem;
    int melhor, pai, mae;

    //elitismo :: continuar com o melhor individuo na proxima geracao
    melhor = static_cast<int>(max_element(fitval.begin(), fitval.end()) - fitval.begin());
    linhagem.push_back(populacao[melhor]);

    for (int chr = 1; chr < populacaoTam; chr++) { //iterando ate que a linhagem tenha o tamanho da populacao

        pai = selecao(), mae = selecao(); //selecionando dois genitores para linhagem                  
        cortes = gerarCorte(colunas, mae, "c"); //cortes de apoio para o cruzamento
        vector<string> filho(matrizTam, ""), mapaFilho;
        
        //cruzando o filho com a informacao genetica da MAE em um intervalo pré-definido de corte
        copy(populacao[mae].begin() + cortes.inf, populacao[mae].begin() + cortes.sup, filho.begin() + cortes.inf);

        for (int i = 0; i < matrizTam; i++) { //para todas as posicoes da matriz do PAI,

            if (populacao[pai][i].back() == 'a') { //caso o fardo esteja em sua posicao "a" (inicial),
                auto filhoIt = find(filho.begin() + cortes.inf, filho.begin() + cortes.sup, populacao[pai][i]); //iterar em busca de duplicatas do corte proveniente da MAE
                
                if (filhoIt == filho.begin() + cortes.sup) //se nao há duplicatas,
                    mapaFilho.push_back(populacao[pai][i]); //adicionar ao mapa para preenchimento dos espacos vazios do filho
            }
        }
        filho = popularFardos(filho, mapaFilho, cortes.sup); //populando o filho fora da area de corte
        linhagem.push_back(filho);
    }
    ga::populacao = linhagem;
}

void ga::mutacao() {
    //mutacao por troca

    limites cortes;
    double num_aleatorio;
    int bloco = 3, tamanho = bloco * linhas;

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando individuos
        num_aleatorio = rand() / (double)RAND_MAX; //numero aleatorio entre 0 e 1
        if (num_aleatorio <= mutacaoProb) { //ocorre apenas se o numero aleatorio for menor do que a probabilidade de mutacao

            cortes = gerarCorte(colunas - bloco, chr, "m"); //cortes de apoio para a mutacao

            while (checarBloco(cortes.inf, chr))
                cortes.inf += linhas;

            while (checarBloco(cortes.sup, chr))
                cortes.sup += linhas;

            //swap nos intervalos dos cortes inferior e superior de acordo com o tamanho do bloco de troca
            swap_ranges(populacao[chr].begin() + cortes.inf, populacao[chr].begin() + cortes.inf + tamanho, populacao[chr].begin() + cortes.sup);
        }
    }
}
