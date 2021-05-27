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

double ga::faixas(int distancia) {
    //funcao para quebra da matriz em faixas de pontuacao para a distancia

    double largura, tamanho, intervalo = 0;

    largura = ceil(sqrt(colunas)); //definindo a largura de acordo com a regra de classes de histogramas
    tamanho = colunas / largura; //tamanho de faixas do conjunto

    //se a distancia estiver na ultima faixa, penalizar com um valor menor
    if (static_cast<double>(distancia) >= static_cast<double>(colunas) - largura)
        return 1;

    for (double i = 0; i < largura; i++) {

        intervalo = i * tamanho;
        if (static_cast<double>(distancia) >= intervalo && static_cast<double>(distancia) < intervalo + tamanho)
            return (i + 1);
    }
    return 0;
}

bool ga::checarLimites(int corte, int chr, int bloco = NULL) {
    //checar se o limite do corte nao está pegando um fardo ao meio

    char letra;
    int var, tipo;
    
    if (bloco == NULL) {
        for (int i = 0; i < linhas; i++) { //checar em todas as linhas da coluna cortada
            var = corte + i; //variavel armazenando a linha iterada
            letra = populacao[chr][var].back(), tipo = categoria(populacao[chr][var]); //letra de identificacao (o "b" de "123b") e o tipo do fardo (classe)

            if (inputFardos[tipo].tamanho == "grande" && letra != 'a') //se o fardo for grande e nao estiver em sua posicao inicial ("a"), está sendo cortado
                return true; //portanto, retornar verdadeiro e continuar o loop
        }
    }
    else {
        for (int d = 0; d < bloco; d++) { //iterando todas as colunas dentro de um bloco
            for (int i = 0; i < linhas; i++) { //iterando todas as linhas da coluna em questao
                var = corte + d * linhas + i;
                tipo = categoria(populacao[chr][var]);

                //como um fardo grande tem o mesmo tamanho do bloco, um fardo pequeno na mesma coluna mostra que o fardo grande está sendo cortado
                if (inputFardos[categoria(populacao[chr][corte])].tamanho == "pequeno" && inputFardos[tipo].tamanho == "grande")
                    return true; //portanto, retornar verdadeiro e continuar o loop
            }
        }
    }
    return false; //se nenhum fardo estiver sendo cortado ao meio, parar o loop
}

limites ga::gerarCorte(int range, int chr, int bloco = NULL) {
    //gerador de cortes para os operadores
    
    int corteInf = 0, corteSup = 0;

    do { //gerando dois pontos de corte aleatorios
        corteInf = rand() % range, corteSup = rand() % range;
    } while (corteInf == corteSup);

    if (corteSup < corteInf) //caso a coluna superior seja menor do que a inferior, trocar os valores
        swap(corteInf, corteSup);

    if (bloco != NULL) { //caso os cortes sejam para o operador de mutacao,
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

vector<string> ga::preenchimento(vector<string> filho, vector<string> pequenos, vector<string> grandes, int corte) {
    //suporte de preenchimento de fardos para operador OX

    int var, var_aux, quebra, i = corte / linhas, j = 0;
    string variantes = { "abc" }; //identificador de posicoes do fardo na matriz

    while (grandes.size() != 0) {
            quebra = 0, var = j + i * linhas;

            for (int d = 0; d < 3; d++) {
                var_aux = var + d * linhas;
                if (var_aux >= matrizTam || !filho[var_aux].empty())
                    quebra = 1;
            }

            if (quebra != 1) {
                for (int n = 0; n < 2; n++) {
                    grandes[n].pop_back();

                    for (int d = 0; d < 3; d++) {
                        var_aux = (var + n) + d * linhas;
                        filho[var_aux] = grandes[n] + variantes[d];
                    }
                }
                grandes.erase(grandes.begin(), grandes.begin() + 2);
            }
            i += 3;

            if (i >= colunas)
                j += 2, i = 0;
            if (j == linhas)
                j = 0;
        }

    while (pequenos.size() != 0) {
        quebra = 0, var = j + i * linhas;

        for (int d = 0; d < 2; d++) {
            var_aux = var + d;
            if (var_aux >= matrizTam || !filho[var_aux].empty())
                quebra = 1;
        }

        if (quebra != 1) {
            pequenos[0].pop_back();
            for (int d = 0; d < 2; d++) { //iterar a quantidade de posicoes necessarias para o fardo
                var_aux = var + d;
                filho[var_aux] = pequenos[0] + variantes[d];
            }
            pequenos.erase(pequenos.begin()); //apagar da lista o fardo ja alocado
        }
        j += 2;

        if (j == linhas)
            j = 0, i++;
        if (i == colunas)
            j = 0, i = 0;
    }
    return filho;
}

/*
Funções do algoritmo genético
*/

void ga::init() {
    //inicializador de individuos para a populacao

    int idx = 0;
    vector<string> grandes, pequenos;

    for (unsigned int i = 0; i < inputFardos.size(); i++) { //iterando a lista de fardos a serem misturados
        if (inputFardos[i].tamanho == "pequeno") { //se o fardo for classificado como pequeno,
            for (int j = 0; j < inputFardos[i].qtdade; j++)
                idx++, pequenos.push_back(to_string(idx) + 'a');
        }
        else { //se o fardo for classificado como grande,
            for (int j = 0; j < inputFardos[i].qtdade; j++)
                idx++, grandes.push_back(to_string(idx) + 'a');
        }
    }
    ga::matrizTam = static_cast<int>(pequenos.size()) * 2 + static_cast<int>(grandes.size()) * 3; //calculando o tamanho necessario matriz para acomodar todos os fardos
    ga::colunas = matrizTam / linhas; //calculando tamanho da coluna

    string2d populacao(populacaoTam, vector<string>(matrizTam, "")); //inicializando a populacao com <populacaoTam> individuos de tamanho <matrizTam>
    for (int chr = 0; chr < populacaoTam; chr++) { //iterando os individuos (cromossomos)

        random_shuffle(pequenos.begin(), pequenos.end()); //misturando a ordem de fardos a serem alocados
        random_shuffle(grandes.begin(), grandes.end()); //misturando a ordem de fardos a serem alocados
        populacao[chr] = preenchimento(populacao[chr], pequenos, grandes, 0);
    }
    ga::populacao = populacao;
}

vector<double> ga::fitness() {
    //quantificacao do desempenho objetivo do individuo

    double peso;
    int coluna, aux, distancia, j, tipo;
    int classes = static_cast<int>(inputFardos.size());
    vector<double> valores(populacaoTam, 0.0); //vetor com os valores fitness de cada individuo

    //construcao do vetor com localizacao dos tipos de fardos
    for (int chr = 0; chr < populacaoTam; chr++) { //para cada individuo,
        vector<int> ponderado(classes, 0); //vetor com a qtdade de fardos de cada tipo para penalizacao
        vector<vector<int>> loc_fardos(classes); //vetor com a localizacao dos tipos de fardos ao longo das colunas
        
        for (int i = 0; i < matrizTam; i++) {
            coluna = i / 4; //armazenando a coluna atual
            tipo = categoria(populacao[chr][i]);

            if (loc_fardos[tipo].size() == 0) //condicao para inserir colunas repetidas no vetor
                loc_fardos[tipo].push_back(coluna);

            if (loc_fardos[tipo].back() != coluna) //condicao para inserir colunas repetidas no vetor
                loc_fardos[tipo].push_back(coluna);
        }

        for (unsigned int i = 0; i < inputFardos.size(); i++) { //iterar todas as entradas de fardos
            if (inputFardos[i].tamanho == "grande") //se for grande,
                aux = 3; //penalizar com 3 pois eles sempre ocupam 3 colunas da matriz
            else //se for pequeno,
                aux = 1; //nao penalizar pois sempre ocupam apenas 1 coluna da matriz
            ponderado[i] += inputFardos[i].qtdade * aux;
        }
        
        for (tipo = 0; tipo < loc_fardos.size(); tipo++) { //iterando os tipos de fardos
            if (loc_fardos[tipo].size() > 1) { //se o fardo ocupar mais de duas colunas, continuar (c.c., nao há distancia para calcular)

                peso = static_cast<double>(loc_fardos[tipo].size()) / min(static_cast<double>(colunas), static_cast<double>(ponderado[tipo])); //penalizacao por % de colunas ocupadas
                for (unsigned int i = 0; i < loc_fardos[tipo].size() - 1; i++) { //iterando as colunas de localizacao
                    j = i + 1; //proxima posicao do fardo na matriz
                    distancia = loc_fardos[tipo][j] - loc_fardos[tipo][i]; //distancia entre os fardos
                    valores[chr] += peso * faixas(distancia);
                }
            }
        }
    }
    ga::fitval = valores; //atualizando o valor fitness do algoritmo
    return valores; //retornando o valor fitness do algoritmo
}

int ga::selecao() {
    //selecao de individuos por torneios binarios

    int vencedor = rand() % populacaoTam, desafiante = rand() % populacaoTam;
    
    do {
        desafiante = rand() % populacaoTam;
    } while (vencedor == desafiante);

    //se o desafiante possuir MAIOR valor fitness, ele vence o torneio
    if (fitval[desafiante] >= fitval[vencedor])
        vencedor = desafiante;

    return vencedor;
}

void ga::cruzamento() {
    //Order Crossover (OX)

    limites cortes;
    string2d linhagem;
    int  pai, mae;
    vector<string>::iterator iterador;

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando ate que a linhagem tenha o tamanho da populacao

        vector<string> filho(matrizTam, ""), pequenos, grandes;
        pai = selecao(), mae = selecao(); //selecionando dois genitores para linhagem

        if ((rand() / (double)RAND_MAX) >= cruzamentoProb) {
            if (fitval[pai] >= fitval[mae])
                linhagem.push_back(populacao[pai]);
            else
                linhagem.push_back(populacao[mae]);
            continue;
        }

        cortes = gerarCorte(colunas, mae); //cortes de apoio para o cruzamento
        copy(populacao[mae].begin() + cortes.inf, populacao[mae].begin() + cortes.sup, filho.begin() + cortes.inf); //corte dos genes da mae para o filho

        for (int i = 0; i < matrizTam; i++) { //iterando o pai
            if (populacao[pai][i].back() == 'a') { //caso o fardo esteja em sua posicao "a" (inicial),
                iterador = find(filho.begin() + cortes.inf, filho.begin() + cortes.sup, populacao[pai][i]); //o fardo do pai já está alocado no filho?

                if (iterador == filho.begin() + cortes.sup) { //se não,
                    if (inputFardos[categoria(populacao[pai][i])].tamanho == "pequeno") 
                        pequenos.push_back(populacao[pai][i]); //adicioná-lo para inserção de fardos pequenos
                    else
                        grandes.push_back(populacao[pai][i]); //adicioná-lo para inserção de fardos grandes
                }
            }
        }
        filho = preenchimento(filho, pequenos, grandes, cortes.sup); //populando o filho fora da area de corte
        linhagem.push_back(filho); //adicionando à linhagem
    }
    ga::populacao = linhagem;
}

void ga::mutacao() {
    //mutacao por troca

    limites cortes;
    int bloco = 3, tamanho = bloco * linhas;

    for (int chr = 0; chr < populacaoTam; chr++) { //iterando individuos
        if ((rand() / (double)RAND_MAX) <= mutacaoProb) { //ocorre apenas se o numero aleatorio for menor do que a probabilidade de mutacao

            cortes = gerarCorte(colunas - bloco, chr, bloco); //cortes de apoio para a mutacao
            while (checarLimites(cortes.inf, chr, bloco))
                cortes.inf += linhas;

            while (checarLimites(cortes.sup, chr, bloco))
                cortes.sup += linhas;

            //swap nos intervalos dos cortes inferior e superior de acordo com o tamanho do bloco de troca
            swap_ranges(populacao[chr].begin() + cortes.inf, populacao[chr].begin() + cortes.inf + tamanho, populacao[chr].begin() + cortes.sup);
        }
    }
}
