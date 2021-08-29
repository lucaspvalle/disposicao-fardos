#pragma once

class Integrador
{
public:

    std::string dir_entrada, dir_saida;

	Integrador(std::string entrada, std::string saida) {
        
        if (entrada.empty()) { 
            dir_entrada = "temp.csv";  // Assume diretório específico para busca

            MessageBoxA(
                NULL,
                (LPCSTR)"Estamos procurando por temp.csv na pasta do aplicativo para suprir a falta de um arquivo de entrada!",
                (LPCSTR)"Disposição de Fardos",
                MB_ICONWARNING);
        }
        else
            dir_entrada = entrada;

        if (saida.empty()) {
            dir_saida = dir_entrada;  // Assume diretório de entrada

            MessageBoxA(
                NULL,
                (LPCSTR)"Sobrescreveremos o arquivo de entrada para suprir a falta de um arquivo de saida!",
                (LPCSTR)"Disposição de Fardos",
                MB_ICONWARNING);
        }
        else
            dir_saida = saida;
	}

	/* Leitura de arquivo CSV
	* 
	* @param: diretório do arquivo
	* @return:
	*/
	InputsFardos input_csv();

	/* Escrita de arquivo CSV
	* 
	* @param: diretório do arquivo
	*/
	void output_csv(std::vector<std::string> vencedor, InputsFardos fardos);
};

