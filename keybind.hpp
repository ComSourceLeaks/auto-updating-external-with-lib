#include <WinUser.h>
#include "ImGui/imgui.h"
#include <string>
#include <processthreadsapi.h>
#include "includes.hpp"
#include "crypter.h"
inline static int keystatus = 0;

inline static int realkey = 0;
inline static int realkey1 = 0;

inline int aimkey = 0;

inline int triggerkey = 0;

inline static int keystatus4 = 0;
inline bool GetKey(int key)
{
	realkey = key;
	return true;
}
inline void ChangeKey(void* blank)
{
	keystatus = 1;
	while (true)
	{
		for (int i = 0; i < 0x87; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				aimkey = i;
				keystatus = 0;
				return;
			}
		}
	}
}

inline bool GetKey3(int key)
{
	realkey1 = key;
	return true;
}
inline void ChangeKey7(void* blank)
{
	keystatus4 = 1;
	while (true)
	{
		for (int i = 0; i < 0x87; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				triggerkey = i;
				keystatus4 = 0;
				return;
			}
		}
	}
}

static const char* keyNames[] =
{
	
	"Keybind",
	"Left Mouse",
	"Right Mouse",
	"Cancel",
	"Middle Mouse",
	"Mouse 5",
	"Mouse 4",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
};

inline static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}
inline void HotkeyButton(int aimkey, void* changekey, int status)
{
	const char* preview_value = NULL;
	if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(keyNames))
		Items_ArrayGetter(keyNames, aimkey, &preview_value);

	std::string aimkeys;
	if (preview_value == NULL)
		aimkeys = verucryptt("Select Key");
	else
		aimkeys = preview_value;

	if (status == 1)
	{
		aimkeys = verucryptt("Press the Key");
	}

	// Define the size of the button
	ImVec2 button_size(ImGui::GetContentRegionAvail().x, 25); // Adjust width and height as needed

	// Create the button with the specified size
	if (ImGui::Button(aimkeys.c_str(), button_size))
	{
		if (status == 0)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
		}
	}

	// Adjust the button's text alignment if necessary
	// Center text horizontally and vertically
	ImVec2 text_pos = ImVec2(button_size.x * 0.5f, button_size.y * 0.5f);
	ImVec2 text_size = ImGui::CalcTextSize(aimkeys.c_str());

	ImGui::GetWindowDrawList()->AddText(
		ImVec2(text_pos.x - text_size.x * 0.5f, text_pos.y - text_size.y * 0.5f),
		ImGui::GetColorU32(ImGuiCol_Text),
		aimkeys.c_str()
	);
}