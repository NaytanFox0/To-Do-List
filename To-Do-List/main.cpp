#include "buttons.h"
#include <vector>

// Classe base para janelas
class Window {
protected:
    HINSTANCE hInstance;
    HWND hWnd;
    LPCWSTR className;
    std::vector<Button> buttons;

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        Window* pThis = nullptr;
        if (message == WM_NCCREATE) {
            pThis = static_cast<Window*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
            pThis->hWnd = hWnd;
        }
        else {
            pThis = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        }

        if (pThis) {
            return pThis->HandleMessage(message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

public:
    Window(HINSTANCE hInst, LPCWSTR clsName) : hInstance(hInst), className(clsName), hWnd(nullptr) {}
    virtual ~Window() { buttons.clear(); }

    bool registerClass() {
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = className;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        return RegisterClass(&wc);
    }

    bool Create(LPCWSTR title, int width, int height) {
        hWnd = CreateWindowEx(
            0, 
            className, 
            title, 
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 
            CW_USEDEFAULT, 
            width, 
            height,
            nullptr, 
            nullptr, 
            hInstance, 
            this
        );
        return hWnd != nullptr;
    }

    void Show(int nCmdShow) { ShowWindow(hWnd, nCmdShow); UpdateWindow(hWnd); }
    HWND GetHandle() const { return hWnd; }

protected:
    virtual LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) 
        {
        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;

            // Verificar se é um de nossos botões
            for (auto& button : buttons) { // Assumindo que você tem uma coleção de botões
                if (lpDIS->CtlID == button.GetId()) {
                    button.Draw(lpDIS->hDC, lpDIS->rcItem, lpDIS->itemState);
                    return TRUE;
                }
            }
            
            break;
        }

        case WM_CREATE: {

            buttons.push_back(Button(hInstance, hWnd, L"", 1, 150, 150, 50, 50));
            buttons[0].setBgColor(RGB(30, 144, 255));
            buttons[0].setBorderColor(RGB(30, 144, 255));
            buttons[0].setTextColor(RGB(255, 255, 255));
            buttons[0].setSymbol(BS_PLUS);
            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) 
            {
            case 1:
                MessageBox(hWnd, L"Botão Clicado!", L"Evento", MB_OK);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
};

// Classe de aplicação principal
class Application {
    HINSTANCE hInstance;
    Window* mainWindow;

public:
    Application(HINSTANCE hInst) : hInstance(hInst), mainWindow(nullptr) {}

    int Run() {
        mainWindow = new Window(hInstance, L"MainWindowClass");
        if (!mainWindow->registerClass() || !mainWindow->Create(L"Minha Aplicação", 800, 600)) {
            return -1;
        }
        mainWindow->Show(SW_SHOW);

        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return static_cast<int>(msg.wParam);
    }
};

// Função principal WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    Application app(hInstance);
    return app.Run();
}