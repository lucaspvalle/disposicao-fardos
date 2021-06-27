// Ponto de entrada para o aplicativo

#include "headers/framework.h"
#include "headers/main.h"

#pragma comment (lib, "Gdiplus.lib")
using namespace Gdiplus;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

// Arquivos de integração
std::string entrada;
std::string saida;


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*
Funções de Integração com o Algoritmo
*/

void selecionar_arquivos(HWND hWnd, bool is_entrada) {
 
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr)) {
        IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(
            CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog,
            reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr)) {
            // Define os tipos de arquivos possíveis de serem abertos
            hr = pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
            hr = pFileOpen->SetDefaultExtension(L"csv;txt");
            hr = pFileOpen->SetFileTypeIndex(1);

            // Mostra a janela para abrir arquivo
            hr = pFileOpen->Show(NULL);

            // Pega o nome do arquivo
            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                
                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr)) {
                        
                        std::wstring ws(pszFilePath);
                        std::string temp(ws.begin(), ws.end());

                        if (is_entrada)
                            entrada = temp;
                        else
                            saida = temp;

                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
}

void executar_algoritmo(HWND hWnd) {

    const int tamanho_geracao = 250;

    srand(static_cast<unsigned int>(time(NULL)));

    // Inicializando algoritmo genético
    ga algoritmo;

    // Verificação do diretório de entrada
    if (entrada.empty()) {
        entrada = "temp.csv";  // Assume diretório específico para busca

        MessageBoxA(
            NULL,
            (LPCSTR)"Estamos procurando por temp.csv na pasta do aplicativo para suprir a falta de um arquivo de entrada!",
            (LPCSTR)"Disposição de Fardos",
            MB_ICONWARNING);
    }

    // Verificação do diretório de saída
    if (saida.empty()) {
        saida = entrada;  // Assume diretório de entrada

        MessageBoxA(
            NULL,
            (LPCSTR)"Sobrescreveremos o arquivo de entrada para suprir a falta de um arquivo de saida!",
            (LPCSTR)"Disposição de Fardos",
            MB_ICONWARNING);
    }

    // Continua apenas se a leitura não quebrar (retorna true ou false)
    bool status = algoritmo.__ler_csv(entrada);

    if (status) {

        // Barra de progresso
        RECT janela;
        GetClientRect(hWnd, &janela);  // Área da interface

        int scroll = GetSystemMetrics(SM_CYVSCROLL);  // Barra de rolagem
        
        HWND progresso = CreateWindowEx(
            0,
            PROGRESS_CLASS,
            szTitle,
            WS_CHILD | WS_VISIBLE,
            janela.left,
            janela.bottom - scroll,
            janela.right,
            scroll,
            hWnd,
            (HMENU)0,
            NULL,
            NULL);

        SendMessage(progresso, PBM_SETRANGE, 0, MAKELPARAM(0, tamanho_geracao));  // Define o tamanho da barra
        SendMessage(progresso, PBM_SETSTEP, (WPARAM)1, 0);                        // Define o incremeto da barra

        algoritmo.init();

        for (int individuo = 0; individuo < tamanho_geracao; individuo++) {
            algoritmo.fitness();
            algoritmo.cruzamento();
            algoritmo.mutacao();

            SendMessage(progresso, PBM_STEPIT, 0, 0);  // Incrementa a barra
        }
        algoritmo.escrever_csv(saida);
    }
}

/*
Funções de Interface
*/

void desenhar_imagem(HDC hdc)
{
    Graphics graphics(hdc);
    Image image(L"data/Tela.jpg");
    Pen pen(Color(255, 255, 0, 0), 2);

    graphics.DrawImage(&image, 10, 10);
    Rect destRect(200, 50, 150, 75);
    graphics.DrawRectangle(&pen, destRect);

    graphics.DrawImage(&image, destRect);
}

ATOM MyRegisterClass(HINSTANCE hInstance) {

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MAPADEFARDOS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance;

   // Criando a janela
   HWND hWnd = CreateWindowW(
       szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,

       CW_USEDEFAULT, 0,    // Coordenadas x e y da janela
       400, 400,            // Largura e comprimento da janela

       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
      return FALSE;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Procedimentos da interface
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    switch (message) {
    //case WM_CREATE:
      //  break;

    // Comandos da interface
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            // Menu
            switch (wmId)
            {
            // Arquivo :: Seleciona Arquivo de Entrada 
            case ID_ARQUIVO_ENTRADA:
                selecionar_arquivos(hWnd, true);
                break;

            // Arquivo :: Seleciona Arquivo de Saída
            case ID_ARQUIVO_SAIDA:
                selecionar_arquivos(hWnd, false);
                break;

            // Arquivo :: Executa Algoritmo
            case ID_ARQUIVO_EXECUTAR:
                executar_algoritmo(hWnd);
                break;

            // Ajuda :: Sobre
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    // Escreve na tela
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            desenhar_imagem(hdc);
            
            EndPaint(hWnd, &ps);
        }
        break;

    // Finaliza o aplicativo
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/*
Main
*/

// Inicia o aplicativo
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ULONG_PTR token;
    GdiplusStartupInput input = { 0 };

    input.GdiplusVersion = 1;
    GdiplusStartup(&token, &input, NULL);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAPADEFARDOS, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAPADEFARDOS));
    MSG msg;

    // Loop de mensagem principal
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    GdiplusShutdown(token);

    return (int)msg.wParam;
}
