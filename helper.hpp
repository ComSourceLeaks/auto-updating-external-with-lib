#include <minwindef.h>

typedef int(*pDD_btn)(int btn);
typedef int(*pDD_whl)(int whl);
typedef int(*pDD_key)(int keycode, int flag);
typedef int(*pDD_mov)(int x, int y);
typedef int(*pDD_str)(char* str);
typedef int(*pDD_todc)(int vk);
typedef int(*pDD_movR)(int dx, int dy);

pDD_btn   DD_btn;
pDD_key   DD_key;
pDD_mov   DD_mov;
pDD_str   DD_str;
pDD_todc  DD_todc;
pDD_movR  DD_movR;
HMODULE hDll;

class Mouse
{
public:

	void init()
	{
		HMODULE hDll = LoadLibraryW(L"C:\\dd32695.x64.dll");

		if (hDll == nullptr)
		{
		}

		DD_btn = (pDD_btn)GetProcAddress(hDll, "DD_btn");
		DD_key = (pDD_key)GetProcAddress(hDll, "DD_key");
		DD_mov = (pDD_mov)GetProcAddress(hDll, "DD_mov");
		DD_str = (pDD_str)GetProcAddress(hDll, "DD_str");
		DD_todc = (pDD_todc)GetProcAddress(hDll, "DD_todc");
		DD_movR = (pDD_movR)GetProcAddress(hDll, "DD_movR");

		if (!(DD_btn && DD_key && DD_mov && DD_str && DD_todc && DD_movR))
		{
		}

		int st = DD_btn(0);
		if (st != 1)
		{
		};
	}
}; Mouse Input;