#pragma once
#include <string>
#include <cstdint>

class OffsetAPI {
public:
    static uint64_t DumpOffset( std::string offset );
    static bool InitializeGame( );
};

// Current Offsets That The Dumper Can Find Is Listed Below
/*
GWORLD
Levels
PersistentLevel
GameState
OwningGameInstance
DefaultPhysicsVolume
OwningWorld
WorldSettings
PlayerArray
ServerWorldTimeSecondsDelta
LocalPlayers
Mesh
PlayerController
AcknowledgedPawn
NetConnection
RootComponent
Rarity
PlayerState
PawnPrivate
bIsABot
DefaultFlyTime
PrimaryPickupItemEntry
ItemDefinition
ItemName
PlayerName
ComponentVelocity
SpawnSourceOverride
bAlreadySearched
HighestRarity
CurrentWeapon
bIsDBNO
bIsDying
WeaponData
AmmoCount
WorldGravityZ
TargetedFortPawn
HabaneroComponent
Platform
RankedProgress
TeamIndex
ReviveFromDBNOTime
BoundsScale
ProjectileSpeed
GravityScale
AActors
ComponentToWorld
BoneArray
BoneCache
*/