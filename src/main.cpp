#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
//#include <math.h>
#include "resource.h"

//#include <stdio.h>

using namespace Gdiplus;

#pragma comment (lib, "comctl32.lib")
#pragma comment(lib, "gdiplus.lib")

#define TIMER 1

#define WIN_WIDTH 360
#define WIN_HIGHT 120
#define WIN_WIDTH_MIN 175
#define WIN_HIGHT_MIN 75

struct CLOCKCOLOR
{
  Color bg;
  Color fg;
  Color ld;
};

static CLOCKCOLOR def_color = {
  Color(255, 0, 0, 0),
  Color(255, 120, 120, 120),
  Color(255, 15, 15, 15)
};

static CLOCKCOLOR color1 = {
  Color(255, 75, 75, 75),
  Color(255, 0, 0, 0),
  Color(255, 65, 65, 65)
};

static CLOCKCOLOR color2 = {
  Color(255, 130, 130, 130),
  Color(255, 0, 0, 0),
  Color(255, 115, 115, 115)
};

static CLOCKCOLOR color3 = {
  Color(255, 180, 180, 180),
  Color(255, 0, 0, 0),
  Color(255, 165, 165, 165)
};


static CLOCKCOLOR ccolor = def_color;

static CLOCKCOLOR *arrthemes[] = {
  &def_color,
  &color1,
  &color2,
  &color3
};

static Color *slctcolor = &ccolor.bg;
static Color *clrarr[]  = {&ccolor.bg, &ccolor.fg, &ccolor.ld};

static int settclr  = 0;
static int w_popup = 0;
static HWND topmost = HWND_NOTOPMOST;

static RECT cw_rc;

static   FontFamily fontFamily[1];

// объявление функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
VOID OnPaint(HDC, RECT);
void LoadSaveSettings(BOOL);

void LoadSaveSettings(BOOL do_save)
{
    HKEY key;
    DWORD disposition;
    DWORD type = REG_DWORD, size = sizeof(REG_DWORD);

    if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\DDClock", 0, NULL, 0, KEY_WRITE | KEY_READ, NULL, &key, &disposition) == ERROR_SUCCESS)
    {
        if (do_save)
        {
            RegSetValueEx(key, "bg.color", 0, type, (PBYTE)&ccolor.bg, size);
            RegSetValueEx(key, "fg.color", 0, type, (PBYTE)&ccolor.fg, size);
            RegSetValueEx(key, "ld.color", 0, type, (PBYTE)&ccolor.ld, size);

            RegSetValueEx(key, "w_left", 0, type, (PBYTE)&cw_rc.left, size);
            RegSetValueEx(key, "w_right", 0, type, (PBYTE)&cw_rc.right, size);
            RegSetValueEx(key, "w_top", 0, type, (PBYTE)&cw_rc.top, size);
            RegSetValueEx(key, "w_bottom", 0, type, (PBYTE)&cw_rc.bottom, size);
            RegSetValueEx(key, "topmost", 0, type, (PBYTE)&topmost, size);
            RegSetValueEx(key, "popup", 0, type, (PBYTE)&w_popup, size);
        }
        else
        {
            RegQueryValueEx(key, "bg.color", 0, &type, (PBYTE)&ccolor.bg, &size);
            RegQueryValueEx(key, "fg.color", 0, &type, (PBYTE)&ccolor.fg, &size);
            RegQueryValueEx(key, "ld.color", 0, &type, (PBYTE)&ccolor.ld, &size);

            RegQueryValueEx(key, "w_left", 0, &type, (PBYTE)&cw_rc.left, &size);
            RegQueryValueEx(key, "w_right", 0, &type, (PBYTE)&cw_rc.right, &size);
            RegQueryValueEx(key, "w_top", 0, &type, (PBYTE)&cw_rc.top, &size);
            RegQueryValueEx(key, "w_bottom", 0, &type, (PBYTE)&cw_rc.bottom, &size);
            RegQueryValueEx(key, "topmost", 0, &type, (PBYTE)&topmost, &size);
            RegQueryValueEx(key, "popup", 0, &type, (PBYTE)&w_popup, &size);
        }
        RegCloseKey(key);
    }
}

VOID OnPaint(HDC *hdc, RECT *rc)
{
  SYSTEMTIME st;
  GetLocalTime(&st);
  WCHAR ledstr[] = L"88:88:88";
  WCHAR buffer1[] = L"00:00:00";
  WCHAR buffer2[] = L"00 00 00";
  WCHAR *buffer =  (st.wSecond % 2) ? buffer1: buffer2;

  WCHAR dchar[] = L"0123456789";
  buffer[0]=         dchar[st.wHour/10];
  buffer[1]=         dchar[st.wHour%10];
  buffer[3]=         dchar[st.wMinute/10];
  buffer[4]=         dchar[st.wMinute%10];
  buffer[6]=         dchar[st.wSecond/10];
  buffer[7]=         dchar[st.wSecond%10];
   
  float k = 1.05;

  int fsz = rc->right*k/4.1;
  if (fsz > rc->bottom*k)
     fsz = rc->bottom*k;

   int sx = rc->right*k - rc->right;
   int sy = rc->bottom*k - rc->bottom;

  HDC hDCMem = CreateCompatibleDC(*hdc);
  HBITMAP hBmp = CreateCompatibleBitmap(*hdc, rc->right, rc->bottom);
  HGDIOBJ hTmp = SelectObject(hDCMem, hBmp);

  Graphics graphics(hDCMem);
  graphics.Clear(ccolor.bg);
  graphics.SetSmoothingMode(SmoothingModeHighQuality);
  graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

  Font myFont(&fontFamily[0], fsz, FontStyleRegular, UnitPixel);
  SolidBrush fgBrush(ccolor.fg);
  SolidBrush ledBrush(ccolor.ld);
  StringFormat format;
  format.SetAlignment(StringAlignmentCenter);
  format.SetLineAlignment(StringAlignmentCenter);
  RectF layoutRect(7.0f-sx, 0.0f, rc->right+sx, rc->bottom+2*sy);
  graphics.DrawString(ledstr,8,&myFont,layoutRect,&format,&ledBrush);
  graphics.DrawString(buffer,8,&myFont,layoutRect,&format,&fgBrush);

  RectF ellipseRect(2.0f, 2.0f, 5.0f, 5.0f);
  SolidBrush *cyrBrush = (topmost == HWND_TOPMOST) ? &fgBrush : &ledBrush;
  graphics.FillEllipse(cyrBrush,ellipseRect);

  for (int i=0; i < 3; i++) {
    RectF ellipseRect(2.0f, 7.0f* i + 10, 5.0f, 5.0f);
     cyrBrush = (settclr == i) ? &fgBrush : &ledBrush;
     graphics.FillEllipse(cyrBrush,ellipseRect);
  }

  BitBlt(*hdc, 0, 0, rc->right, rc->bottom, hDCMem, 0, 0, SRCCOPY);

  SelectObject(*hdc, hTmp);
  DeleteObject(hBmp);
  DeleteDC(hDCMem);
}

// функция вхождений программы WinMain
int APIENTRY WinMain(HINSTANCE hInstance,
             HINSTANCE         hPrevInstance,
             LPSTR             lpCmdLine,
             int               nCmdShow)
{


  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  // имя будущего класса
  LPCTSTR lpzClass = TEXT("Desktop Digit Clock Class!");

  // регистрация класса
  if (!RegMyWindowClass(hInstance, lpzClass))
    return 1;


  // вычисление координат центра экрана
  RECT screen_rect;
  GetWindowRect(GetDesktopWindow(),&screen_rect); // разрешение экрана
  int x = screen_rect.right - WIN_WIDTH-5;
  int y = 120;

  // создание диалогового окна
  HWND hWnd = CreateWindow(lpzClass, TEXT("Desktop Digit Clock"), 
    WS_VISIBLE | WS_OVERLAPPEDWINDOW, x, y, WIN_WIDTH, WIN_HIGHT, NULL, NULL, 
    hInstance, NULL);


  // если окно не создано, описатель будет равен 0
  if(!hWnd) return 2;

Gdiplus::PrivateFontCollection m_fontcollection;

HRSRC res = FindResource(hInstance,
    MAKEINTRESOURCE(IDR_MYFONT),"BINARY");
if (res) 
  {
    HGLOBAL mem = LoadResource(hInstance, res);
    void *data = LockResource(mem);
    size_t len = SizeofResource(hInstance, res);
    Gdiplus::Status nResults = m_fontcollection.AddMemoryFont(data,len);
  }

    int nNumFound=0;
    m_fontcollection.GetFamilies(1,fontFamily,&nNumFound);


  // цикл сообщений приложения
  MSG msg = {0};    // структура сообщения
  int iGetOk = 0;   // переменная состояния
  while ((iGetOk = GetMessage(&msg, NULL, 0, 0 )) != 0) // цикл сообщений
  {
    if (iGetOk == -1) return 3;  // если GetMessage вернул ошибку - выход
    TranslateMessage(&msg);    
    DispatchMessage(&msg);
  }
  return msg.wParam;  // возвращаем код завершения программы
}

////////////////////////////////////////////////////////////////////////// 
// функция регистрации класса окон
ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
  WNDCLASS wcWindowClass = {0};
  // адрес ф-ции обработки сообщений
  wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
  // стиль окна
  wcWindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
  // дискриптор экземпляра приложения
  wcWindowClass.hInstance = hInst;
  // название класса
  wcWindowClass.lpszClassName = lpzClassName;
  // загрузка курсора
  wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  // загрузка цвета окон
  wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
  return RegisterClass(&wcWindowClass); // регистрация класса
}

////////////////////////////////////////////////////////////////////////// 
// функция обработки сообщений
LRESULT CALLBACK WndProc(
  HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static HDC          hdc;      // device-context handle  
  static RECT         rc;       // RECT structure  
  static UINT         uTimer;   // timer identifier
  static PAINTSTRUCT ps;


  static RECT         cc_rc; // RECT structure

  static int ix, iy;
  static int xm, ym;
  static int color_n = 0;

  switch (message)
  {
    case WM_CREATE:
    {
      uTimer = SetTimer(hWnd, TIMER, 1000, NULL);

      LoadSaveSettings(FALSE);
      RECT wrc, crc;
      GetWindowRect(hWnd, &wrc);
      GetClientRect(hWnd, &crc);
      ix = (wrc.right - wrc.left - crc.right)/2;
      iy = wrc.bottom - wrc.top - crc.bottom - ix;
      LONG popup = (w_popup) ? WS_POPUP : WS_OVERLAPPEDWINDOW;
      SetWindowLong(hWnd, GWL_STYLE, popup);

      if (cw_rc.right == 0) {
        cw_rc = wrc;
      }

      SetWindowPos(hWnd, topmost,
        cw_rc.left,
        cw_rc.top,
        cw_rc.right - cw_rc.left,
        cw_rc.bottom - cw_rc.top,
        SWP_SHOWWINDOW);

      return 0;
    }
    case WM_GETMINMAXINFO:
    {
      /* Prevent our window from being sized too small */
      MINMAXINFO *minMax = (MINMAXINFO*)lParam;
      minMax->ptMinTrackSize.x = WIN_WIDTH_MIN;
      minMax->ptMinTrackSize.y = WIN_HIGHT_MIN;
      return 0;
    }

    case WM_TIMER:
      switch (wParam)
      {
      case TIMER:
        RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
      //  InvalidateRect(hWnd, &rc, FALSE);
        break;
      }
    break;
  case WM_PAINT:
    {
      hdc = BeginPaint(hWnd, &ps);
      GetClientRect(hWnd, &rc);
      OnPaint(&hdc, &rc);
      EndPaint(hWnd, &ps);
    }
    break;
    case WM_LBUTTONDBLCLK:
    {
      xm = GET_X_LPARAM(lParam); ///|||!!!11????????????????
      ym = GET_Y_LPARAM(lParam); ///|||!!!11????????????
      int x,y,w,h;
      RECT crc;
      GetClientRect(hWnd, &crc);
      if (w_popup) {
        x = cw_rc.left - ix;
        y = cw_rc.top - iy;
        w = crc.right + 2*ix ;
        h = crc.bottom + ix + iy;
        SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        w_popup = 0;
      }
      else {
        x = cw_rc.left + ix;
        y = cw_rc.top + iy;
        w = crc.right;
        h = crc.bottom;
        SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
        w_popup = 1;
      }
      SetWindowPos(hWnd, topmost, x, y, w, h,SWP_SHOWWINDOW);
      GetWindowRect(hWnd, &cw_rc);
      //RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
    }
    break;
  case WM_LBUTTONDOWN:
  {
    GetWindowRect(hWnd, &cw_rc);
    if (w_popup) {
      xm = GET_X_LPARAM(lParam);
      ym = GET_Y_LPARAM(lParam);
     // SetCapture(hWnd);
    }
    return 0;
  }
  case WM_KEYDOWN: 
    {
    if ((DWORD)wParam == 0x54)
    {
      int x,y,w,h;
      RECT wrc, crc;
      GetWindowRect(hWnd, &wrc);
      GetClientRect(hWnd, &crc);
      x = wrc.left;
      y = wrc.top;
      w = wrc.right - wrc.left;
      h = wrc.bottom - wrc.top;
      topmost = (topmost == HWND_TOPMOST) ? HWND_NOTOPMOST : HWND_TOPMOST;
      SetWindowPos(hWnd, topmost, x, y, w, h,SWP_SHOWWINDOW);
    }
    RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
    if ((DWORD)wParam == 0x51)
    {
      SendMessage(hWnd, WM_DESTROY, 0, 0);
    }
    return 0;
  }
  case WM_LBUTTONUP: 
  {
    if (w_popup) {
     // ReleaseCapture(); 
    }
    return 0;
  }
  case WM_MOUSEMOVE: 
  { 
    if (w_popup & (DWORD)wParam & MK_LBUTTON)
    {
      int x,y,w,h,m;      
      GetWindowRect(hWnd, &cw_rc);
      GetClientRect(hWnd, &cc_rc);
      m = GET_X_LPARAM(lParam) - xm;
      x = cw_rc.left + m;
      m = GET_Y_LPARAM(lParam) - ym;
      y = cw_rc.top + m;
      w = cc_rc.right;
      h = cc_rc.bottom;
      MoveWindow(hWnd, x ,y, w, h, TRUE);
    }
    return 0;
  }
  case WM_RBUTTONDOWN:
    {
      color_n = color_n > 2 ? 0 :++color_n;
      ccolor = *arrthemes[color_n];
      RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
    }
    break;
  case WM_MBUTTONDOWN:
    {
      settclr = (settclr > 1) ? 0 : ++settclr; 
      slctcolor = clrarr[settclr];
      RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
    }
    break;
  case WM_MOUSEWHEEL:
    {
    BYTE   r = slctcolor->GetR();
    int i = GET_WHEEL_DELTA_WPARAM(wParam);
    if (i > 0) {
      r = r < 255 ? ++r : 255;
    }
    else {
      r = r > 0 ? --r : 0;
    }
      *slctcolor = Color(255, r, r, r);
      RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
    }
    break;

  case WM_DESTROY:
  GetWindowRect(hWnd, &cw_rc);
    LoadSaveSettings(TRUE);
    PostQuitMessage(0);  // реакция на сообщение
    break;
  default:  
    // все сообщения не обработанные Вами обработает сама Windows
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
