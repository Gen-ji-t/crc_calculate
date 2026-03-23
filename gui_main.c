#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include "crc.h"

#define IDC_RADIO_8   101
#define IDC_RADIO_16  102
#define IDC_RADIO_32  103
#define IDC_RADIO_64  104
#define IDC_BTN_FILE  105
#define IDC_BTN_CALC  106
#define IDC_EDIT_OUT  107

static HWND hRadio8, hRadio16, hRadio32, hRadio64, hBtnFile, hBtnCalc, hEditOut;
static WCHAR g_filepath[MAX_PATH] = L"";

static int get_selected_algo(void)
{
    if (SendMessage(hRadio8, BM_GETCHECK, 0, 0) == BST_CHECKED) return 8;
    if (SendMessage(hRadio16, BM_GETCHECK, 0, 0) == BST_CHECKED) return 16;
    if (SendMessage(hRadio64, BM_GETCHECK, 0, 0) == BST_CHECKED) return 64;
    return 32;
}

static void calculate_crc(const WCHAR *path)
{
    int algo = get_selected_algo();
    FILE *fp = _wfopen(path, L"rb");
    if (!fp) {
        MessageBoxW(NULL, L"无法打开文件", L"错误", MB_OK | MB_ICONERROR);
        return;
    }

    uint8_t buf[8192];
    size_t n;
    uint8_t c8 = 0x00;
    uint16_t c16 = 0xFFFF;
    uint32_t c32 = 0xFFFFFFFF;
    uint64_t c64 = 0xFFFFFFFFFFFFFFFFULL;

    while ((n = fread(buf, 1, 8192, fp)) > 0) {
        if (algo == 8) c8 = crc8_update(c8, buf, n);
        else if (algo == 16) c16 = crc16_update(c16, buf, n);
        else if (algo == 64) c64 = crc64_update(c64, buf, n);
        else c32 = crc32_update(c32, buf, n);
    }
    fclose(fp);

    WCHAR result[512];
    const WCHAR *filename = wcsrchr(path, L'\\');
    const WCHAR *slash = wcsrchr(path, L'/');
    if (slash && (!filename || slash > filename)) filename = slash;
    filename = filename ? filename + 1 : path;

    if (algo == 8)
        swprintf(result, 512, L"文件: %ls\r\n算法: CRC-8\r\n结果: 0x%02X", filename, c8);
    else if (algo == 16)
        swprintf(result, 512, L"文件: %ls\r\n算法: CRC-16\r\n结果: 0x%04X", filename, c16);
    else if (algo == 64)
        swprintf(result, 512, L"文件: %ls\r\n算法: CRC-64\r\n结果: 0x%016llX", filename, (unsigned long long)(c64 ^ 0xFFFFFFFFFFFFFFFFULL));
    else
        swprintf(result, 512, L"文件: %ls\r\n算法: CRC-32\r\n结果: 0x%08X", filename, c32 ^ 0xFFFFFFFF);

    SetWindowTextW(hEditOut, result);
}

static void select_file(void)
{
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"所有文件\0*.*\0";
    ofn.lpstrFile = g_filepath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        SetWindowTextW(hEditOut, L"已选择文件，请点击计算按钮");
        EnableWindow(hBtnCalc, TRUE);
    }
}

static void do_calculate(void)
{
    if (g_filepath[0] == L'\0') {
        MessageBoxW(NULL, L"请先选择文件", L"提示", MB_OK | MB_ICONINFORMATION);
        return;
    }
    calculate_crc(g_filepath);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE:
        hRadio8 = CreateWindowW(L"BUTTON", L"CRC-8", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                               20, 20, 80, 25, hwnd, (HMENU)IDC_RADIO_8, NULL, NULL);
        hRadio16 = CreateWindowW(L"BUTTON", L"CRC-16", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                110, 20, 80, 25, hwnd, (HMENU)IDC_RADIO_16, NULL, NULL);
        hRadio32 = CreateWindowW(L"BUTTON", L"CRC-32", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                200, 20, 80, 25, hwnd, (HMENU)IDC_RADIO_32, NULL, NULL);
        hRadio64 = CreateWindowW(L"BUTTON", L"CRC-64", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                290, 20, 80, 25, hwnd, (HMENU)IDC_RADIO_64, NULL, NULL);
        hBtnFile = CreateWindowW(L"BUTTON", L"选择文件", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                20, 60, 170, 30, hwnd, (HMENU)IDC_BTN_FILE, NULL, NULL);
        hBtnCalc = CreateWindowW(L"BUTTON", L"计算", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                210, 60, 170, 30, hwnd, (HMENU)IDC_BTN_CALC, NULL, NULL);
        hEditOut = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY,
                                20, 100, 360, 140, hwnd, (HMENU)IDC_EDIT_OUT, NULL, NULL);
        SendMessage(hRadio32, BM_SETCHECK, BST_CHECKED, 0);
        EnableWindow(hBtnCalc, FALSE);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BTN_FILE)
            select_file();
        else if (LOWORD(wParam) == IDC_BTN_CALC)
            do_calculate();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    (void)hPrevInstance;
    (void)lpCmdLine;

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"CRCToolClass";

    if (!RegisterClassExW(&wc)) return 1;

    HWND hwnd = CreateWindowExW(0, L"CRCToolClass", L"CRC 计算工具",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                               CW_USEDEFAULT, CW_USEDEFAULT, 420, 300,
                               NULL, NULL, hInstance, NULL);

    if (!hwnd) return 1;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

