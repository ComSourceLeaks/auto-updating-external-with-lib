#define IMGUI_DEFINE_MATH_OPERATORS
#include "cache.cpp"
#include <D3D11.h>
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_settings.h"
#include "ImGui/font.h"
#include "ImGui/image.h"
#include "ImGui.hpp"
#include <D3DX11tex.h>
#include "particles.hpp"
#include "menucol.h"
#include "Config.h"
#include "lootesp.hpp"
#include "communication.h"
#include "crypter.h"
#include "OverlayHelper.h"
#include "lazyyyy.hpp"
#include "crypt23.h"
#include "gui.h"
#include "ImGui/custom.hpp"
#include "ImGui/bytes.hpp"
#include "ImGui/custom.hpp"
#define HJWND_PROGRAM L"odbcad32.exe"
#define MAX_CLASSNAME 255
#define MAX_WNDNAME 25
#define STB_IMAGE_IMPLEMENTATION
#pragma comment(lib, "D3DX11.lib")
#define ALPHA    ( ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )
#define NO_ALPHA ( ImGuiColorEditFlags_NoTooltip    | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )


void LoadFonts() {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("C:\Windows\Fonts\Arial.ttf", 72.0f); // Pfad zu Ihrer Schriftart-Datei
}
std::vector<DWORD> GetPIDs(std::wstring targetProcessName) {
	std::vector<DWORD> pids;
	if (targetProcessName == L"")
		return pids;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof entry;
	if (!Process32FirstW(snap, &entry)) {
		CloseHandle(snap);
		return pids;
	}
	do {
		if (std::wstring(entry.szExeFile) == targetProcessName) {
			pids.emplace_back(entry.th32ProcessID);
		}
	} while (Process32NextW(snap, &entry));
	CloseHandle(snap);
	return pids;
}

static int keystatusTrig = 0;



float MaxSmooth = 100;
struct WindowsFinderParams {
	DWORD pidOwner = NULL;
	std::wstring wndClassName = L"";
	std::wstring wndName = L"";
	RECT pos = { 0, 0, 0, 0 };
	POINT res = { 0, 0 };
	float percentAllScreens = 0.0f;
	float percentMainScreen = 0.0f;
	DWORD style = NULL;
	DWORD styleEx = NULL;
	bool satisfyAllCriteria = false;
	std::vector<HWND> hwnds;
};

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
	WindowsFinderParams& params = *(WindowsFinderParams*)lParam;

	unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

	// If looking for windows of a specific PID
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	if (params.pidOwner != NULL)
		if (params.pidOwner == pid)
			++satisfiedCriteria; // Doesn't belong to the process targeted
		else
			++unSatisfiedCriteria;

	// If looking for windows of a specific class
	wchar_t className[MAX_CLASSNAME] = L"";
	GetClassName(hwnd, className, MAX_CLASSNAME);
	std::wstring classNameWstr = className;
	if (params.wndClassName != L"")
		if (params.wndClassName == classNameWstr)
			++satisfiedCriteria; // Not the class targeted
		else
			++unSatisfiedCriteria;

	// If looking for windows with a specific name
	wchar_t windowName[MAX_WNDNAME] = L"";
	GetWindowText(hwnd, windowName, MAX_CLASSNAME);
	std::wstring windowNameWstr = windowName;
	if (params.wndName != L"")
		if (params.wndName == windowNameWstr)
			++satisfiedCriteria; // Not the class targeted
		else
			++unSatisfiedCriteria;

	// If looking for window at a specific position
	RECT pos;
	GetWindowRect(hwnd, &pos);
	if (params.pos.left || params.pos.top || params.pos.right || params.pos.bottom)
		if (params.pos.left == pos.left && params.pos.top == pos.top && params.pos.right == pos.right && params.pos.bottom == pos.bottom)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for window of a specific size
	POINT res = { pos.right - pos.left, pos.bottom - pos.top };
	if (params.res.x || params.res.y)
		if (res.x == params.res.x && res.y == params.res.y)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for windows taking more than a specific percentage of all the screens
	LONG ratioAllScreensX = res.x / GetSystemMetrics(SM_CXSCREEN);
	LONG ratioAllScreensY = res.y / GetSystemMetrics(SM_CYSCREEN);
	float percentAllScreens = (float)ratioAllScreensX * (float)ratioAllScreensY * 100;
	if (params.percentAllScreens != 0.0f)
		if (percentAllScreens >= params.percentAllScreens)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for windows taking more than a specific percentage or the main screen
	RECT desktopRect;
	GetWindowRect(GetDesktopWindow(), &desktopRect);
	POINT desktopRes = { desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top };
	LONG ratioMainScreenX = res.x / desktopRes.x;
	LONG ratioMainScreenY = res.y / desktopRes.y;
	float percentMainScreen = (float)ratioMainScreenX * (float)ratioMainScreenY * 100;
	if (params.percentMainScreen != 0.0f)
		if (percentAllScreens >= params.percentMainScreen)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
	if (params.style)
		if (params.style & style)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	LONG_PTR styleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	if (params.styleEx)
		if (params.styleEx & styleEx)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	if (satisfiedCriteria == 0)
	{
		return TRUE;

	}

	if (params.satisfyAllCriteria == 1 && unSatisfiedCriteria > 0)
	{
		return TRUE;
	}

	params.hwnds.push_back(hwnd);
	return TRUE;
}
HWND window_handle;

std::vector<HWND> WindowsFinder(WindowsFinderParams params) {
	EnumWindows(EnumWindowsCallback, (LPARAM)&params);
	return params.hwnds;
}
bool TerminateProcessByName(const std::wstring& processName) {
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) return false;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32)) {
		CloseHandle(hProcessSnap);
		return false;
	}

	do {
		if (pe32.szExeFile == processName) {
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
			if (hProcess == NULL) {
				CloseHandle(hProcessSnap);
				return false;
			}
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			CloseHandle(hProcessSnap);
			return true;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return false;
}

inline HWND HiJackNotepadWindow() {

	HWND hwnd = NULL;

	std::vector<DWORD> existingNotepads = GetPIDs((L"notepad.exe"));
	if (!existingNotepads.empty()) {
		for (int i(0); i < existingNotepads.size(); ++i) {
			// Terminating processes
			HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, existingNotepads[i]);
			TerminateProcess(hOldProcess, 0);
			CloseHandle(hOldProcess);
		}
	}

	system(("start notepad.exe")); 

	std::vector<DWORD> notepads = GetPIDs((L"notepad.exe"));
	if (notepads.empty() || notepads.size() > 1) 
		return hwnd;
	WindowsFinderParams params;
	params.pidOwner = notepads[0];
	params.style = WS_VISIBLE;
	params.satisfyAllCriteria = true;
	std::vector<HWND> hwnds;
	int attempt = 0; 
	while (hwnd == NULL || attempt > 50) {
		Sleep(50);
		hwnds = WindowsFinder(params);
		if (hwnds.size() > 1)
			return hwnd;
		hwnd = hwnds[0];
		++attempt;
	}
	if (!hwnd)
		return hwnd;

	SetMenu(hwnd, NULL);
	SetWindowLongPtr(hwnd, GWL_STYLE, WS_VISIBLE);
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW); // WS_EX_NOACTIVATE  and WS_EX_TOOLWINDOW removes it from taskbar

	SetWindowPos(hwnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
	screen_width = GetSystemMetrics(SM_CXSCREEN);
	screen_height = GetSystemMetrics(SM_CYSCREEN);
	return hwnd;
}


ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);
HWND windows = NULL;
HWND hwnd = NULL;
RECT GameRect = { NULL };

IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD ScreenCenterX;
DWORD ScreenCenterY;

ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_ctx;
IDXGISwapChain* d3d_swap_chain;
ID3D11RenderTargetView* d3d_render_target;
D3DPRESENT_PARAMETERS d3d_present_params;

void SetWindowToTarget()
{
	while (true)
	{
		if (windows)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(windows, &GameRect);
			screen_width = GameRect.right - GameRect.left;
			screen_height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(windows, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				screen_height -= 39;
			}
			ScreenCenterX = screen_width / 2;
			ScreenCenterY = screen_height / 2;
			MoveWindow(window_handle, GameRect.left, GameRect.top, screen_width, screen_height, true);
		}
		else
		{
			exit(0);
		}
	}
}


namespace pic
{
	ID3D11ShaderResourceView* background = nullptr;

	ID3D11ShaderResourceView* aimbot = nullptr;
	ID3D11ShaderResourceView* visuals = nullptr;
	ID3D11ShaderResourceView* skins = nullptr;
	ID3D11ShaderResourceView* settings = nullptr;

	ID3D11ShaderResourceView* combo_widget = nullptr;
	ID3D11ShaderResourceView* input_widget = nullptr;

	ID3D11ShaderResourceView* menu_settings_icon = nullptr;

	ID3D11ShaderResourceView* circle_success = nullptr;
	ID3D11ShaderResourceView* circle_error = nullptr;

}

namespace font
{
	ImFont* inter_bold = nullptr;
	ImFont* inter_default = nullptr;
	ImFont* icon = nullptr;
}


namespace n_render {
	class c_render {
	public:

		//static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
		auto imgui() -> bool {
			DXGI_SWAP_CHAIN_DESC swap_chain_description;
			ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
			swap_chain_description.BufferCount = 2;
			swap_chain_description.BufferDesc.Width = 0;
			swap_chain_description.BufferDesc.Height = 0;
			swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
			swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
			swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_description.OutputWindow = window_handle;
			swap_chain_description.SampleDesc.Count = 1;
			swap_chain_description.SampleDesc.Quality = 0;
			swap_chain_description.Windowed = 1;
			swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			D3D_FEATURE_LEVEL d3d_feature_lvl;

			const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

			D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, d3d_feature_array, 2, D3D11_SDK_VERSION, &swap_chain_description, &d3d_swap_chain, &d3d_device, &d3d_feature_lvl, &d3d_device_ctx);

			ID3D11Texture2D* pBackBuffer;


			d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

			d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);

			pBackBuffer->Release();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
			ImFontConfig icons_config;

			ImFontConfig CustomFont;
			CustomFont.FontDataOwnedByAtlas = false;

			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.OversampleH = 3;
			icons_config.OversampleV = 3;

			ImGui_ImplWin32_Init(window_handle);
			ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

			ImFontConfig font_config;
			font_config.OversampleH = 1;
			font_config.OversampleV = 1;
			font_config.PixelSnapH = 1;

			static const ImWchar ranges[] =
			{
				0x0020, 0x00FF,
				0x0400, 0x044F,
				0,
			};



			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			static ID3D11Device* g_pd3dDevice = nullptr;
			static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
			static IDXGISwapChain* g_pSwapChain = nullptr;
			static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
			static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
			D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
			ImFontConfig cfg;
			
			/*font_config.PixelSnapH = false;
			font_config.FontDataOwnedByAtlas = false;
			font_config.OversampleH = 5;
			font_config.OversampleV = 5;
			font_config.RasterizerMultiply = 1.2f;*/
			//static const ImWchar ranges[] = {

			//	0x0020, 0x00FF, // Basic Latin + Latin Supplement
			//	0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
			//	0x2DE0, 0x2DFF, // Cyrillic Extended-A
			//	0xA640, 0xA69F, // Cyrillic Extended-B
			//	0xE000, 0xE226, // icons
			//	0,
			//};
			//font_config.GlyphRanges = ranges;
			GameFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Impact.ttf", 14, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			FortniteFont = io.Fonts->AddFontFromMemoryTTF(icons_binary, sizeof icons_binary, 15, &cfg, io.Fonts->GetGlyphRangesCyrillic());


			font::inter_default = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			font::inter_bold = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			font::icon = io.Fonts->AddFontFromMemoryTTF(icon_font, sizeof(icon_font), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

			ImGui::StyleColorsDark();

			if (pic::background == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, background, sizeof(background), &info, pump, &pic::background, 0);

			if (pic::aimbot == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, aimbot, sizeof(aimbot), &info, pump, &pic::aimbot, 0);
			if (pic::visuals == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, visuals, sizeof(visuals), &info, pump, &pic::visuals, 0);
			if (pic::skins == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, skins, sizeof(skins), &info, pump, &pic::skins, 0);
			if (pic::settings == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, settings, sizeof(settings), &info, pump, &pic::settings, 0);

			if (pic::combo_widget == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, combo_widget, sizeof(combo_widget), &info, pump, &pic::combo_widget, 0);
			if (pic::input_widget == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, input_widget, sizeof(input_widget), &info, pump, &pic::input_widget, 0);

			if (pic::menu_settings_icon == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, menu_settings_icon, sizeof(menu_settings_icon), &info, pump, &pic::menu_settings_icon, 0);

			if (pic::circle_success == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, circle_success, sizeof(circle_success), &info, pump, &pic::circle_success, 0);
			if (pic::circle_error == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, circle_error, sizeof(circle_error), &info, pump, &pic::circle_error, 0);



			ImGui_ImplWin32_Init(window_handle);

			ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

			d3d_device->Release();

			return true;
		}

		auto hijack() -> bool {

			while (!window_handle) {
				window_handle = HiJackNotepadWindow();
				Sleep(10);
			}
			MARGINS margin = { -1 };
			DwmExtendFrameIntoClientArea(window_handle, &margin);
			SetMenu(window_handle, NULL);
			SetWindowLongPtr(window_handle, GWL_STYLE, WS_VISIBLE);
			SetWindowLongPtr(window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

			ShowWindow(window_handle, SW_SHOW);
			UpdateWindow(window_handle);

			SetWindowLong(window_handle, GWL_EXSTYLE, GetWindowLong(window_handle, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);

			return true;
		}
		void AlignForWidth(float width, float alignment = 0.5f)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			float avail = ImGui::GetContentRegionAvail().x;
			float off = (avail - width) * alignment;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		}
		bool info_bar = true;
		const char* cheat_name = "Vokes Public";
		const char* game_status = "Fortnite";
		const char* developer = "Vokes";

		const char* ping = "Powered By";
		const char* world_time = "Vokes";

		DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

		void RenderMenu() {



			static bool previous_insert_state = false;

			bool current_insert_state = GetAsyncKeyState(VK_INSERT) & 0x8000;

			if (current_insert_state && !previous_insert_state) {
				globals::g_render_menu = !globals::g_render_menu;
			}

			previous_insert_state = current_insert_state;

			if (globals::g_render_menu) {
				ImGuiStyle* style = &ImGui::GetStyle();
				style->WindowTitleAlign = { 0.5f, 0.5f };
				style->Colors[ImGuiCol_TitleBg] = custom.get_accent_color();
				style->Colors[ImGuiCol_TitleBgActive] = custom.get_accent_color();
				style->Colors[ImGuiCol_TitleBgCollapsed] = custom.get_accent_color();
				style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 200);

				// aimbot window
				ImGui::SetNextWindowSize({ 250.0f, 0 });
				ImGui::SetNextWindowBgAlpha(0.95f);
				ImGui::Begin("Aimbot", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize); {
					ImGui::Checkbox("Enable", &globals::g_aimbot);
					HotkeyButton(aimkey, ChangeKey, keystatus);
					ImGui::Checkbox(verucryptt("Aimbot Prediction"), &globals::prediction);
					ImGui::Checkbox(verucryptt("Visible Check"), &globals::g_visible_check);
					ImGui::Checkbox("Draw FOV", &globals::g_render_fov);
					ImGui::Checkbox("Draw Crosshair", &globals::crosshair);
					ImGui::SetCursorPosX(10);
					ImGui::SliderFloat(verucryptt("Aimbot FOV"), &globals::g_aimfov, 10, 500, "%.0f");
					ImGui::Text("Aimbot FOV");
					ImGui::SetCursorPosX(10);
					ImGui::SliderFloat(verucryptt("Smooth"), &globals::g_smooth, 1, MaxSmooth, "%.0f");
					ImGui::Text("Smooth");
				} ImGui::End();

				// esp window
				ImGui::SetNextWindowSize({ 250.0f, 0 });
				ImGui::SetNextWindowBgAlpha(0.95f);
				ImGui::Begin("ESP", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize); {
					ImGui::Checkbox("Boxes", &globals::g_box_esp);
					ImGui::Checkbox("Skeleton", &globals::g_skeleton);
					if (globals::g_skeleton) {
						ImGui::Checkbox("Head", &globals::headesp);
					}
					ImGui::Checkbox("Distance", &globals::g_distance);
					ImGui::Checkbox("Snapline", &globals::g_line);
					//ImGui::Checkbox("Platform", &globals::g_platform);
					ImGui::Checkbox("Username", &globals::g_username);
					ImGui::Checkbox("Ranked", &globals::g_ranked);
					ImGui::Checkbox("Outline", &globals::g_outline);
					ImGui::Combo("Box Type", &globals::g_box_type, ("2D\0\Corner\0\Filled"));
					ImGui::SliderFloat("ESP Distance", &globals::g_render_distance, 0.f, 275.f, "%.2fF");
					ImGui::Text("ESP Distance");
					ImGui::SliderFloat("Box Thickness", &globals::g_box_thick, 0.f, 15.f, "%.2fF");
					ImGui::Text("Box Thickness");
					ImGui::SliderInt(verucryptt("Skeleton Thickness"), (int*)&globals::g_skeletonthickness, 1.0f, 10.0f);
					ImGui::Text("Skeleton Thickness");
					ImGui::SliderInt(verucryptt("Snapline Thickness"), (int*)&globals::g_line_thick, 1.0f, 10.0f);
					ImGui::Text("Snapline Thickness");
				} ImGui::End();

				// radar window
				ImGui::SetNextWindowSize({ 250.0f, 0 });
				ImGui::SetNextWindowBgAlpha(0.95f);
				ImGui::Begin("Radar", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize); {
				  ImGui::Checkbox("Show Radar", &globals::radaresp);
				  ImGui::Checkbox("Local Player", &globals::show_localplayer);
			      ImGui::Checkbox("Draw Lines", &globals::show_lines);
				  ImGui::SetCursorPosX(10);  
				  ImGui::SliderFloat("Pos X", &radar_position_x, 10.f, 2000.f);
				  ImGui::Text("Pos X");
                  ImGui::SetCursorPosX(10);  
				  ImGui::SliderFloat("Pos Y", &radar_position_y, 10.f, 2000.f);
				  ImGui::Text("Pos Y");
                  ImGui::SetCursorPosX(10);  
				  ImGui::SliderFloat("Distance", &RadarDistance, 10.f, 700.f);
				  ImGui::Text("Distance");
                  ImGui::SetCursorPosX(10);  
				  ImGui::SliderFloat("Size", &radar_size, 100.f, 500.f);
				  ImGui::Text("Size");
				} ImGui::End();

				// misc window
				ImGui::SetNextWindowSize({ 250.0f, 0 });
				ImGui::SetNextWindowBgAlpha(0.95f);
				ImGui::Begin("Misc", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize); {
					ImGui::Checkbox("Vsync", &globals::vsync);
					ImGui::Checkbox("Draw FPS", &globals::g_fps);
				} ImGui::End();
			}
		}
	
		


		auto menu() -> void {
		

			
			struct tab
			{
				const char* tab_name;
				int tab_id;
			};

			static int tabs = 0;
			static int settingtab = 0;
			static int current_tab = 0;

			

		
		}

		auto draw() -> void {
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);

			ImGui::StyleColorsDark();
			SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			g_main->actor_loop();
			//g_loop::LevelDrawing();

			RenderMenu();
			//menu();
			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
			d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			if (globals::vsync)
			{
				d3d_swap_chain->Present(1, 0);
			}
			else
			{
				d3d_swap_chain->Present(0, 0);
			}
		}

		auto render() -> bool {
			static IDXGISwapChain* pSwapChain;
			IDXGISwapChain* pSwaChain = nullptr;
			static RECT rect_og;
			MSG msg = { NULL };
			ZeroMemory(&msg, sizeof(MSG));

			while (msg.message != WM_QUIT)
			{
				UpdateWindow(window_handle);
				ShowWindow(window_handle, SW_SHOW);

				if (PeekMessageA(&msg, window_handle, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				ImGuiIO& io = ImGui::GetIO();
				io.ImeWindowHandle = window_handle;
				io.DeltaTime = 1.0f / 60.0f;

				POINT p_cursor;
				GetCursorPos(&p_cursor);
				io.MousePos.x = p_cursor.x;
				io.MousePos.y = p_cursor.y;

				if (GetAsyncKeyState(VK_LBUTTON)) {
					io.MouseDown[0] = true;
					io.MouseClicked[0] = true;
					io.MouseClickedPos[0].x = io.MousePos.x;
					io.MouseClickedPos[0].x = io.MousePos.y;
				}
				else
					io.MouseDown[0] = false;

				draw();
			}
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			DestroyWindow(window_handle);

			return true;
		}

		void Overlay()
		{
		    hijack();

			imgui();

			render();
		};
	};
} static n_render::c_render* g_render = new n_render::c_render();