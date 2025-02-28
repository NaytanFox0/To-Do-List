#include "buttons.h"

Button::Button(HINSTANCE hInst, HWND parentWnd, LPCWSTR text, int m_id, int x, int y, int w, int h) : hInstance(hInst), parent(parentWnd), m_id(m_id), symbol_id(BS_DEFAULTE), m_bgColor(RGB(200, 200, 200)), m_borderColor(RGB(200, 200, 200)), m_textColor(RGB(0, 0, 0)), m_isPressed(false), m_isHovered(false)
{
    hWnd = CreateWindowEx(
        0,
        L"BUTTON",
        text,
        WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        x,
        y,
        w,
        h,
        parentWnd,
        (HMENU)(LONG_PTR)m_id,
        hInst,
        nullptr
    );

    HRGN hRgn = CreateEllipticRgn(0, 0, w, h);
    SetWindowRgn(hWnd, hRgn, TRUE);

    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);
    
}

Button::~Button() {}

void Button::setBgColor(COLORREF bgColor) { m_bgColor = bgColor; }
void Button::setBorderColor(COLORREF borderColor) { m_borderColor = borderColor; }
void Button::setTextColor(COLORREF textColor) { m_textColor = textColor; }

void Button::setSymbol(int id) { symbol_id = id; }

int Button::GetId() const { return m_id; }
HWND Button::GetHandle() const { return hWnd; }

void Button::Draw(HDC hDC, RECT rcItem, UINT state) {
    // Determinar cores com base no estado do botão
    COLORREF bgColor = m_bgColor;
    if (state & ODS_SELECTED || m_isPressed) {
        // Botão pressionado - escurecer a cor de fundo
        bgColor = RGB(
            GetRValue(m_bgColor) * 0.8,
            GetGValue(m_bgColor) * 0.8,
            GetBValue(m_bgColor) * 0.8
        );
    }
    else if (m_isHovered) {
        // Botão em hover - clarear a cor de fundo
        bgColor = RGB(
            min(255, GetRValue(m_bgColor) * 1.1),
            min(255, GetGValue(m_bgColor) * 1.1),
            min(255, GetBValue(m_bgColor) * 1.1)
        );
    }

    // Limpar COMPLETAMENTE o fundo com a cor de chave de transparência
    HBRUSH hBgBrush = CreateSolidBrush(RGB(255, 255, 255)); // Cor chave para transparência
    FillRect(hDC, &rcItem, hBgBrush);
    DeleteObject(hBgBrush);

    // Calcular as dimensões do botão
    int width = rcItem.right - rcItem.left;
    int height = rcItem.bottom - rcItem.top;

    // Criar pincéis e canetas para desenho
    HBRUSH hBtnBrush = CreateSolidBrush(bgColor);
    HPEN hBtnPen = CreatePen(PS_SOLID, 1, m_borderColor);

    // Selecionar objetos GDI
    HGDIOBJ hOldBrush = SelectObject(hDC, hBtnBrush);
    HGDIOBJ hOldPen = SelectObject(hDC, hBtnPen);

    // Determinar o raio de arredondamento
    // Nota: Se width == height, teremos um círculo perfeito
    int cornerRadiusX = min(width, height) / 2;
    int cornerRadiusY = cornerRadiusX;

    // Desenhar o botão com cantos arredondados ou como um círculo
    if (width == height) {
        // Desenhar um círculo perfeito se largura e altura forem iguais
        Ellipse(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
    }
    else {
        // Desenhar retângulo com cantos arredondados
        // O raio é proporcional à menor dimensão
        int radius = min(width, height) / 4; // Ajuste o divisor para mais ou menos arredondamento
        RoundRect(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom,
            radius * 2, radius * 2);
    }

    // Obter texto do botão
    TCHAR buffer[256] = { 0 };
    GetWindowText(hWnd, buffer, sizeof(buffer) / sizeof(TCHAR));

    // Desenhar texto, se houver
    switch (symbol_id)
    {
    case 0:
        if (buffer[0] != '\0') {
            RECT textRect = rcItem;

            // Ajustar texto quando pressionado
            if (state & ODS_SELECTED || m_isPressed) {
                OffsetRect(&textRect, 1, 1);
            }

            // Configurar modo de fundo transparente
            SetBkMode(hDC, TRANSPARENT);
            SetTextColor(hDC, m_textColor);

            // Desenhar texto centralizado
            DrawText(hDC, buffer, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        break;

    case 1: {
        // Desenhando o "+", no centro da área
        int centerX = rcItem.left + width / 2;
        int centerY = rcItem.top + height / 2;

        HPEN hPen = CreatePen(PS_SOLID, 3, m_textColor); // Vermelho
        HGDIOBJ hOldPen = SelectObject(hDC, hPen);

        // Desenhando "+"
        MoveToEx(hDC, centerX - 10, centerY, NULL);
        LineTo(hDC, centerX + 10, centerY); // Linha horizontal
        MoveToEx(hDC, centerX, centerY - 10, NULL);
        LineTo(hDC, centerX, centerY + 10); // Linha vertical

        // Restaurar e liberar recursos
        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
        break;
    }

    case 2: {
        // Desenhando o "-", no centro da área
        int centerX = rcItem.left + width / 2;
        int centerY = rcItem.top + height / 2;

        HPEN hPen = CreatePen(PS_SOLID, 3, m_textColor); // Vermelho
        HGDIOBJ hOldPen = SelectObject(hDC, hPen);

        // Desenhando "-"
        MoveToEx(hDC, centerX - 10, centerY, NULL);
        LineTo(hDC, centerX + 10, centerY); // Linha horizontal

        // Restaurar e liberar recursos
        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
        break;
    }

    default:
        break;
    }

    // Desenhar foco, se necessário
    if (state & ODS_FOCUS) {
        RECT focusRect = rcItem;
        InflateRect(&focusRect, -3, -3);
        DrawFocusRect(hDC, &focusRect);
    }

    // Restaurar objetos GDI originais
    SelectObject(hDC, hOldBrush);
    SelectObject(hDC, hOldPen);

    // Liberar recursos
    DeleteObject(hBtnBrush);
    DeleteObject(hBtnPen);
}