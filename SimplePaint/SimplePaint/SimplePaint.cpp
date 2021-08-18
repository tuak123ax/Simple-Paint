// SimplePaint.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "SimplePaint.h"
#include"windowsx.h"
#include <windows.h>
#include <commctrl.h>
#include<vector>
#include <commdlg.h>
#pragma comment(lib, "Comctl32.lib")
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include<string>
#include <fstream>
#define IMAGE_WIDTH     18
#define IMAGE_HEIGHT    18
#define BUTTON_WIDTH    0
#define BUTTON_HEIGHT   0
#ifdef MATHLIBRARY_EXPORTS
#define PAINTLIBRARY_API __declspec(dllexport) 
#else
#define PAINTLIBRARY_API __declspec(dllimport) 
#endif

namespace PaintLibrary
{
    //Enum để lựa chọn công cụ vẽ
    enum ShapeMode
    {
        LINE,
        DRAWRECT,
        DRAWELLIPSE,
        FILLRECT,
        FILLELLIPSE

    };

    struct Point
    {
        int x;
        int y;
    };


    Point newPoint(int x, int y)
    {
        Point p;
        p.x = x;
        p.y = y;

        return p;
    }

    class CShape
    {
    protected:
        int x1;
        int y1;
        int x2;
        int y2;
        COLORREF color;
        ShapeMode type;
        int penStyle;
    public:
        virtual void Draw(HDC hdc) = 0;
        virtual CShape* Create() = 0;
        virtual void SetData(int a, int b, int c, int d, COLORREF color, int penStyle) = 0;
        virtual ShapeMode getType() = 0;
        void setType(ShapeMode type)
        {
            this->type = type;
        }
        COLORREF getColor()
        {
            return this->color;
        }
        RECT* getDimens()
        {
            RECT* rect = new RECT();
            rect->left = x1;
            rect->top = y1;
            rect->right = x2;
            rect->bottom = y2;

            return rect;
        }
        void setDimens(RECT* rect)
        {
            this->x1 = rect->left;
            this->x2 = rect->right;
            this->y1 = rect->top;
            this->y2 = rect->bottom;
        }

        void setColor(COLORREF color)
        {
            this->color = color;
        }

        int getPenStyle()
        {
            return penStyle;
        }
    };
    class CRectangle : public CShape {
    private:
    public:
        CShape* Create()
        {
            return new CRectangle;
        }


        void SetData(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, COLORREF color, int penStyle)
        {
            x1 = nLeftRect;
            y1 = nTopRect;
            x2 = nRightRect;
            y2 = nBottomRect;

            this->type = DRAWRECT;
            this->color = color;
            this->penStyle = penStyle;
        }


        void Draw(HDC hdc)
        {
            if (x1 < x2)
            {
                int tmp = x1;
                x1 = x2;
                x2 = tmp;
            }
            if (y1 < y2)
            {
                int tmp = y1;
                y1 = y2;
                y2 = tmp;
            }

            Rectangle(hdc, x1, y1, x2, y2);
        }

        ShapeMode getType()
        {
            return this->type;
        }

    };

    class CLine : public CShape {
    public:
    public:
        void SetData(int a, int b, int c, int d, COLORREF color, int penStyle)
        {
            x1 = a;
            y1 = b;
            x2 = c;
            y2 = d;

            this->type = LINE;
            this->color = color;
            this->penStyle = penStyle;
        }

        void Draw(HDC hdc)
        {
            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);
        }

        CShape* Create()
        {
            return new CLine;
        }

        ShapeMode getType()
        {
            return this->type;
        }
    };
    class CEllipse : public CShape {
    private:
    public:
        CShape* Create()
        {
            return new CEllipse;
        }

        void SetData(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, COLORREF color, int penStyle)
        {
            x1 = nLeftRect;
            y1 = nTopRect;
            x2 = nRightRect;
            y2 = nBottomRect;

            this->type = DRAWELLIPSE;
            this->color = color;
            this->penStyle = penStyle;
        }

        void Draw(HDC hdc)
        {
            if (x1 < x2)
            {
                int tmp = x1;
                x1 = x2;
                x2 = tmp;
            }
            if (y1 < y2)
            {
                int tmp = y1;
                y1 = y2;
                y2 = tmp;
            }

            Ellipse(hdc, x1, y1, x2, y2);
        }

        ShapeMode getType()
        {
            return this->type;
        }
    };
}
using namespace PaintLibrary;
using namespace std;
int choose = 0;
CHOOSEFONT  cf;
LOGFONT  lf;
HFONT  hfont;HGDIOBJ hfontPrev;
HDC hdc;

#define MAX_LOADSTRING 100
#define MAX_BUFF		255
CHOOSECOLOR  cc; // Thông tin màu chọn
COLORREF  acrCustClr[16]; // Mảng custom color
DWORD  rgbCurrent = RGB(255, 0, 0); // Red
DWORD  rgbPrev;
HBRUSH  hbrush; // Tạo ra brush từ màu đã chọn

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnDestroy(HWND hwnd);
void OnPaint(HWND hwnd);
void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
int fromX;
int fromY;
int toX;
int toY;
bool isPreview;
bool isDrawing;
bool isDrawingTextbox = false;
HWND g_hWnd;
HWND hStatusBar;
PaintLibrary::Point p1, p2;
bool isDown = false;
PaintLibrary::ShapeMode currShapeMode = PaintLibrary::DRAWELLIPSE;
vector<PaintLibrary::CShape*> shapes;
PaintLibrary::CShape* currShape = NULL;
COLORREF currColor = RGB(219, 50, 54);
int currPenSize = 1;
int currPenStyle = PS_SOLID;
HPEN hPen = CreatePen(currPenStyle, currPenSize, currColor);
int windowWidth = 1080, windowHeight = 720;
string defaultFilePath = "data.bin";
HWND textbox;
HWND text;



void saveToFile(string filePath)
{
    ofstream myfile;
    myfile.open(filePath, ios::binary);

    if (!myfile.is_open())
    {
        MessageBox(g_hWnd, L"Error open file!", L"My Paint", MB_OK | MB_ICONWARNING);
        return;
    }

    //ok, proceed with output
    int l = shapes.size();
    myfile.write((char*)&l, sizeof(l));

    for (int i = 0; i < shapes.size(); i++)
    {
        myfile.write(reinterpret_cast<const char*>(shapes[i]), sizeof(PaintLibrary::CShape));
    }
    myfile.close();


    //Read
    ifstream inFile;
    inFile.open(filePath, ios::binary);

    if (!inFile.is_open())
    {
        MessageBox(g_hWnd, L"Error open file!", L"My Paint", MB_OK | MB_ICONWARNING);
        return;
    }
    int len = 0;
    inFile.read((char*)&len, sizeof(int));

    while (!inFile.eof() && len > 0)
    {
        char* item_buff = new char[255];
        PaintLibrary::CShape* tmp;
        inFile.read(item_buff, sizeof(PaintLibrary::CShape));
        tmp = reinterpret_cast<PaintLibrary::CShape*>(item_buff);
        //Workaround!
        if (tmp == NULL || &tmp == NULL)
        {
            break;
        }

        switch (tmp->getType())
        {
        case PaintLibrary::LINE:
            shapes.push_back(((PaintLibrary::CLine*)tmp));
            break;
        case PaintLibrary::DRAWRECT:
            shapes.push_back(((PaintLibrary::CRectangle*)tmp));
            break;
        case PaintLibrary::DRAWELLIPSE:
            shapes.push_back(((PaintLibrary::CEllipse*)tmp));
            break;
        case FILLRECT:
            break;
        case FILLELLIPSE:
            break;
        default:
            break;
        }

        len--;
    }
    inFile.close();
}


void saveToBinaryFile(string filePath) {
    std::ofstream out;
    out.open(filePath, std::iostream::out | std::iostream::binary | std::iostream::trunc);

    if (out.is_open()) {
        int size = shapes.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));

        for (PaintLibrary::CShape* shape : shapes) {
            int type = shape->getType();
            COLORREF color = shape->getColor();
            RECT* rect = shape->getDimens();
            out.write(reinterpret_cast<const char*>(&type), sizeof(type));
            out.write(reinterpret_cast<const char*>(&color), sizeof(COLORREF));
            out.write(reinterpret_cast<const char*>(rect), sizeof(RECT));
        }
    }
    else {
        OutputDebugString(L"Can't open data.bin to write");
    }

    out.close();
}

void loadFromBinaryFile(string filePath) {
    std::ifstream in;
    in.open(filePath, std::iostream::in | std::iostream::binary);

    if (in.is_open()) {
        char* buffer = new char[MAX_BUFF];
        int size;
        in.read(buffer, sizeof(size));

        size = buffer[0];
        shapes.clear();


        for (int i = 0; i < size; i++)
        {
            char* item_buff = new char[MAX_BUFF];

            PaintLibrary::CShape* shape = NULL;

            int type;
            COLORREF color;
            in.read(item_buff, sizeof(type));
            type = item_buff[0];
            in.read(item_buff, sizeof(COLORREF));
            color = item_buff[0];
            int r = GetRValue(color);
            color = item_buff[1];
            int g = GetGValue(color);
            color = item_buff[2];
            int b = GetBValue(color);

            color = RGB(r, g, b);

            switch (type)
            {
            case PaintLibrary::LINE:
                shape = new PaintLibrary::CLine();
                break;
            case PaintLibrary::DRAWRECT:
                shape = new PaintLibrary::CRectangle();
                break;
            case PaintLibrary::DRAWELLIPSE:
                shape = new PaintLibrary::CEllipse();
                break;
            case FILLRECT:
                break;
            case FILLELLIPSE:
                break;
            default:
                break;
            }

            shape->setType((PaintLibrary::ShapeMode)type);
            shape->setColor(color);

            RECT* rect;
            in.read(item_buff, sizeof(RECT));
            rect = reinterpret_cast<RECT*>(item_buff);
            shape->setDimens(rect);

            shapes.push_back(shape);

            delete[] item_buff;
            item_buff = NULL;
        }

        delete[] buffer;
    }
    else {

        OutputDebugString(L"Can't open data.bin to read");
    }

    in.close();
}
void openFileDialog()
{
    OPENFILENAME ofn;
    WCHAR szFilePath[MAX_PATH] = L"";
    WCHAR szFileTitle[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFilter = L"Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFilePath;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"bin";

    if (GetOpenFileName(&ofn))
    {
        // Do something usefull with the filename stored in szFileName 
        wstring ws(szFilePath);
        // your new String
        string fileName(ws.begin(), ws.end());
        loadFromBinaryFile(fileName);
        SetWindowText(text, ofn.lpstrFile);
        InvalidateRect(g_hWnd, NULL, TRUE);
    }
}

void saveFileDialog()
{
    OPENFILENAME ofn;
    WCHAR szFileName[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFilter = L"Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"bin";

    if (GetSaveFileName(&ofn))
    {
        // Do something usefull with the filename stored in szFileName 
        wstring ws(szFileName);
        // your new String
        string fileName(ws.begin(), ws.end());

        saveToBinaryFile(fileName);
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SIMPLEPAINT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEPAINT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEPAINT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SIMPLEPAINT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LPARAM param;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    param = lParam;
    switch (message)
    {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
        HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
        if (choose == 0)
        {
            HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        }
    case WM_CTLCOLOREDIT:
    {
        if ((HWND)lParam == GetDlgItem(hWnd, IDC_TEXTBOX))
        {
            SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
            SetTextColor((HDC)wParam, rgbCurrent);
            return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        }
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    // Lấy font hệ thống
    LOGFONT lf;
    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
        lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
        lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
        lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
        lf.lfPitchAndFamily, lf.lfFaceName);

    InitCommonControls();
    TBBUTTON tbButtons[] =
    {
        { STD_FILENEW,	ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_FILEOPEN,	ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_FILESAVE,	ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    };
    HWND hToolBarWnd = CreateToolbarEx(hwnd,
        WS_CHILD | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS,
        ID_TOOLBAR, sizeof(tbButtons) / sizeof(TBBUTTON), HINST_COMMCTRL,
        0, tbButtons, sizeof(tbButtons) / sizeof(TBBUTTON),
        BUTTON_WIDTH, BUTTON_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT,
        sizeof(TBBUTTON));

    TBBUTTON buttonsToAdd[] =
    {
        { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 }, // Nút phân cách
        { STD_CUT,	ID_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_COPY,	ID_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_PASTE, ID_EDIT_PASTE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_DELETE, ID_EDIT_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    };
    SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(buttonsToAdd) / sizeof(TBBUTTON),
        (LPARAM)(LPTBBUTTON)&buttonsToAdd);

    TBBUTTON userButtons[] =
    {
        { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 },
        { 0, ID_DRAW_ELLIPSE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 1, ID_DRAW_RECTANGLE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 2, ID_DRAW_LINE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 3, ID_DRAW_TEXT,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    };
    TBADDBITMAP	tbBitmap = { hInst, IDB_BITMAP1 };
    int idx = SendMessage(hToolBarWnd, TB_ADDBITMAP, (WPARAM)sizeof(tbBitmap) / sizeof(TBADDBITMAP),
        (LPARAM)(LPTBADDBITMAP)&tbBitmap);
    // Xác định chỉ mục hình ảnh của mỗi button từ ảnh bự liên hoàn nhiều tấm
    userButtons[1].iBitmap += idx;
    userButtons[2].iBitmap += idx;
    userButtons[3].iBitmap += idx;
    userButtons[4].iBitmap += idx;
    // Thêm nút bấm vào toolbar
    SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(userButtons) / sizeof(TBBUTTON),
        (LPARAM)(LPTBBUTTON)&userButtons);

    CreateWindowEx(NULL, L"STATIC", L"Current file:", WS_CHILD | WS_VISIBLE | SS_CENTER,
        265, 5, 200, 30, hwnd,
        (HMENU)ID_CURRENT, lpCreateStruct->hInstance, NULL);

    text=CreateWindowEx(NULL, L"STATIC", L"newfile.bin", WS_CHILD | WS_VISIBLE | SS_CENTER,
        400, 5, 350, 30, hwnd,
        (HMENU)ID_NEWFILE, lpCreateStruct->hInstance, NULL);

    return TRUE;
}
void initNewObject()
{
    switch (currShapeMode)
    {
    case PaintLibrary::LINE:
        currShape = new PaintLibrary::CLine();
        break;
    case PaintLibrary::DRAWRECT:
        currShape = new PaintLibrary::CRectangle();
        break;
    case PaintLibrary::DRAWELLIPSE:
        currShape = new PaintLibrary::CEllipse();
        break;
    case FILLRECT:
        break;
    case FILLELLIPSE:
        break;
    default:
        break;
    }
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    int i = 0;
    switch (id) {
    case ID_CHOOSE_COLOR:
        ZeroMemory(&cc, sizeof(CHOOSECOLOR));
        cc.lStructSize = sizeof(CHOOSECOLOR);
        cc.hwndOwner = hwnd;
        cc.lpCustColors = (LPDWORD)acrCustClr;
        cc.rgbResult = rgbCurrent;
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;
        if (ChooseColor(&cc))
        {
            hbrush = CreateSolidBrush(cc.rgbResult);
            rgbCurrent = cc.rgbResult;
        }
        break;
    case ID_CHOOSE_FONT:
        ZeroMemory(&cf, sizeof(CHOOSEFONT));
        cf.lStructSize = sizeof(CHOOSEFONT);
        cf.hwndOwner = hwnd;
        cf.lpLogFont = &lf;
        cf.Flags = CF_SCREENFONTS | CF_EFFECTS;

        if (ChooseFont(&cf) == TRUE)
        {
            hfont = CreateFontIndirect(cf.lpLogFont);
            hfontPrev = SelectObject(hdc, hfont);
            rgbCurrent = cf.rgbColors;
            rgbPrev = SetTextColor(hdc, rgbCurrent);
        }
        break;
    case ID_DRAW_LINE:
        currShapeMode = PaintLibrary::LINE;
        choose = 3;
        break;
    case ID_DRAW_TEXT:
        choose = 4;
        break;
    case ID_DRAW_RECTANGLE:
        currShapeMode = PaintLibrary::DRAWRECT;
        choose = 2;
        break;
    case ID_DRAW_ELLIPSE:
        currShapeMode = PaintLibrary::DRAWELLIPSE;
        break;
    case IDM_ABOUT:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
        break;
    case IDM_EXIT:
        DestroyWindow(hwnd);
        break;
    case ID_FILE_NEW:
        saveToBinaryFile(defaultFilePath);
        defaultFilePath = "newfile.bin";
        shapes.clear();
        DestroyWindow(textbox);
        InvalidateRect(hwnd, NULL, TRUE);
        SetWindowText(text, L"newfile.bin");
        choose = 5;
        break;
    case ID_FILE_OPEN:
        openFileDialog();
        break;
    case ID_FILE_SAVE:
        saveFileDialog();
        break;
    case ID_EDIT_COPY:
        choose = 9;
        break;
    case ID_EDIT_CUT:
        choose = 8;
        break;
    case ID_EDIT_DELETE:
        shapes.clear();
        DestroyWindow(textbox);
        InvalidateRect(hwnd, NULL, TRUE);
        choose = 11;
        break;
    case ID_EDIT_PASTE:
        choose = 10;
        break;
    }
}

void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}

void OnPaint(HWND hwnd)
{
    RECT  rect;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    if (isDrawingTextbox) {
        HPEN hPen = CreatePen(PS_DASHDOT, 3, rgbCurrent);
        Rectangle(hdc, fromX, fromY, toX, toY);
        EndPaint(hwnd, &ps);
    }
    HPEN hPen = CreatePen(PS_DASHDOT, 3, rgbCurrent);
    for (int i = 0; i < shapes.size(); i++)
    {
        SelectObject(hdc, hPen);
        shapes[i]->Draw(hdc);
    }

    if (isDown)
    {
        SelectObject(hdc, hPen);
        currShape->SetData(p2.x, p2.y, p1.x, p1.y, rgbCurrent, currPenStyle);
        currShape->Draw(hdc);
    }
    EndPaint(hwnd, &ps);
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (choose == 0)
    {
        HDC hdc = GetDC(hwnd);
        SetPixel(hdc, x, y, RGB(255, 0, 0));
        ReleaseDC(hwnd, hdc);
    }
    if (choose == 4)
    {
        isDrawingTextbox = true;
        isPreview = true;
        isDrawing = true;
        fromX = x;
        fromY = y;
        HDC hdc = GetDC(hwnd);
        MoveToEx(hdc, x, y, NULL);
    }
    else
    {
        if (!isDown)
        {
            p1 = PaintLibrary::newPoint(GET_X_LPARAM(param), GET_Y_LPARAM(param));
            initNewObject();
            isDown = true;
        }
    }
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (choose == 4)
    {
        isPreview = false;

        if (isDrawingTextbox) {
            if (toY < fromY) {
                int temp = fromY;
                fromY = toY;
                toY = temp;
            }

            if (toX < fromX) {
                int temp = fromX;
                fromX = toX;
                toX = temp;
            }

            int width = abs(fromX - toX);
            int height = abs(fromY - toY);
            if (textbox != NULL) DestroyWindow(textbox);
            textbox = CreateWindowEx(
                NULL, L"EDIT", L"",
                WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE,
                fromX, fromY, width, height, hwnd, (HMENU)IDC_TEXTBOX, NULL, NULL
            );
            SendMessage(textbox, WM_SETFONT, (WPARAM)hfont, TRUE);
            choose = 0;
            isDrawingTextbox = false;
        }
    }
    else
    {
        isDown = false;
        p2 = PaintLibrary::newPoint(GET_X_LPARAM(param), GET_Y_LPARAM(param));

        switch (currShapeMode)
        {
        case PaintLibrary::LINE:
        {
            PaintLibrary::CLine* line = new PaintLibrary::CLine;
            line->SetData(p2.x, p2.y, p1.x, p1.y, currColor, currPenStyle);
            shapes.push_back(line);
        }
        break;
        case PaintLibrary::DRAWRECT:
        {
            PaintLibrary::CRectangle* rect = new PaintLibrary::CRectangle;
            rect->SetData(p1.x, p1.y, p2.x, p2.y, currColor, currPenStyle);
            shapes.push_back(rect);
        }
        break;
        case PaintLibrary::DRAWELLIPSE:
        {
            PaintLibrary::CEllipse* ellipse = new PaintLibrary::CEllipse;
            ellipse->SetData(p1.x, p1.y, p2.x, p2.y, currColor, currPenStyle);
            shapes.push_back(ellipse);
        }
        break;
        }
    }
}

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    x = GET_X_LPARAM(param);
    y = GET_Y_LPARAM(param);
    if (choose == 4)
    {
        if (isDrawing) {
            toX = x;
            toY = y;
            // Báo hiệu cần xóa đi toàn bộ màn hình
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
    else
    {
        if (isDown)
        {
            p2 = PaintLibrary::newPoint(x, y);
            InvalidateRect(hwnd, NULL, TRUE); 
        }
    }
}

