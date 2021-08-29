#pragma once

class Integrador
{
public:

    std::string dir_entrada, dir_saida;

	Integrador(std::string entrada, std::string saida) {
        
        if (entrada.empty()) { 
            dir_entrada = "temp.csv";  // Assume diret�rio espec�fico para busca

            MessageBoxA(
                NULL,
                (LPCSTR)"Estamos procurando por temp.csv na pasta do aplicativo para suprir a falta de um arquivo de entrada!",
                (LPCSTR)"Disposi��o de Fardos",
                MB_ICONWARNING);
        }
        else
            dir_entrada = entrada;

        if (saida.empty()) {
            dir_saida = dir_entrada;  // Assume diret�rio de entrada

            MessageBoxA(
                NULL,
                (LPCSTR)"Sobrescreveremos o arquivo de entrada para suprir a falta de um arquivo de saida!",
                (LPCSTR)"Disposi��o de Fardos",
                MB_ICONWARNING);
        }
        else
            dir_saida = saida;
	}

	/* Leitura de arquivo CSV
	* 
	* @param: diret�rio do arquivo
	* @return:
	*/
	InputsFardos input_csv();

	/* Escrita de arquivo CSV
	* 
	* @param: diret�rio do arquivo
	*/
	void output_csv(std::vector<std::string> vencedor, InputsFardos fardos);
};

