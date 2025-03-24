#include "cache_helper.hpp"
#include <iomanip>
#include "triggerbothelper.h"
#include "xorst.h"
#include "crypter.h"
#include "ImGui/imgui.h"
#include "crypt23.h"

struct WeaponInformation
{
	int32_t ammo_count;
	int32_t max_ammo;

	BYTE tier;
	std::string weapon_name;
	std::string buildplan;
};
std::string LocalPlayerWeapon;
WeaponInformation WeaponInfo;
int InFovEnemy = 0;
int visennemy = 0;

__int64 TargetedBuild;
bool bTargetedBuild;

bool ShowRadar = true;
bool rect_radar = true;
float radar_position_x{ 50.0f };
float radar_position_y{ 104.121 };
float radar_size{ 163.513 };
float RadarDistance = { 200.f };
float downed[1];
std::wstring string_to_wstring(const std::string& str) {
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

std::string wstring_to_utf8(const std::wstring& wstr) {
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
	return str;
}
std::string get_rank(int32_t Tier)
{
	if (Tier == 0)
		return std::string("Bronze 1");
	else if (Tier == 1)
		return std::string("Bronze 2");
	else if (Tier == 2)
		return std::string("Bronze 3");
	else if (Tier == 3)
		return std::string("Silver 1");
	else if (Tier == 4)
		return std::string("Silver 2");
	else if (Tier == 5)
		return std::string("Silver 3");
	else if (Tier == 6)
		return std::string("Gold 1");
	else if (Tier == 7)
		return std::string("Gold 2");
	else if (Tier == 8)
		return std::string("Gold 3");
	else if (Tier == 9)
		return std::string("Platinum 1");
	else if (Tier == 10)
		return std::string("Platinum 2");
	else if (Tier == 11)
		return std::string("Platinum 3");
	else if (Tier == 12)
		return std::string("Diamond 1");
	else if (Tier == 13)
		return std::string("Diamond 2");
	else if (Tier == 14)
		return std::string("Diamond 3");
	else if (Tier == 15)
		return std::string("Elite");
	else if (Tier == 16)
		return std::string("Champion");
	else if (Tier == 17)
		return std::string("Unreal");
	else
		return std::string("Unranked");
}
ImVec4 get_rank_color(int32_t Tier)
{
	if (Tier == 0 || Tier == 1 || Tier == 2)
		return ImVec4(0.902f, 0.580f, 0.227f, 1.0f);
	else if (Tier == 3 || Tier == 4 || Tier == 5)
		return ImVec4(0.843f, 0.843f, 0.843f, 1.0f);
	else if (Tier == 6 || Tier == 7 || Tier == 8)
		return ImVec4(1.0f, 0.871f, 0.0f, 1.0f);
	else if (Tier == 9 || Tier == 10 || Tier == 11)
		return ImVec4(0.0f, 0.7f, 0.7f, 1.0f);
	else if (Tier == 12 || Tier == 13 || Tier == 14)
		return ImVec4(0.1686f, 0.3294f, 0.8235f, 1.0f);
	else if (Tier == 15)
		return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	else if (Tier == 16)
		return ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
	else if (Tier == 17)
		return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);
	else
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

auto getranked(uintptr_t PlayerState) -> std::string
{
	if (driver::valid_check(PlayerState))
	{
		uintptr_t HabaneroComponent = read<uintptr_t>(PlayerState + 0xab0);
		if (driver::valid_check(HabaneroComponent))

		{
			uintptr_t rankedprogress = read<uintptr_t>(HabaneroComponent + 0xb8);
		}
		wchar_t ranked[64] = { 0 };
		std::wstring ranked_wstr(ranked);
		std::string ranked_str(ranked_wstr.begin(), ranked_wstr.end());
		return ranked_str;
	}
}

namespace Offset
{
	namespace AFortWeapon
	{
		auto bIsReloadingWeapon = 0x399;
		auto Ammo = 0xe54;
	}

	namespace UFortMovementComp_Character
	{
		auto bWantsToSwing = 0x14bc;
		auto SwingAttachLocation = 0x14c0;
		auto bSwingInstantVelocity = 0x14bf;
		auto SwingLaunch = 0x14d8;

		auto bWantsToSkate = 0x5511;
		auto acceleration = 0x58;
	}
}

void CalcRadarPoint(fvector vOrigin, int& screenx, int& screeny)
{
	ue5->get_camera();
	fvector vAngle = camera_postion.rotation;
	auto fYaw = vAngle.y * M_PI / 180.0f;
	float dx = vOrigin.x - camera_postion.location.x;
	float dy = vOrigin.y - camera_postion.location.y;

	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	x = -x;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = (float)RadarDistance * 1000.f;

	ue5->RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImVec2(radar_position_x, radar_position_y);
	ImVec2 DrawSize = ImVec2(globals::radar_size, globals::radar_size);

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}
void fortnite_radar(float x, float y, float size, bool rect = false)
{
	if (ShowRadar)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Once);
		static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::Begin(("##radar"), nullptr, flags);

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImU32 transparentBlack = ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.5f });
		drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size), transparentBlack, 0.0f, 0);

		ImVec2 center(x + size / 2, y + size / 2);

		ImU32 gray = ImGui::GetColorU32({ 0.5f, 0.5f, 0.5f, 1.0f });

		ImU32 white = ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f });
		if (globals::show_localplayer) {
			drawList->AddCircleFilled(center, 4.0f, white, 100);
		}

		if (globals::show_lines) {
			float line_length = size / 2;

			drawList->AddLine(center, ImVec2(x, y), white, 1.0f);
			drawList->AddLine(center, ImVec2(x + size, y), white, 1.0f);

			drawList->AddLine(ImVec2(x, center.y), ImVec2(x + size, center.y), gray, 2.5f);

			drawList->AddLine(center, ImVec2(center.x, center.y + line_length), gray, 2.5f);
		}

		ImGui::End();
	}
}

void add_players_radar(fvector WorldLocation)
{
	if (ShowRadar)
	{
		static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::Begin(("##radar"), nullptr, flags);

		int ScreenX, ScreenY = 0;
		const auto& GetWindowDrawList = ImGui::GetWindowDrawList();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		CalcRadarPoint(WorldLocation, ScreenX, ScreenY);
		if (globals::g_visible_check) {
			drawList->AddCircleFilled(ImVec2(ScreenX, ScreenY), 5, ImGui::GetColorU32({ globals::g_color_Visible[0],globals::g_color_Visible[1],globals::g_color_Visible[2],  1.0f }), 7);
		}
		else {
			drawList->AddCircleFilled(ImVec2(ScreenX, ScreenY), 5, ImGui::GetColorU32({ globals::g_color_invisible[0],globals::g_color_invisible[1],globals::g_color_invisible[2],  1.0f }), 7);
		}

		ImGui::End();
	}
}

namespace g_loop {

	class g_fn {
	public:

		auto actor_loop() -> void {
			ImGui::PushFont(FortniteFont);

			ue5->get_camera();

			ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
			float target_dist = FLT_MAX;
			uintptr_t target_entity = 0;

			if (globals::g_watermark) {
				switch (globals::g_watermark_type)
				{
				case 0:
				{
					char aimfovText[64];
					sprintf_s(aimfovText, sizeof(aimfovText), "", globals::g_aimfov);
					DrawString(globals::g_font_size, 10, 30, ImColor(255, 255, 255, 255), false, true, aimfovText);
					break;
				}

				case 1:
				{

					char fps[64];
					sprintf_s(fps, sizeof(fps), "FPS-> %.1f", ImGui::GetIO().Framerate);
					DrawString(globals::g_font_size, 110, 70, ImColor(255, 255, 255, 255), false, true, fps);
					break;
				}

				case 2:
				{
					ImVec2 rectMin3 = ImVec2(10, 10);
					ImVec2 rectMax3 = ImVec2(120, 30);

					draw_list->AddRectFilled(rectMin3, rectMax3, ImColor(255, 192, 203, 255), 5.0f);
					draw_list->AddRect(rectMin3, rectMax3, ImColor(255, 192, 203, 255), 5.0f);

					ImVec2 textSize3 = ImGui::CalcTextSize("Mikz Private");

					ImVec2 textPos3 = ImVec2(rectMin3.x + (rectMax3.x - rectMin3.x - textSize3.x) * 0.5f, rectMin3.y + (rectMax3.y - rectMin3.y - textSize3.y) * 0.5f);

					draw_list->AddText(textPos3, ImColor(255, 255, 255), "Mikz Private");
					break;
				}

				}
			}

			const float centerWidth = screen_width / 2;
			const float centerHeight = screen_height / 2;

			for (auto& cached : entity_list) {
				auto root_bone = ue5->Bone(cached.skeletal_mesh, bone::Root);
				root = ue5->w2s(root_bone);
				root_box = ue5->w2s(fvector(root_bone.x, root_bone.y, root_bone.z + 30));
				root_box1 = ue5->w2s(fvector(root_bone.x, root_bone.y, root_bone.z - 15));

				auto head_bone = ue5->Bone(cached.skeletal_mesh, bone::Head);
				head = ue5->w2s(head_bone);
				head_box = ue5->w2s(fvector(head_bone.x, head_bone.y, head_bone.z + 15));

				float box_height = abs(head.y - root_box1.y);
				float box_width = box_height * 0.25f;
				float distance = arrays->relative_location.distance(root_bone) / 100;

				auto pawn_private1 = read<uintptr_t>(cached.player_state + offset::PawnPriv);
				if (arrays->acknowledged_pawn && !arrays->current_vehicle)
					if (distance > globals::g_render_distance && arrays->acknowledged_pawn) continue;

				if (globals::g_aimbot) {
					auto dx = head.x - (screen_width / 2);
					auto dy = head.y - (screen_height / 2);
					auto dist = sqrtf(dx * dx + dy * dy);

					if (globals::g_visible_check) {
						if (ue5->is_visible(cached.skeletal_mesh)) {
							if (dist < globals::g_aimfov && dist < target_dist) {
								target_dist = dist;
								target_entity = cached.entity;
							}
						}
					}
					else {
						if (dist < globals::g_aimfov && dist < target_dist) {
							target_dist = dist;
							target_entity = cached.entity;
						}
					}
				}
				if (globals::g_star_fov)
				{
					ImColor fov_col;

					float size = globals::g_aimfov;
					ImVec2 center(screen_width / 2, screen_height / 2);

					ImVec2 starPoints[10];
					float angleOff = 144.0f;

					for (int i = 0; i < 5; i++) {
						starPoints[i * 2] = ImVec2(center.x + size * sin(angleOff * i * 3.14159265359 / 180.0f),
							center.y - size * cos(angleOff * i * 3.14159265359 / 180.0f));
						starPoints[i * 2 + 1] = ImVec2(center.x + size / 2 * sin(angleOff * (i * 2 + 1) * 3.14159265359 / 180.0f),
							center.y - size / 2 * cos(angleOff * (i * 2 + 1) * 3.14159265359 / 180.0f));
					}

					for (int i = 0; i < 5; i++) {
						int next = (i + 1) % 5;
						ImGui::GetBackgroundDrawList()->AddLine(starPoints[i * 2], starPoints[next * 2], fov_col, 2);
					}
				}
				int visibleMeshes = 0;
				int nonVisibleMeshes = 0;
				ImVec2 countPosition = ImVec2(10, 10);
				std::string visibleCountStr = std::string(verucryptt("Visible Entities: ")) + std::to_string(visibleMeshes);
				ImVec2 visibleTextSize = ImGui::CalcTextSize(visibleCountStr.c_str());
				if (arrays->aactor != arrays->acknowledged_pawn)
				{
					visibleMeshes++;
				}
				int playersWithin50m = 0;

				if (globals::g_render_fov) {
					ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(screen_width / 2, screen_height / 2), globals::g_aimfov, ImColor(255, 255, 255), 64, 1);
				}
				if (globals::g_star_fov)
				{
					ImColor fov_col;

					float size = globals::g_aimfov;
					ImVec2 center(screen_width / 2, screen_height / 2);

					ImVec2 starPoints[10];
					float angleOff = 144.0f;

					for (int i = 0; i < 5; i++) {
						starPoints[i * 2] = ImVec2(center.x + size * sin(angleOff * i * 3.14159265359 / 180.0f),
							center.y - size * cos(angleOff * i * 3.14159265359 / 180.0f));
						starPoints[i * 2 + 1] = ImVec2(center.x + size / 2 * sin(angleOff * (i * 2 + 1) * 3.14159265359 / 180.0f),
							center.y - size / 2 * cos(angleOff * (i * 2 + 1) * 3.14159265359 / 180.0f));
					}

					for (int i = 0; i < 5; i++) {
						int next = (i + 1) % 5;
						ImGui::GetBackgroundDrawList()->AddLine(starPoints[i * 2], starPoints[next * 2], fov_col, 2);
					}
				}
				if (globals::nearbyplayers)
				{
					if (distance <= globals::entitydistance)
					{
						playersWithin50m++;
					}

					ImVec2 playerCountPosition = ImVec2(countPosition.x, countPosition.y + visibleTextSize.y + 5);
					std::string nearbyPlayerCountStr = std::string(verucryptt("Nearby Players: ")) + std::to_string(playersWithin50m);

					ImVec2 distancePosition = ImVec2(playerCountPosition.x, playerCountPosition.y + visibleTextSize.y + 5);

					ImVec2 textSize = ImGui::CalcTextSize(nearbyPlayerCountStr.c_str());

					ImVec2 textPosition(screen_width / 2.0f - textSize.x / 2.0f, 10.0f);

					ImGui::GetBackgroundDrawList()->AddText(textPosition, ImColor(0, 255, 0, 255), nearbyPlayerCountStr.c_str());

				}

				if (globals::crosshair)
				{
					ImVec2 center(screen_width / 2, screen_height / 2);

					ImU32 crosshair_color_u32 = ImGui::ColorConvertFloat4ToU32(globals::crosshair_color);

					draw_list->AddLine(ImVec2(center.x - globals::crosshair_size, center.y), ImVec2(center.x - globals::crosshair_length, center.y), crosshair_color_u32, globals::crosshair_thickness);
					draw_list->AddLine(ImVec2(center.x + globals::crosshair_size, center.y), ImVec2(center.x + globals::crosshair_length, center.y), crosshair_color_u32, globals::crosshair_thickness);
					draw_list->AddLine(ImVec2(center.x, center.y - globals::crosshair_size), ImVec2(center.x, center.y - globals::crosshair_length), crosshair_color_u32, globals::crosshair_thickness);
					draw_list->AddLine(ImVec2(center.x, center.y + globals::crosshair_size), ImVec2(center.x, center.y + globals::crosshair_length), crosshair_color_u32, globals::crosshair_thickness);

					draw_list->AddLine(ImVec2(center.x - globals::crosshair_size - 1, center.y), ImVec2(center.x - globals::crosshair_length + 1, center.y), crosshair_color_u32, globals::crosshair_thickness - 1);
					draw_list->AddLine(ImVec2(center.x + globals::crosshair_size + 1, center.y), ImVec2(center.x + globals::crosshair_length - 1, center.y), crosshair_color_u32, globals::crosshair_thickness - 1);
					draw_list->AddLine(ImVec2(center.x, center.y - globals::crosshair_size - 1), ImVec2(center.x, center.y - globals::crosshair_length + 1), crosshair_color_u32, globals::crosshair_thickness - 1);
					draw_list->AddLine(ImVec2(center.x, center.y + globals::crosshair_size + 1), ImVec2(center.x, center.y + globals::crosshair_length - 1), crosshair_color_u32, globals::crosshair_thickness - 1);
				}

				ImColor vis;
				ImColor invis;
				ImColor normal;

				vis = ImVec4(globals::g_color_Visible[0], globals::g_color_Visible[1], globals::g_color_Visible[2], 1.0f);
				invis = ImVec4(globals::g_color_invisible[0], globals::g_color_invisible[1], globals::g_color_invisible[2], 1.0f);
				normal = ImVec4(globals::g_color[0], globals::g_color[1], globals::g_color[2], 1.0f);

				ImColor visibleColor;
				if (globals::g_visible_check && globals::g_enable_esp)
				{
					visibleColor = ue5->is_visible(cached.skeletal_mesh) ? ImColor(vis) : ImColor(invis);
				}
				else
				{
					visibleColor = ImColor(normal);
				}

				if (globals::g_line && globals::g_enable_esp)
				{
					switch (globals::g_line_type)
					{
					case 0:
						draw_list->AddLine(ImVec2(screen_width / 2, screen_height), ImVec2(root.x, root.y), visibleColor, globals::g_line_thick);
						break;
					}
				}

				if (globals::g_box_esp && globals::g_enable_esp) {
					switch (globals::g_box_type)
					{
					case 0:
						if (globals::g_outline)
						{
							draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), head_box.y), ImVec2(root.x + (box_width / 2), head_box.y), ImColor(0, 0, 0), globals::g_box_thick + 2);
							draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), head_box.y), ImVec2(head_box.x - (box_width / 2), root.y), ImColor(0, 0, 0), globals::g_box_thick + 2);
							draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), root.y), ImVec2(root.x + (box_width / 2), root.y), ImColor(0, 0, 0), globals::g_box_thick + 2);
							draw_list->AddLine(ImVec2(root.x + (box_width / 2), head_box.y), ImVec2(root.x + (box_width / 2), root.y), ImColor(0, 0, 0), globals::g_box_thick + 2);
						}

						draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), head_box.y), ImVec2(root.x + (box_width / 2), head_box.y), visibleColor, globals::g_box_thick);
						draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), head_box.y), ImVec2(head_box.x - (box_width / 2), root.y), visibleColor, globals::g_box_thick);
						draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), root.y), ImVec2(root.x + (box_width / 2), root.y), visibleColor, globals::g_box_thick);
						draw_list->AddLine(ImVec2(root.x + (box_width / 2), head_box.y), ImVec2(root.x + (box_width / 2), root.y), visibleColor, globals::g_box_thick);
						break;

					case 1:
						if (globals::g_outline)
						{
							DrawCorneredBox(root.x - (box_width / 2), head_box.y, box_width, box_height, ImColor(0, 0, 0), globals::g_box_thick + 2.0, 0, 0);
						}

						DrawCorneredBox(root.x - (box_width / 2), head_box.y, box_width, box_height, visibleColor, globals::g_box_thick, 0, 0);
						break;

					case 2:
					{
						if (globals::g_outline)
						{
							ImVec4 filledColorTransparent = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
							draw_list->AddRectFilled(ImVec2(head_box.x - (box_width / 2), head_box.y),
								ImVec2(root.x + (box_width / 2), root.y),
								IM_COL32(filledColorTransparent.x * 255, filledColorTransparent.y * 255, filledColorTransparent.z * 255, filledColorTransparent.w * 255));
						}

						draw_list->AddRectFilled(ImVec2(head_box.x - (box_width / 2), head_box.y),
							ImVec2(root.x + (box_width / 2), root.y),
							IM_COL32(0, 0, 0, 128));

					}
					}
				}

				if (globals::g_username && globals::g_enable_esp) {
					switch (globals::g_username_type)
					{
					case 0:
					{
						std::string username_str = ue5->get_player_name(cached.player_state);
						ImVec2 text_size = ImGui::CalcTextSize(username_str.c_str());
						DrawString(globals::g_font_size, head_box.x - (text_size.x / 2), head_box.y - 20, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), false, true, username_str.c_str());
						break;
					}

					case 1:
					{
						std::string username_str = ue5->get_player_name(cached.player_state);
						ImVec2 text_size = ImGui::CalcTextSize(username_str.c_str());
						DrawString(globals::g_font_size, root.x - (text_size.x / 2), root.y + 20, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), false, true, username_str.c_str());
						break;
					}
					}
				}

				if (globals::g_platform && globals::g_enable_esp) {
					DWORD_PTR test_platform = read<DWORD_PTR>((uintptr_t)cached.player_state + 0x438);

					wchar_t platform[64];

					driver::read_physical((PVOID)test_platform, (uint8_t*)platform, sizeof(platform));

					std::wstring platform_wstr(platform);

					std::string platform_str(platform_wstr.begin(), platform_wstr.end());

					ImVec2 textPosition(head_box.x, head_box.y - 35);

					draw_outlined_text(textPosition, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), platform_str.c_str());
				}

				if (globals::g_ranked)
				{
					ImVec2 text_size;
					int32_t rankProgress = read<int32_t>(
						read<uintptr_t>(
							(uintptr_t)cached.player_state + 0x9E8) + 0xb8 + 0x10
					);
					std::string ranktype = get_rank(rankProgress);
					std::wstring w_ranktype = string_to_wstring(ranktype);
					std::string final = wstring_to_utf8(w_ranktype);
					char ranked[64];
					sprintf_s(ranked, E("%s"), (final));
					ImVec4 rankedColor = get_rank_color(rankProgress);

					DrawString(globals::g_font_size, head_box.x - (text_size.x / 2) - 25, head_box.y - 50, rankedColor, false, true, ranked);

				}

				if (globals::radaresp)
				{
					float radarX = radar_position_x;
					float radarY = radar_position_y;
					bool isRect = rect_radar;
					fortnite_radar(radarX, radarY, globals::radar_size, isRect);
					add_players_radar(root_bone);

				}

				if (globals::g_distance && globals::g_enable_esp)
				{
					switch (globals::g_distance_type)
					{
					case 0:
					{
						std::string Distance = "[" + std::to_string((int)distance) + (std::string)"m]";

						ImVec2 text_size = ImGui::CalcTextSize(Distance.c_str());

						DrawString(globals::g_font_size, head_box.x - (text_size.x / 2), head_box.y + 5, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), false, true, Distance.c_str());
						break;
					}

					case 1:
					{
						std::string Distance = "" + std::to_string((int)distance) + (std::string)"m";

						ImVec2 text_size = ImGui::CalcTextSize(Distance.c_str());

						DrawString(globals::g_font_size, root.x - (text_size.x / 2), root.y + 5, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), false, true, Distance.c_str());
						break;
					}
					}
				}

				if (globals::g_skeleton && globals::g_enable_esp) {
					ue5->skeleton(cached.skeletal_mesh);
				}
			}

			if (globals::g_watermark) {

				DrawString(globals::g_font_sizez, 13, 13, ImColor(255, 255, 255), false, true, "Vokes Public -> INSERT To Open/Close Menu");

			}

			if (globals::g_fps) {

				char fps[64];
				sprintf_s(fps, sizeof(fps), "%.0f > FPS", ImGui::GetIO().Framerate);
				DrawString(globals::g_font_size, 10, 35, ImColor(255, 255, 255, 255), false, true, fps);
			}

			float ProjectileSpeed = 60000;
			fvector ComponentVelocity = read<fvector>(arrays->root_component + 0x168);
			auto test = read<uintptr_t>(target_entity + offset::root_component);

			float projectileSpeed = 0;
			float projectileGravityScale = 0;

			if (globals::prediction && GetAsyncKeyState(aimkey)) {

				projectileSpeed = read<float>(arrays->current_weapon + 0x1F30);
				projectileGravityScale = read<float>(arrays->current_weapon + 0x1F34);
				auto closest_mesh2 = read <std::uint64_t>(target_entity + offset::mesh);
				fvector hitbox = ue5->Bone(closest_mesh2, bone::Head);
				auto root = read <uintptr_t>(target_entity + offset::root_component);
				auto distance = arrays->relative_location.distance(hitbox);
				fvector velocity = read <fvector>(root + offset::velocity);
				fvector Predictor = ue5->Prediction(hitbox, velocity, distance, projectileSpeed);
				fvector2d hitbox_screen_predict = ue5->w2s(Predictor);
				if (globals::predictiondot)
					ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(head_box.x, head_box.y), 3.0f, ImColor(255, 105, 180));
				input->move(hitbox_screen_predict);

			}

			if (globals::g_triggerbot) {
				{
					__int64 temp_targted = read<__int64>(arrays->player_controller + 0x1710);
					bool b = GetAsyncKeyState(triggerkey);
					if (TargetedBuild != 0 && TargetedBuild != temp_targted)
					{
						INPUT    Input = { 0 };

						Input.type = INPUT_MOUSE;
						Input.mi.dwFlags = TYPES::left_down;
						::SendInput(1, &Input, sizeof(INPUT));

						::ZeroMemory(&Input, sizeof(INPUT));
						Input.type = INPUT_MOUSE;
						Input.mi.dwFlags = TYPES::left_up;
						::SendInput(1, &Input, sizeof(INPUT));
						TargetedBuild = 0;
					}

					if (TargetedBuild != temp_targted && TargetedBuild != 0)
						Beep(500, 500);
				}
			}

			int PlayerPOS = 0;

			if (target_entity && globals::g_aimbot) {
				auto closest_mesh = read <std::uint64_t>(target_entity + offset::mesh);

				float projectileSpeed = 0;
				float projectileGravityScale = 0;
				fvector hitbox;
				fvector2d hitbox_screen;

				switch (globals::g_hitbox) {
				case 0:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Head));
					hitbox = ue5->Bone(closest_mesh, bone::Head);;
					break;
				case 1:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Neck));
					hitbox = ue5->Bone(closest_mesh, bone::Neck);;
					break;
				case 2:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Chest));
					hitbox = ue5->Bone(closest_mesh, bone::Chest);;
					break;
				case 3:
					std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
					std::uniform_int_distribution<int> distribution(0, 3);
					int randomHitbox = distribution(rng);

					switch (randomHitbox) {
					case 0:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Head));
						hitbox = ue5->Bone(closest_mesh, bone::Head);
						break;
					case 1:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Neck));
						hitbox = ue5->Bone(closest_mesh, bone::Neck);
						break;
					case 2:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Chest));
						hitbox = ue5->Bone(closest_mesh, bone::Chest);
						break;
					case 3:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::RShoulder));
						hitbox = ue5->Bone(closest_mesh, bone::RShoulder);
						break;
					case 4:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::LShoulder));
						hitbox = ue5->Bone(closest_mesh, bone::LShoulder);
						break;
					}
					break;
				}
				std::string LocalPlayerWeapon;

				fvector2d hitbox_screen2 = ue5->w2s(hitbox);

				if (hitbox.x != 0 || hitbox.y != 0 && (get_cross_distance(hitbox.x, hitbox.y, screen_width / 2, screen_height / 2) <= globals::g_aimfov))
				{
					if ((closest_mesh))
					{

						if (globals::g_target_text)
						{
							DrawString(20, hitbox_screen.x - 6, hitbox_screen.y - 80, ImColor(255, 0, 0), true, true, ("TARGET"));
						}

						if (GetAsyncKeyState(aimkey))
							input->move(hitbox_screen);
					}
				}
			}
			else {
				target_dist = FLT_MAX;
				target_entity = NULL;
			}

			ImGui::PopFont();
		}
	};
} static g_loop::g_fn* g_main = new g_loop::g_fn();