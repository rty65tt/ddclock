#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>

#include <Windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
//#include <math.h>
#include "resource.h"
#include "ddclock.h"

//#include <stdio.h>
#include <wchar.h>
//#include <iostream>

using namespace Gdiplus;

static FontFamily fontFamily[1];
static CLOCKCOLOR *themes[] = {&def_color, &cth1, &cth2, &cth3};

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("DesktopDigitalClock");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HWND hWnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0);
    wincl.hIconSm = NULL;
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hWnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Desktop Digital Clock"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           WIN_WIDTH,                 /* The programs width */
           WIN_HIGHT,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hWnd, nCmdShow);

    /* Add Font Digital-7 from Recource *//////////////////
    Gdiplus::PrivateFontCollection m_fontcollection;

    HRSRC res = FindResource(hThisInstance,
        MAKEINTRESOURCE(IDR_MYFONT),"BINARY");
    if (res) 
      {
        HGLOBAL mem = LoadResource(hThisInstance, res);
        void *data = LockResource(mem);
        size_t len = SizeofResource(hThisInstance, res);
        m_fontcollection.AddMemoryFont(data,len);
      }

    int nNumFound=0;
    m_fontcollection.GetFamilies(1,fontFamily,&nNumFound);
    /////////////////////////////////////////////////////////

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    static HDC          hdc;      // device-context handle  
    static RECT         rc;       // RECT structure  
    static PAINTSTRUCT  ps;

    static int ix, iy;
    static int xm, ym;

    static int cth_n = 0;

    static bool changes = FALSE;

    GetClientRect(hWnd, &rc);

    static PREFS c_prefs = {def_color, rc, 0, 0};

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
        {
          SetTimer(hWnd, TIMER, 1000, NULL);

          LoadSaveSettings(FALSE, &c_prefs);

          RECT wrc, crc;

          GetWindowRect(hWnd, &wrc);
          GetClientRect(hWnd, &crc);
          ix = (wrc.right - wrc.left - crc.right)/2;
          iy = wrc.bottom - wrc.top - crc.bottom - ix;

          LONG popup = (c_prefs.popup) ? WS_POPUP : WS_OVERLAPPEDWINDOW;
          SetWindowLong(hWnd, GWL_STYLE, popup);

          if (c_prefs.rc.right == 0) {
              c_prefs = {def_color, wrc, 0, 0};
          }

          SetWindowPos(hWnd, c_prefs.topmost,
            c_prefs.rc.left,
            c_prefs.rc.top,
            c_prefs.rc.right - c_prefs.rc.left,
            c_prefs.rc.bottom - c_prefs.rc.top,
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
              if (changes) {
                changes = FALSE;
                GetWindowRect(hWnd, &c_prefs.rc);
                LoadSaveSettings(TRUE, &c_prefs);
              }
              RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
              break;
          }
        break;
        case WM_PAINT:
          {
            hdc = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &c_prefs.rc);
            OnPaint(&hdc, &c_prefs, &fontFamily[0]);
            EndPaint(hWnd, &ps);
          }
        break;
        case WM_RBUTTONDOWN:
          {
            cth_n = cth_n > 2 ? 0 :cth_n+1;
            c_prefs.colors = *themes[cth_n];
            changes = TRUE;
            RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
          }
          break;
        case WM_LBUTTONDOWN:
        {
          GetWindowRect(hWnd, &c_prefs.rc);
          if (c_prefs.popup) {
            xm = GET_X_LPARAM(lParam);
            ym = GET_Y_LPARAM(lParam);
           // SetCapture(hWnd);
          }
          return 0;
        }
        case WM_LBUTTONDBLCLK:
        {
          xm = GET_X_LPARAM(lParam); ///|||!!!11????????????????
          ym = GET_Y_LPARAM(lParam); ///|||!!!11????????????
          int x,y,w,h;
          RECT crc;
          GetWindowRect(hWnd, &c_prefs.rc);
          GetClientRect(hWnd, &crc);
          if (c_prefs.popup) {
            x = c_prefs.rc.left - ix;
            y = c_prefs.rc.top - iy;
            w = crc.right + 2*ix ;
            h = crc.bottom + ix + iy;
            c_prefs.popup = 0;
          }
          else {
            x = c_prefs.rc.left + ix;
            y = c_prefs.rc.top + iy;
            w = crc.right;
            h = crc.bottom;
            c_prefs.popup = 1;
          }
          LONG popup = (c_prefs.popup) ? WS_POPUP : WS_OVERLAPPEDWINDOW;
          SetWindowLong(hWnd, GWL_STYLE, popup);
          SetWindowPos(hWnd, c_prefs.topmost, x, y, w, h,SWP_SHOWWINDOW);
          changes = TRUE;
          //GetWindowRect(hWnd, &c_prefs.rc);
          //RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
        }
        break;
        case WM_MOUSEMOVE: 
        { 
          if (c_prefs.popup & (DWORD)wParam & MK_LBUTTON)
          {
            RECT cc_rc;
            int x,y,w,h,m;
            GetWindowRect(hWnd, &c_prefs.rc);
            GetClientRect(hWnd, &cc_rc);
            m = GET_X_LPARAM(lParam) - xm;
            x = c_prefs.rc.left + m;
            m = GET_Y_LPARAM(lParam) - ym;
            y = c_prefs.rc.top + m;
            w = cc_rc.right;
            h = cc_rc.bottom;
            
            MoveWindow(hWnd, x ,y, w, h, TRUE);
            changes = TRUE;
          }
          return 0;
        }
        case WM_KEYDOWN: 
        {
            if ((DWORD)wParam == 0x54)
            {
                  int x,y,w,h;
                  RECT wrc;
                  GetWindowRect(hWnd, &wrc);
                  x = wrc.left;
                  y = wrc.top;
                  w = wrc.right - wrc.left;
                  h = wrc.bottom - wrc.top;
                  c_prefs.topmost = (c_prefs.topmost == HWND_TOPMOST) ? HWND_NOTOPMOST : HWND_TOPMOST;
                  SetWindowPos(hWnd, c_prefs.topmost, x, y, w, h,SWP_SHOWWINDOW);
                  changes = TRUE;
            }
            RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
            if ((DWORD)wParam == 0x51)
            {
              SendMessage(hWnd, WM_DESTROY, 0, 0);
            }
        }
        break;
        case WM_SIZING:
            RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
            break;
        case WM_EXITSIZEMOVE:
            changes = TRUE;
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hWnd, message, wParam, lParam);
    }

    return 0;
}

