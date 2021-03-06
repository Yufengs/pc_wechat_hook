// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <Windows.h>
#include "resource.h"
#include <stdlib.h>
#include <string.h>
#include "commctrl.h"
#include <TlHelp32.h>
#include <wchar.h>
DWORD WINAPI ThreadProc(
	_In_ HMODULE hModule
);

HMODULE getWeChatWinAdd();

struct wxMyUserInfo
{
	wchar_t * wxid;
	wchar_t * wxUsername;
	wchar_t * wxNick;
	wchar_t * wxHeadPic;
	wchar_t * wxBindPhone;
	wchar_t * wxProvince;
	wchar_t * wxCity;
	wchar_t * wxDevice;
};
wxMyUserInfo getUserInfo();

INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, hModule, 0, NULL);
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

DWORD WINAPI ThreadProc(
	_In_ HMODULE hModule
)
{
	DialogBoxA(hModule, (LPCSTR)MAKEINTRESOURCE(IDD_DIALOG1), NULL, &DialogProc);
	return TRUE;
}



INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		wxMyUserInfo userInfo = getUserInfo();
		SetDlgItemText(hwndDlg, ID_WXID, userInfo.wxid);
		SetDlgItemText(hwndDlg, ID_WXNAME, userInfo.wxNick);
		SetDlgItemText(hwndDlg, WX_HEAD_PIC, userInfo.wxHeadPic);
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}


wchar_t * UTF8ToUnicode(const char* str)
{
	int    textlen = 0;
	wchar_t * result;
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	result = (wchar_t *)malloc((textlen + 1) * sizeof(wchar_t));
	memset(result, 0, (textlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
	return    result;
}

//获取基址
HMODULE getWeChatWinAdd()
{
	HMODULE address = LoadLibrary(L"WeChatWin.dll");
	return address;
}

wxMyUserInfo getUserInfo()
{
	//获取dll基址
	DWORD WechatWin = (DWORD)getWeChatWinAdd();
	wchar_t buff[0x100] = { 0 };
	wxMyUserInfo userInfo = { 0 };
	userInfo.wxid = UTF8ToUnicode((const char *)WechatWin + 0x1131B78);
	if (wcslen(userInfo.wxid) < 0x6) {
		DWORD pWxid = WechatWin + 0x1131B78;
		pWxid = *((DWORD *)pWxid);
		userInfo.wxid = UTF8ToUnicode((const char *)pWxid);
	}
	userInfo.wxUsername = UTF8ToUnicode((const char *)WechatWin + 0x1131DC8);
	userInfo.wxNick = UTF8ToUnicode((const char *)WechatWin + 0x1131C64);
	userInfo.wxBindPhone = UTF8ToUnicode((const char *)WechatWin + 0x1131C98);
	userInfo.wxDevice = UTF8ToUnicode((const char *)WechatWin + 0x1102218);
	DWORD pHeadPic = WechatWin + 0x1131F44;
	//1131F2C
	pHeadPic = *((DWORD *)pHeadPic);
	userInfo.wxHeadPic = UTF8ToUnicode((const char *)pHeadPic);
	//swprintf_s(buff,L"pic=%p add=%s", userInfo.wxHeadPic, userInfo.wxHeadPic);
	//MessageBox(NULL, buff,L"标题啊",MB_OK);
	return userInfo;
	//*((LPVOID *)receiveWxid
	//1101E4C nick
	//1101E4C 1101DD4 + 0x14 F非指针 1F指针
	//1101E80 phone
	//1102114 head
	//1101FB0 name
}