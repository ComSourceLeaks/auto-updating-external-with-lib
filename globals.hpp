#include <mutex>
#include <memory>
namespace globals
{
	inline bool show_ui = true;

	inline bool vsync = 0;
	
	inline bool performancemode = 0;
	//g_legitaimbot

	inline bool g_aimbot = 1;

	inline bool g_triggerbot = 0;
	int triggerbot_key;

	bool has_clicked;
	inline bool g_details = 0;

	int maximum_distance = 20;

	std::chrono::steady_clock::time_point tb_begin;

	std::chrono::steady_clock::time_point tb_end;

	int tb_time_since;

	int custom_delay = 1;

	inline float g_font_sizez = 15;

	HWND hwnd = 0;

	inline float g_font_size = 15;

	inline int g_username_type = 0;

	inline int g_watermark_type = 0;
	inline bool NoReload = 0;
	inline int RadarType = 0;

	inline bool particles = 1;

	inline bool g_target_text = 0;

	inline bool playerfly = 0;
	inline bool playerfly2 = 0;
	inline bool speedhack = 0;
	inline bool freecam = 0;

	inline bool g_visible_check = 0;

	inline bool g_enemiesarround = 1;

	inline bool g_watermark = 1;

	inline bool customname = 0;

	inline bool g_line = 0;
	inline bool kills = 0;
	inline bool level = 0;



	inline int g_line_type = 0;

	inline float g_aimfov = 75;

	inline int g_hitbox = 0;

	inline bool nigger = 0;

	inline bool prediction = 0;

	inline bool predictiondot = 0;
	inline float predictiondistance = 250;

	inline bool g_skeleton = 0;

	inline bool PlayerInfo = 0;

	inline bool nearbyplayers = 0;

	inline bool g_box_esp = 1;

	inline bool headesp = 0;

	inline bool g_enable_esp = 1;

	inline bool g_fps = 0;

	inline bool magicbullet = 0;

	float testslider = 12.0f;

	inline bool g_distance = 1;
	inline float entitydistance = 100;

	inline bool crosshair = 0;
	float crosshair_size = 2.0f; // Size of the main crosshair lines
	float crosshair_length = 9.0f; // Length of the main crosshair lines
	float crosshair_thickness = 1.0f; // Thickness of the main crosshair lines
	ImVec4 crosshair_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Color of the crosshair lines

	inline bool menucrosshair = 0;

	inline bool keybindlist = 1;

	inline bool g_username = 1;

	inline bool radaresp = 0;

	inline bool weaponesp = 0;

	inline bool g_platform = 0;

	inline int g_distance_type = 1;

	inline bool g_snapline = 3;

	static float FontColor[3] = { 1.0f, 1.0f, 1.0f }; // Blanc
	// Couleur blanche si visible
	static float g_color_Visible[3] = { 1.0f, 1.0f, 1.0f }; // Blanc

	static float g_color_skeleton[3] = { 1.0f, 1.0f, 1.0f }; // Blanc
	// Couleur blanche pour le texte

	// Couleur blanche si invisible (au lieu de vert)
	static float g_color_invisible[3] = { 1.0f, 1.0f, 1.0f }; // Blanc

	static float g_color[3] = { 1.0f, 1.0f, 1.0f }; // Blanc
	inline float FovColor[3] = { 1.0f, 1.0f, 1.0f }; // Blanc



	float g_render_distance = 275;

	inline int g_box_type = 2;

	inline bool g_outline = 0;
	inline bool g_ranked = 0;


	inline float g_box_thick = 2;
	//g_ranked
	inline int g_line_thick = 2;

	inline int g_skeletonthickness = 2.7;

	inline float g_aim_max_dist = 275;

	inline float g_smooth = 12;

	inline bool g_render_fov = 1;
	inline bool g_star_fov = 0;

	//star_fov

	//Loot esp
	inline bool draw_chests = 0;

	inline bool draw_cooler = 0;

	inline int cooler_render = 50;

	inline bool draw_ammo = 0;

	inline int g_weapon_type = 0;

	inline bool g_weapon_rarity = 0;

	inline int ammo_render = 50;

	inline int lammarender = 275;

	inline int vehiclesrender = 50;

	inline bool lamma = 0;

	inline bool pickups = 0;

	inline bool draw_vehicles = 0;

	inline int chestrender = 50;

	inline int pickupsrender = 50;

	inline bool Common = 1;

	inline bool rare = 1;

	inline bool purple = 1;

	inline bool gold = 1;

	inline bool mythic = 1;

	inline bool UnCommon = 1;

	inline bool g_render_menu = 1;

	inline bool settingswindow = 0;

	inline bool in_lobby;

	inline bool show_localplayer = 1;
	inline bool show_lines = 1;
	inline float radar_size = 250.0f;
}


namespace developer
{
	namespace menutesting
	{
		inline bool paint = false;
		inline bool notepad = false;
	}
}
