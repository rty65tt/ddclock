#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>

using namespace Gdiplus;

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
  Color(255, 0, 0, 0), Color(255, 120, 120, 120), Color(255, 15, 15, 15)
};

static CLOCKCOLOR cth1 = {
  Color(255, 75, 75, 75), Color(255, 0, 0, 0), Color(255, 65, 65, 65)
};

static CLOCKCOLOR cth2 = {
  Color(255, 130, 130, 130), Color(255, 0, 0, 0), Color(255, 115, 115, 115)
};

static CLOCKCOLOR cth3 = {
  Color(255, 180, 180, 180), Color(255, 0, 0, 0), Color(255, 165, 165, 165)
};

struct PREFS {
  CLOCKCOLOR colors;
  RECT rc;
  HWND topmost;
  int popup;
};

VOID OnPaint(HDC, PREFS, FontFamily);
VOID LoadSaveSettings(BOOL, PREFS);

VOID OnPaint(HDC *hdc, PREFS *p_prefs, FontFamily *fF)
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
    
	float k = 1.05f;

    float fsz = p_prefs->rc.right * k/4.1f;
    if (fsz > p_prefs->rc.bottom * k)
        fsz = p_prefs->rc.bottom * k;
    int sx = (int)(p_prefs->rc.right * k - p_prefs->rc.right);
    int sy = (int)(p_prefs->rc.bottom * k - p_prefs->rc.bottom);

    Gdiplus::Font dgtFont(fF, fsz, FontStyleRegular, UnitPixel);

    HDC hDCMem = CreateCompatibleDC(*hdc);
    HBITMAP hBmp = CreateCompatibleBitmap(*hdc, p_prefs->rc.right, p_prefs->rc.bottom);
    HGDIOBJ hTmp = SelectObject(hDCMem, hBmp);

    Graphics graphics(hDCMem);
    graphics.Clear(p_prefs->colors.bg);
    graphics.SetSmoothingMode(SmoothingModeHighQuality);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

    SolidBrush fgBrush(p_prefs->colors.fg);
    SolidBrush ldBrush(p_prefs->colors.ld);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    RectF layoutRect(7.0f-sx, 0.0f, p_prefs->rc.right+sx, p_prefs->rc.bottom+2*sy);
    graphics.DrawString(ledstr,8,&dgtFont,layoutRect,&format,&ldBrush);
    graphics.DrawString(buffer,8,&dgtFont,layoutRect,&format,&fgBrush);

    RectF ellipseRect(2.0f, 2.0f, 5.0f, 5.0f);
    SolidBrush *crBrush = (p_prefs->topmost == HWND_TOPMOST) ? &fgBrush : &ldBrush;
    graphics.FillEllipse(crBrush,ellipseRect);
/*
    for (int i=0; i < 3; i++) {
      RectF ellipseRect(2.0f, 7.0f* i + 10, 5.0f, 5.0f);
       crBrush = (settclr == i) ? &fgBrush : &ldBrush;
       graphics.FillEllipse(crBrush,ellipseRect);
    }*/

    BitBlt(*hdc, 0, 0, p_prefs->rc.right, p_prefs->rc.bottom, hDCMem, 0, 0, SRCCOPY);
  
    SelectObject(*hdc, hTmp);
    DeleteObject(hBmp);
    DeleteDC(hDCMem);
    
}


void LoadSaveSettings(BOOL do_save, PREFS *p_prefs)
{
    HKEY key;
    DWORD disposition;
    DWORD type = REG_DWORD, size = sizeof(REG_DWORD);

    CLOCKCOLOR ccolor;

    RECT rc       = p_prefs->rc;
    HWND topmost  = p_prefs->topmost;
    int popup     = p_prefs->popup;

    if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\DDClock", 0, NULL, 0, KEY_WRITE | KEY_READ, NULL, &key, &disposition) == ERROR_SUCCESS)
    {
        if (do_save)
        {
            ccolor = p_prefs->colors;
            RegSetValueEx(key, "bgcolor", 0, type, (PBYTE)&ccolor.bg, size);
            RegSetValueEx(key, "fgcolor", 0, type, (PBYTE)&ccolor.fg, size);
            RegSetValueEx(key, "ldcolor", 0, type, (PBYTE)&ccolor.ld, size);

            RegSetValueEx(key, "w_left", 0, type,   (PBYTE)&rc.left, size);
            RegSetValueEx(key, "w_right", 0, type,  (PBYTE)&rc.right, size);
            RegSetValueEx(key, "w_top", 0, type,    (PBYTE)&rc.top, size);
            RegSetValueEx(key, "w_bottom", 0, type, (PBYTE)&rc.bottom, size);
            RegSetValueEx(key, "topmost", 0, type,  (PBYTE)&topmost, size);
            RegSetValueEx(key, "popup", 0, type,    (PBYTE)&popup, size);
        }
        else
        {
            RegQueryValueEx(key, "bgcolor", 0, &type, (PBYTE)&ccolor.bg, &size);
            RegQueryValueEx(key, "fgcolor", 0, &type, (PBYTE)&ccolor.fg, &size);
            RegQueryValueEx(key, "ldcolor", 0, &type, (PBYTE)&ccolor.ld, &size);

            RegQueryValueEx(key, "w_left", 0, &type,   (PBYTE)&rc.left, &size);
            RegQueryValueEx(key, "w_right", 0, &type,  (PBYTE)&rc.right, &size);
            RegQueryValueEx(key, "w_top", 0, &type,    (PBYTE)&rc.top, &size);
            RegQueryValueEx(key, "w_bottom", 0, &type, (PBYTE)&rc.bottom, &size);
            RegQueryValueEx(key, "topmost", 0, &type,  (PBYTE)&topmost, &size);
            RegQueryValueEx(key, "popup", 0, &type,    (PBYTE)&popup, &size);

            p_prefs->colors = ccolor;

            p_prefs->rc      = rc;

            p_prefs->topmost = topmost;
            p_prefs->popup   = popup;
        }
        RegCloseKey(key);
    }
}
