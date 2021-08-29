#include "headers/framework.h"
#include "headers/main.h"

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

std::string entrada; // Arquivo de integração para o input
std::string saida;   // Arquivo de integração para o output

/*
Funções de Integração com o Algoritmo
*/

void selecionar_arquivos(HWND hWnd, bool is_entrada) {
 
    // Inicializando Component Object Model (COM)
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr)) {

        IFileOpenDialog* pFileOpen;  // Objeto para chamar a interface de abrir arquivos
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr)) {
            
            // Define os tipos de arquivos possíveis de serem abertos
            // filtros está definido no cabeçalho
            hr = pFileOpen->SetFileTypes(ARRAYSIZE(filtros), filtros);

            // Mostra a janela para abrir arquivo
            hr = pFileOpen->Show(NULL);

            if (SUCCEEDED(hr)) {

                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);  // Pega o nome do arquivo
                
                if (SUCCEEDED(hr)) {

                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);   // Pega o diretório do arquivo

                    if (SUCCEEDED(hr)) {
                        
                        std::wstring ws(pszFilePath);  // Transforma w_chart* para wstring
                        std::string temp = std::filesystem::path(ws).string();  // Transforma wstring para string
                        
                        // Condição decisória de atribuição para o arquivo selecionado
                        if (is_entrada)
                            entrada = temp;
                        else
                            saida = temp;

                        CoTaskMemFree(pszFilePath);  // Libera a memória
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

    srand(static_cast<unsigned int>(time(NULL)));

    Integrador integra(entrada, saida);
    InputsFardos fardos = integra.input_csv();

    if (!fardos.empty()) {

        AlgoritmoGenetico algoritmo(fardos);

        // Construção da barra de progresso
        RECT janela;
        GetClientRect(hWnd, &janela);                 // Área da interface
        int scroll = GetSystemMetrics(SM_CYVSCROLL);  // Barra de rolagem
        
        HWND progresso = CreateWindowEx(
            0, PROGRESS_CLASS, szTitle, WS_CHILD | WS_VISIBLE,
            janela.left, janela.bottom - scroll, janela.right,
            scroll, hWnd, (HMENU)0, NULL, NULL);

        SendMessage(progresso, PBM_SETRANGE, 0, MAKELPARAM(0, algoritmo.tamanho_geracao));  // Define o tamanho da barra
        SendMessage(progresso, PBM_SETSTEP, (WPARAM)1, 0);                                  // Define o incremeto da barra

        algoritmo.init();

        // Iteração do algoritmo genético
        for (int individuo = 0; individuo < algoritmo.tamanho_geracao; individuo++) {
            algoritmo.fitness();
            algoritmo.cruzamento();
            algoritmo.mutacao();

            SendMessage(progresso, PBM_STEPIT, 0, 0);  // Incrementa a barra
        }

        const int idx_maior_fitness = static_cast<const int>(std::max_element(algoritmo.valores_fitness.begin(), algoritmo.valores_fitness.end()) - algoritmo.valores_fitness.begin());
        std::vector<std::string> vencedor = algoritmo.populacao[idx_maior_fitness];

        integra.output_csv(vencedor, algoritmo.fardos);

        DestroyWindow(progresso);  // Libera a barra de progresso
    }
}


/*
Funções de Interface
*/

Gdiplus::Bitmap* carrega_imagem(HMODULE hMod, const wchar_t* resid, const wchar_t* restype) {

    IStream* pStream = nullptr;  // Ponteiro para o arquivo de input da imagem
    Gdiplus::Bitmap* pBmp = nullptr;  // Ponteiro para o arquivo de imagem
    HGLOBAL hGlobal = nullptr;  // Ponteiro para a memória alocada
    HRSRC hrsrc = FindResourceW(hInst, resid, restype);  // Pega o recurso utilizado na interface   

    if (hrsrc) {

        DWORD dwResourceSize = SizeofResource(hMod, hrsrc);  // Retorna o valor em bytes do recurso
        if (dwResourceSize > 0) {

            HGLOBAL hGlobalResource = LoadResource(hMod, hrsrc);  // Ponteiro para o primeiro byte do recurso em memória
            if (hGlobalResource) {

                void* imagebytes = LockResource(hGlobalResource);  // Ponteiro para o recurso em memória
                hGlobal = ::GlobalAlloc(GHND, dwResourceSize);  // Copia a alocação de memória da imagem
                if (hGlobal) {

                    void* pBuffer = ::GlobalLock(hGlobal);
                    if (pBuffer) {

                        memcpy(pBuffer, imagebytes, dwResourceSize);
                        HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);  // pStream agora tem o hGlobal
                        if (SUCCEEDED(hr)) {
                            
                            hGlobal = nullptr;
                            pBmp = new Gdiplus::Bitmap(pStream);
                        }
                    }
                }
            }
        }
    }

    // Libera as alocações de memória
    if (pStream) {
        pStream->Release();
        pStream = nullptr;
    }

    if (hGlobal) {
        GlobalFree(hGlobal);
        hGlobal = nullptr;
    }

    return pBmp;
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

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, 0,    // Coordenadas x e y da janela
        930, 580,            // Largura e comprimento da janela
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

INT_PTR CALLBACK CaixasDialogo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {

    UNREFERENCED_PARAMETER(lParam);
    switch (message) {

    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    switch (message) {

    // Comandos da interface
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        // Menu
        switch (wmId) {

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

        // Ajuda :: Manual de Uso
        case ID_AJUDA_MANUALDEUSO:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_MANUAL), hWnd, CaixasDialogo);
            break;

        // Ajuda :: Sobre
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, CaixasDialogo);
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

        Gdiplus::Graphics graphics(hdc);
        Gdiplus::Bitmap* pBmp = carrega_imagem(hInst, MAKEINTRESOURCE(IDI_BACKGROUND), L"PNG");
        graphics.DrawImage(pBmp, 0, 0);

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Inicializando o Gdiplus
    ULONG_PTR token;
    Gdiplus::GdiplusStartupInput input = { 0 };
    Gdiplus::GdiplusStartup(&token, &input, NULL);

    // Inicializando as strings globais
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAPADEFARDOS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Inicializando a interface
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAPADEFARDOS));
    MSG msg;

    // Loop de mensagem principal
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Encerra o Gdiplus
    Gdiplus::GdiplusShutdown(token);

    return (int)msg.wParam;
}
