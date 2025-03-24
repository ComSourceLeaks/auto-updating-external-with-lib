#include <string.h>
#include <list>
#include <string>
#include "ue.hpp"

namespace camera
{
	fvector location, rotation;
	float fov;
}


namespace uee
{
	class ue
	{
	public:

		auto get_player_name(uintptr_t player_state) -> std::string
		{
			auto fstring = read<__int64>(player_state + 0xAB0);
			auto length = read<int>(fstring + 16);

			auto v6 = (__int64)length;

			if (!v6) return std::string(verucryptt("NPC"));

			auto IsABot = read<char>(player_state + 0x29A & 0x8);

			if (IsABot) return std::string(verucryptt("AI"));

			auto ftext = (uintptr_t)read<__int64>(fstring + 8);

			wchar_t* buffer = new wchar_t[length];
			//read(PVOID(static_cast<ULONGLONG>(ftext)), buffer, length * sizeof(wchar_t));

			char v21;
			int v22;
			int i;

			int v25;
			UINT16* v23;

			v21 = v6 - 1;
			if (!(UINT32)v6)
				v21 = 0;
			v22 = 0;
			v23 = (UINT16*)buffer;
			for (i = (v21) & 3; ; *v23++ += i & 7)
			{
				v25 = v6 - 1;
				if (!(UINT32)v6)
					v25 = 0;
				if (v22 >= v25)
					break;
				i += 3;
				++v22;
			}

			std::wstring username{ buffer };
			delete[] buffer;
			return std::string(username.begin(), username.end());
		}

		auto in_screen(fvector2d screen_location) -> bool {

			if (screen_location.x > 0 && screen_location.x < screen_width && screen_location.y > 0 && screen_location.y < screen_height) return true;
			else return false;
		}


		camera_position_s get_camera()
		{
			camera_position_s camera;

			auto location_pointer = read <uintptr_t>(arrays->UWorld + 0x128);
			auto rotation_pointer = read <uintptr_t>(arrays->UWorld + 0x138);

			struct FNRot
			{
				double a;
				char pad_0008[24];
				double b;
				char pad_0028[424];
				double c;
			} fnRot;

			fnRot.a = read <double>(rotation_pointer);
			fnRot.b = read <double>(rotation_pointer + 0x20);
			fnRot.c = read <double>(rotation_pointer + 0x1d0);

			camera.location = read <fvector>(location_pointer);
			camera.rotation.x = asin(fnRot.c) * (180.0 / M_PI);
			camera.rotation.y = ((atan2(fnRot.a * -1, fnRot.b) * (180.0 / M_PI)) * -1) * -1;
			camera.fov = read<float>((uintptr_t)arrays->player_controller + 0x3AC) * 90.f;

			return camera;
		}

		void RadarRange(float* x, float* y, float range)
		{
			if (fabs((*x)) > range || fabs((*y)) > range)
			{
				if ((*y) > (*x))
				{
					if ((*y) > -(*x))
					{
						(*x) = range * (*x) / (*y);
						(*y) = range;
					}
					else
					{
						(*y) = -range * (*y) / (*x);
						(*x) = -range;
					}
				}
				else
				{
					if ((*y) > -(*x))
					{
						(*y) = range * (*y) / (*x);
						(*x) = range;
					}
					else
					{
						(*x) = -range * (*x) / (*y);
						(*y) = -range;
					}
				}
			}
		}


		struct FPlane : fvector
		{
			double W;

			FPlane() : W(0) { }
			FPlane(double W) : W(W) { }
		};

		class FMatrix
		{
		public:
			double m[4][4];
			FPlane XPlane, YPlane, ZPlane, WPlane;

			FMatrix() : XPlane(), YPlane(), ZPlane(), WPlane() { }
			FMatrix(FPlane XPlane, FPlane YPlane, FPlane ZPlane, FPlane WPlane)
				: XPlane(XPlane), YPlane(YPlane), ZPlane(ZPlane), WPlane(WPlane) { }
		};

		inline double RadiansToDegrees(double dRadians)
		{
			return dRadians * (180.0 / M_PI);
		}

		template< typename t >
		class TArray
		{
		public:

			TArray() : tData(), iCount(), iMaxCount() { }
			TArray(t* data, int count, int max_count) :
				tData(tData), iCount(iCount), iMaxCount(iMaxCount) { }

		public:

			auto Get(int idx) -> t
			{
				return read< t >(reinterpret_cast<__int64>(this->tData) + (idx * sizeof(t)));
			}

			auto Size() -> std::uint32_t
			{
				return this->iCount;
			}

			bool IsValid()
			{
				return this->iCount != 0;
			}

			t* tData;
			int iCount;
			int iMaxCount;
		};

		auto GetViewState() -> uintptr_t
		{
			TArray<uintptr_t> ViewState = read<TArray<uintptr_t>>(arrays->local_player + 0xD0);
			return ViewState.Get(1);
		}

		camera_position_s get_view_point()
		{
			auto mProjection = read<FMatrix>(GetViewState() + 0x900);

			camera_position_s camera_position;
			camera_postion.rotation.x = RadiansToDegrees(std::asin(mProjection.ZPlane.W));
			camera_postion.rotation.y = RadiansToDegrees(std::atan2(mProjection.YPlane.W, mProjection.XPlane.W));
			camera_postion.rotation.z = 0.0;

			camera_postion.location.x = mProjection.m[3][0];
			camera_postion.location.y = mProjection.m[3][1];
			camera_postion.location.z = mProjection.m[3][2];

			float FieldOfView = atanf(1 / read<double>(GetViewState() + 0x700)) * 2;
			camera_postion.fov = FieldOfView * (180.f / M_PI);

			return camera_position;
		}



		inline fvector2d w2s(fvector WorldLocation)
		{
			camera_position_s camera_position = get_view_point();

			if (WorldLocation.x == 0 && WorldLocation.y == 0 && WorldLocation.z == 0)
				return fvector2d(0, 0);


			_MATRIX tempMatrix = Matrix(camera_postion.rotation);

			fvector vAxisX = fvector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
			fvector vAxisY = fvector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
			fvector vAxisZ = fvector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

			fvector vDelta = WorldLocation - camera_postion.location;
			fvector vTransformed = fvector(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));

			if (vTransformed.z < 1.f)
				vTransformed.z = 1.f;

			return fvector2d(
				(screen_width / 2.0f) + vTransformed.x * ((screen_width / 2.0f) / tanf(camera_postion.fov * (float)M_PI / 360.f)) / vTransformed.z,
				(screen_height / 2.0f) - vTransformed.y * ((screen_width / 2.0f) / tanf(camera_postion.fov * (float)M_PI / 360.f)) / vTransformed.z
			);
		}

		static auto Bone(uintptr_t skeletal_mesh, int bone_index) -> fvector {

			uintptr_t bone_array = read<uintptr_t>(skeletal_mesh + offset::BoneArray); //Bone Array ud
			if (bone_array == NULL) bone_array = read<uintptr_t>(skeletal_mesh + offset::BoneArray + 0x10);
			FTransform bone = read<FTransform>(bone_array + (bone_index * 0x60));
			FTransform component_to_world = read<FTransform>(skeletal_mesh + 0x1c0);
			D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());
			return fvector(matrix._41, matrix._42, matrix._43);
		}

		auto IsShootable(fvector lur, fvector wl) -> bool {

			if (lur.x >= wl.x - 20 && lur.x <= wl.x + 20 && lur.y >= wl.y - 20 && lur.y <= wl.y + 20 && lur.z >= wl.z - 30 && lur.z <= wl.z + 30)
				return true;
			else
				return false;

		}


		static auto is_visible(uintptr_t skeletal_mesh) -> bool {

			float fLastSubmitTime = read<float>(offset::SkeletalMeshes + offset::LastSubmitTime);
			float fLastRenderTimeOnScreen = read<float>(offset::SkeletalMeshes + offset::LastRenderTimeOnScreen);
			const float fVisionTick = 0.06f;
			bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
			return bVisible;
		}

		fvector Prediction(fvector TargetPosition, fvector ComponentVelocity, float player_distance, float ProjectileSpeed = 239)
		{
			float gravity = 3.5;
			float TimeToTarget = player_distance / ProjectileSpeed;
			float bulletDrop = abs(gravity) * (TimeToTarget * TimeToTarget) * 0.5;
			return fvector
			{
			TargetPosition.x + TimeToTarget * ComponentVelocity.x,
			TargetPosition.y + TimeToTarget * ComponentVelocity.y,
			TargetPosition.z + TimeToTarget * ComponentVelocity.z + bulletDrop
			};
		}



		__forceinline auto skeleton(uintptr_t mesh) -> void
		{  //wait
			const int visibleColorSkeleton = ImColor(globals::g_color_skeleton[0], globals::g_color_skeleton[1], globals::g_color_skeleton[2], 1.0f);
			const int notVisibleColor = ImColor(globals::g_color_invisible[0], globals::g_color_invisible[1], globals::g_color_invisible[2], 1.0f);


			fvector bonePositions[] = {
				Bone(mesh, 110),  // HeadBone
				Bone(mesh, 67),   // Neck
				Bone(mesh, 7),    // Chest
				Bone(mesh, 2),    // Pelvis
				Bone(mesh, 9),    // RightShoulder
				Bone(mesh, 10),   // RightElbow
				Bone(mesh, 11),   // RightWrist
				Bone(mesh, 38),   // LeftShoulder
				Bone(mesh, 39),   // LeftElbow
				Bone(mesh, 40),   // LeftWrist
				Bone(mesh, 71),   // RightHip
				Bone(mesh, 72),   // RightKnee
				Bone(mesh, 73),   // RightAnkle
				Bone(mesh, 78),   // LeftHip
				Bone(mesh, 79),   // LeftKnee
				Bone(mesh, 80)    // LeftAnkle
			};

			fvector2d bonePositionsOut[16];
			for (int i = 0; i < 16; ++i) {
				bonePositionsOut[i] = w2s(bonePositions[i]);
			}

			//// Affiche le cercle de la tête uniquement si globals::headesp est activé
			if (globals::headesp) {
				float headCircleRadius = 15.0f; // Ajuster pour la taille souhaitée du cercle de la tête
				ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(bonePositionsOut[0].x, bonePositionsOut[0].y), headCircleRadius, visibleColorSkeleton, 50, globals::g_skeletonthickness);
			}

			// Head to neck
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[0].x, bonePositionsOut[0].y), ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), visibleColorSkeleton, globals::g_skeletonthickness);

			// Neck to chest
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), visibleColorSkeleton, globals::g_skeletonthickness);

			// Chest to pelvis
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), visibleColorSkeleton, globals::g_skeletonthickness);

			// Right arm
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), visibleColorSkeleton, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), visibleColorSkeleton, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImVec2(bonePositionsOut[6].x, bonePositionsOut[6].y), visibleColorSkeleton, globals::g_skeletonthickness);

			// Left arm
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), ImVec2(bonePositionsOut[7].x, bonePositionsOut[7].y), visibleColorSkeleton, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[7].x, bonePositionsOut[7].y), ImVec2(bonePositionsOut[8].x, bonePositionsOut[8].y), visibleColorSkeleton, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[8].x, bonePositionsOut[8].y), ImVec2(bonePositionsOut[9].x, bonePositionsOut[9].y), visibleColorSkeleton, globals::g_skeletonthickness);

			// Right leg
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), visibleColorSkeleton, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), visibleColorSkeleton, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), ImVec2(bonePositionsOut[12].x, bonePositionsOut[12].y), visibleColorSkeleton, globals::g_skeletonthickness);

			// Left leg
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), ImVec2(bonePositionsOut[13].x, bonePositionsOut[13].y), visibleColorSkeleton, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[13].x, bonePositionsOut[13].y), ImVec2(bonePositionsOut[14].x, bonePositionsOut[14].y), visibleColorSkeleton, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[14].x, bonePositionsOut[14].y), ImVec2(bonePositionsOut[15].x, bonePositionsOut[15].y), visibleColorSkeleton, globals::g_skeletonthickness);
		}


	};  static uee::ue* ue5 = new uee::ue();
}


