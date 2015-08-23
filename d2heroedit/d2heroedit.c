// Created by Kamran Poursohrab 2013

#include "../d2savelib/d2save.h"
#include "resource.h"
#include "version.h"

#include <windows.h>

static d2save_t		d2s;
static HWND			hDlg;

static BOOL _OpenSave(void)
{
	OPENFILENAMEA ofna			= {0};
	CHAR szSavePath[MAX_PATH]	= {0};
	BOOL bRet					= FALSE;

	if (NULL != d2s) {
		d2save_free(&d2s);
	}

	ofna.lStructSize			= sizeof(ofna);
	ofna.lpstrFilter			= "D2 Save (.d2s)\0*.d2s\0";
	ofna.Flags					= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofna.lpstrFile				= szSavePath;
	ofna.nMaxFile				= sizeof(szSavePath);

	if (FALSE != (bRet = GetOpenFileNameA(&ofna))) {
		bRet = (BOOL)!d2save_open(&d2s, szSavePath);
	}

	return bRet;
}

static void _ReadValues(void)
{
	int val = 0;
	CHAR szName[16];

	d2save_get_died(d2s, &val);
	SendMessage(GetDlgItem(hDlg, IDC_DIED), BM_SETCHECK, (0 != val)? BST_CHECKED : BST_UNCHECKED, 0);
	
	d2save_get_hardcore(d2s, &val);
	SendMessage(GetDlgItem(hDlg, IDC_HARDCORE), BM_SETCHECK, (0 != val)? BST_CHECKED : BST_UNCHECKED, 0);

	d2save_get_expansion(d2s, &val);
	SendMessage(GetDlgItem(hDlg, IDC_EXPANSION), BM_SETCHECK, (0 != val)? BST_CHECKED : BST_UNCHECKED, 0);

	PlaySoundA((LPCSTR)IDR_WAVE2, NULL, SND_RESOURCE | SND_ASYNC);

	d2save_get_name(d2s, szName, sizeof(szName));
	SetWindowTextA(GetDlgItem(hDlg, IDC_NAME), szName);
}

static void _WriteValues(void)
{
	int val = 0;

	val = SendMessage(GetDlgItem(hDlg, IDC_DIED), BM_GETCHECK, 0, 0);
	d2save_set_died(d2s, val);

	val = SendMessage(GetDlgItem(hDlg, IDC_HARDCORE), BM_GETCHECK, 0, 0);
	d2save_set_hardcore(d2s, val);

	val = SendMessage(GetDlgItem(hDlg, IDC_EXPANSION), BM_GETCHECK, 0, 0);
	d2save_set_expansion(d2s, val);

	d2save_write(d2s);
	PlaySoundA((LPCSTR)IDR_WAVE1, NULL, SND_RESOURCE | SND_ASYNC);
}

static INT_PTR CALLBACK _DialogProc(HWND hwnd,
                          UINT message, 
                          WPARAM wParam, 
                          LPARAM lParam)
{
    switch (message)
	{
	case WM_INITDIALOG:
		{
			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));

			SendMessage(hwnd, WM_SETICON, ICON_BIG,		(LPARAM)hIcon);
			SendMessage(hwnd, WM_SETICON, ICON_SMALL,	(LPARAM)hIcon);

			DestroyIcon(hIcon);

			SetWindowTextA(hwnd, "D2 Hero Editor " VERSION_STRING);

			hDlg = hwnd;
			if (FALSE != _OpenSave()) {
				_ReadValues();
			}
			else {
				MessageBoxA(NULL, "Invalid save file", "Error", MB_ICONERROR);
			}
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_OPENSAVE:
			if (FALSE != _OpenSave()) {
				_ReadValues();
			}
			break;
		case IDC_APPLY:
			if (BN_CLICKED == HIWORD(wParam)) {
				_WriteValues();
			}
			break;
		default:
			break;
		}
		return TRUE;
    case WM_DESTROY:
		d2save_free(&d2s);
        PostQuitMessage(0);
        return TRUE;
    case WM_CLOSE:
        DestroyWindow (hwnd);
        return TRUE;
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	MSG  msg;

    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, _DialogProc);

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
