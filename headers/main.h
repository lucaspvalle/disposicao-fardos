#pragma once
#include "headers/resource.h"

#define MAX_LOADSTRING 100

/*
Fun��es
*/

// Define o estilo da interface
ATOM MyRegisterClass(HINSTANCE hInstance);

// Iniciar a interface
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

// Procedimentos da interface
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

// Manipulador de mensagem para as caixas de di�logos
INT_PTR CALLBACK    CaixasDialogo(HWND, UINT, WPARAM, LPARAM);

// Fun��o para carregar imagem PNG na interface
// Cr�ditos: selbie (https://stackoverflow.com/questions/66237978/c-gdi-how-to-get-and-load-image-from-resource)
Gdiplus::Bitmap* carrega_imagem(HMODULE hMod, const wchar_t* resid, const wchar_t* restype);

// Fun��o para abrir a caixa de sele��o de arquivos
// Cr�ditos: MSDN (https://docs.microsoft.com/pt-br/windows/win32/learnwin32/example--the-open-dialog-box)
void selecionar_arquivos(HWND hWnd, bool is_entrada);

// Fun��o para chamar a execu��o do algoritmo
void executar_algoritmo(HWND hWnd);

/*
Estruturas
*/

// Filtro para a sele��o de arquivos
COMDLG_FILTERSPEC filtros[] = {
    { L"Arquivos de texto", L"*.txt;*.csv" }
};
