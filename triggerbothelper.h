#include "imports.h"
#include "hider.h"
#include <minwindef.h>
#include "libloaderapi.h"

enum TYPES
{
	move = 1,
	left_up = 4,
	left_down = 2,
	right_up = 8,
	right_down = 16
};

struct MOUSE_INFO
{
	int move_direction_x;
	int move_direction_y;
	unsigned int mouse_data;
	TYPES mouse_options;
	unsigned int time_offset_in_miliseconds;
	void* extra_info;
};

bool(*MOVE)(MOUSE_INFO*, int) = nullptr;

enum INFO : int {
	FAIL = 0,
	SUCCESS = 1
};

class c_hooking {
public:

	auto WINAPI init() -> BOOLEAN {
		auto win32u = IMPORT(LoadLibraryA).safe_cached()(HIDE("win32u.dll"));
		void* ADDRESS = (void*)IMPORT(GetProcAddress).safe_cached()(win32u, HIDE("NtUserInjectMouseInput"));
		if (!ADDRESS) return INFO::FAIL;
		*(void**)&MOVE = ADDRESS;
		IMPORT(FreeLibrary).get()(win32u);
		return INFO::SUCCESS;
	}
};
static c_hooking* g_hooking = new c_hooking();

auto WINAPI Move(int X, int Y, unsigned int time = 0) -> BOOLEAN {
	MOUSE_INFO info = {};
	info.mouse_data = 0;
	info.mouse_options = TYPES::move;
	info.move_direction_x = X;
	info.move_direction_y = Y;
	info.time_offset_in_miliseconds = time;
	return MOVE(&info, 1);
}

bool Left(int x = 0, int y = 0)
{
	MOUSE_INFO info = {};
	info.mouse_data = 0;
	info.mouse_options = TYPES::left_down;
	info.move_direction_x = x;
	info.move_direction_y = y;
	info.time_offset_in_miliseconds = 0;
	return MOVE(&info, 1);
}

bool Up(int x = 0, int y = 0)
{
	MOUSE_INFO info = {};
	info.mouse_data = 0;
	info.mouse_options = TYPES::left_up;
	info.move_direction_x = x;
	info.move_direction_y = y;
	info.time_offset_in_miliseconds = 0;
	return MOVE(&info, 1);
}