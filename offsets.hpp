#include <cstdint>

enum bone : int {
    Head = 110,
    Neck = 67,
    Chest = 66,
    Pelvis = 2,
    LShoulder = 9,
    LElbow = 10,
    LHand = 11,
    RShoulder = 38,
    RElbow = 39,
    RHand = 40,
    LHip = 71,
    LKnee = 72,
    LFoot = 73,
    RHip = 78,
    RKnee = 79,
    RFoot = 82,
    Root = 0
};

namespace offset {
    uintptr_t uworld = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t GObjects = 0x12E4DF88;
    uintptr_t GNames = 0x12EFC740;
    uintptr_t game_instance = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t DefaultPawnPtr = 0x180;
    uintptr_t game_state = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t bAdsWhileNotOnGround = 0x5619;
    uintptr_t local_player = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t PawnMaterials_ALL = 0x5ca0;
    uintptr_t SkeletalMeshes = 0x4bd0;
    uintptr_t Parent = 0x100;
    uintptr_t WireFrame = 0x1b0;
    uintptr_t player_controller = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t acknowledged_pawn = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t pname = 0xAA8;
    uintptr_t mesh = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t DefaultPacket = 0x328;
    uintptr_t PlayerState = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t root_component = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t velocity = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t relative_location = 0x120;
    uintptr_t relative_rotation = 0x140;
    uintptr_t team_index = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t bot_index = 0x29a;
    uintptr_t StaticMeshComponent = 0x290;
    uintptr_t player_array = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t PawnPriv = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t CurrentVehicle = 0x2a10;
    uintptr_t CurrentWeapon = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t AmmoCount = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t WeaponData = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t BoneCache = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t BoneArray = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t LastRenderTimeOnScreen = 0x2E8;
    uintptr_t FOVMinimum = 0x2738;
    uintptr_t FOVMaximum = 0x273c;
    uintptr_t LastSubmitTime = 0x2E0;
    uintptr_t TargetedFortPawn = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t component_to_world = 0xDEADBEEF; // DUMPED AUTOMATICALLY
    uintptr_t b_Allow_Targeting = 0xd3;
}

enum triggerbotoffsets {
    location_under_reticle = 0x2560,
};