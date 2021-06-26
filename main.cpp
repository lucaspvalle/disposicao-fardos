// Ponto de entrada para o aplicativo

#include "headers/framework.h"
#include "headers/main.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*
Funções de Integração com o Algoritmo
*/

void selecionar_arquivos(HWND hWnd) {
 
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
                    // TODO: gravar o nome do path em uma string
                    if (SUCCEEDED(hr)) {
                        MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
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

    if (algoritmo.status) {
        algoritmo.init();

        for (int individuo = 0; individuo < tamanho_geracao; individuo++) {
            algoritmo.fitness();
            algoritmo.cruzamento();
            algoritmo.mutacao();
        }
        algoritmo.escrever_csv();
    }
}

/*
Funções de Interface
*/

ATOM MyRegisterClass(HINSTANCE hInstance) {

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAPADEFARDOS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MAPADEFARDOS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance;

   // Criando a janela
   HWND hWnd = CreateWindowW(
       szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0,  // Coordenadas x e y da janela
       CW_USEDEFAULT, 0,  // Largura e comprimento da janela
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
                selecionar_arquivos(hWnd);
                break;

            // Arquivo :: Seleciona Arquivo de Saída
            case ID_ARQUIVO_SAIDA:
                selecionar_arquivos(hWnd);
                break;

            // Arquivo :: Executa Algoritmo
            case ID_ARQUIVO_EXECUTAR:
                executar_algoritmo(hWnd);
                break;

            // Arquivo :: Saída
            case IDM_EXIT:
                DestroyWindow(hWnd);
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
            
            // TODO: Adicione qualquer código de desenho que use hdc aqui...
            
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
    return (int)msg.wParam;
}
