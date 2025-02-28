#pragma once
#include <windows.h>

#define BS_DEFAULTE 0
#define BS_PLUS 1
#define BS_MINOS 2

class Button {
protected:
    HWND hWnd;              // Handle da janela
    HWND parent;            // Handle da janela pai
    HINSTANCE hInstance;    // Instância
    int m_id;               // Identificador do controle
    int symbol_id;          // identificador de aparencia
    COLORREF m_bgColor;     // cor de fundo
    COLORREF m_borderColor; // cor da borda
    COLORREF m_textColor;   // cor do texto
    bool m_isPressed;       // 
    bool m_isHovered;       //

public:
    Button(HINSTANCE hInst, HWND parentWnd, LPCWSTR text, int m_id, int x, int y, int w, int h);
    ~Button();

    // Modificadores
    void setBgColor(COLORREF bgColor);
    void setBorderColor(COLORREF borderColor);
    void setTextColor(COLORREF textColor);
    void setSymbol(int id);

    // Pega valores
    int GetId() const;
    HWND GetHandle() const;

    //
    void Draw(HDC hDC, RECT rcI, UINT state);
};