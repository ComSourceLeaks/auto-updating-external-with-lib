#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <future>
#include <iostream>
#include <mutex>

#include "offsets.hpp"
#include "functions.hpp"
#include "aimbot.cpp"
using namespace uee;
bool debugger = false;
#define debug_cache false
std::mutex entity_list_mutex;

namespace g1 {
	class g2 {
	public:
		static auto actors() -> void {
			temporary_entity_list.clear();

			arrays->UWorld = read<__int64>(virtualaddy + offset::uworld);
			arrays->game_instance = read<uintptr_t>(arrays->UWorld + offset::game_instance);
			arrays->local_player = read<uintptr_t>(read<uintptr_t>(arrays->game_instance + offset::local_player));
			arrays->player_controller = read<uintptr_t>(arrays->local_player + offset::player_controller);
			arrays->acknowledged_pawn = read<uintptr_t>(arrays->player_controller + offset::acknowledged_pawn);
			arrays->skeletal_mesh = read<uintptr_t>(arrays->acknowledged_pawn + offset::mesh);
			arrays->player_state = read<uintptr_t>(arrays->acknowledged_pawn + offset::PlayerState);
			arrays->root_component = read<uintptr_t>(arrays->acknowledged_pawn + offset::root_component);
			arrays->relative_location = read<fvector>(arrays->root_component + offset::relative_location);
			arrays->game_state = read<uintptr_t>(arrays->UWorld + offset::game_state);
			arrays->player_array = read<uintptr_t>(arrays->game_state + offset::player_array);
			arrays->player_array_size = read<int>(arrays->game_state + (offset::player_array + sizeof(uintptr_t)));
			arrays->current_vehicle = read<uintptr_t>(arrays->acknowledged_pawn + offset::CurrentVehicle);

			for (int i = 0; i < arrays->player_array_size; ++i) {
				auto player_state = read<uintptr_t>(arrays->player_array + (i * sizeof(uintptr_t)));
				auto current_actor = read<uintptr_t>(player_state + offset::PawnPriv);

				if (current_actor == arrays->acknowledged_pawn) continue;

				auto skeletal_mesh = read<uintptr_t>(current_actor + offset::mesh);
				if (!skeletal_mesh) continue;

				auto base_bone = ue5->Bone(skeletal_mesh, bone::Root);
				if (base_bone.x == 0 && base_bone.y == 0 && base_bone.z == 0) continue;

				if (!ue5->in_screen(ue5->w2s(ue5->Bone(skeletal_mesh, bone::Pelvis)))) continue;

				auto is_despawning = (read<char>(current_actor + 0x758) >> 3);
				if (is_despawning) continue;

				auto root_component = read<uintptr_t>(current_actor + offset::root_component);
				auto relative_location1 = read<fvector>(root_component + offset::relative_location);

				auto team_index = read<int>( player_state + offset::team_index );

				entity cached_actor{};
				cached_actor.entity = current_actor;
				cached_actor.skeletal_mesh = skeletal_mesh;
				cached_actor.root_component = root_component;
				cached_actor.relative_location = relative_location1;
				cached_actor.player_state = player_state;
				cached_actor.team_index = team_index;

				temporary_entity_list.push_back(cached_actor);
			}

			entity_list_mutex.lock();
			entity_list = temporary_entity_list;
			entity_list_mutex.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(550));
		}
	};
}

static g1::g2* g_cache = new g1::g2();