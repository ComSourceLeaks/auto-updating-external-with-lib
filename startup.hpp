#include "render.hpp"
#include <iostream>
#include "filesystem"
#include "auth.hpp"
#include "ImGui/imgui.h" // CHANGE TO UR FOLDER
#include <string>
#include <iostream>
#include <windows.h>
#include "offsetdumper.h"

bool offsets = true;
std::string key;

std::string tm_to_readable_time( tm ctx );
static std::time_t string_to_timet( std::string timestamp );
static std::tm timet_to_tm( time_t timestamp );
const std::string compilation_date = ( std::string )skCrypt( __DATE__ );
const std::string compilation_time = ( std::string )skCrypt( __TIME__ );

//void SigmaMigma() {
//	auto previous_cr3 = handler::CR3_Init();
//	int counter = 0;
//	while (true)
//	{
//		if (!arrays->UWorld) {
//			handler::CR3_Init();
//			std::this_thread::sleep_for(std::chrono::microseconds(100));
//		}
//	}
//}
using namespace KeyAuth;

std::string name = skCrypt( "vokesuk" ).decrypt( ); // App name
std::string ownerid = skCrypt( "zufguoPqn8" ).decrypt( ); // Account ID
std::string version = skCrypt( "1.0" ).decrypt( ); // Application version. Used for automatic downloads see video here https://www.youtube.com/watch?v=kW195PLCBKs
std::string url = skCrypt( "https://keyauth.win/api/1.3/" ).decrypt( ); // change if using KeyAuth custom domains feature
std::string path = skCrypt( "" ).decrypt( ); // (OPTIONAL) see tutorial here https://www.youtube.com/watch?v=I9rxt821gMk&t=1s

api KeyAuthApp( name, ownerid, version, url, path );

//void DownloadFile(std::string fileNumber, std::string fileName)
//{
//	std::vector<std::uint8_t> bytes = KeyAuthApp.download(fileNumber);
//
//	if (!KeyAuthApp.response.success)
//	{
//		Sleep(1500);
//		exit(0);
//	}
//
//	std::ofstream file(fileName, std::ios_base::out | std::ios_base::binary);
//	file.write((char*)bytes.data(), bytes.size());
//	file.close();
//}
void SetPath( std::string path )
{
    std::filesystem::current_path( path );
}

//void cr3_loop()
//{
//	for (;;)
//	{
//		handler::CR3_Init();
//		std::this_thread::sleep_for(std::chrono::milliseconds(3));
//	}
//}

static auto cr3_loop( ) -> void {

    auto LatestGworld = read<uintptr_t>( virtualaddy + offset::uworld );
    while ( true ) {
        arrays->UWorld = read<uintptr_t>( virtualaddy + offset::uworld );
        //printf("GWORLD ---> 0x%llx \n", arrays->UWorld);
        if ( LatestGworld != arrays->UWorld ) {
            auto cr3 = driver::CR3( );
            //printf("DTB ---> 0x%llx\n", cr3);
            LatestGworld = read<uintptr_t>( virtualaddy + offset::uworld );

        }



        std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
    }
}

class auth1 {
private:
    bool isAuthenticated = false;

public:
    void auth( ) {
        system( "cls" );
        KeyAuthApp.init( );
        std::cout << "\n [+] Product Key : ";
        std::string key;
        std::cin >> key;

        KeyAuthApp.license( key );

        if ( KeyAuthApp.response.success ) {
            system( "cls" );
            isAuthenticated = true;
            std::cout << "\n Authentication Successful!";
            Sleep( 3000 );
        }
        else {
            system( "cls" );
            std::cout << "\n Authentication Failed!";
            Sleep( 3000 );
        }
    }

    bool getAuthStatus( ) const { return isAuthenticated; }
};
static auth1* auth = new auth1;



class startup1
{
public:
    void SetupOffsets( ) {
        OffsetAPI::InitializeGame( );

        offset::uworld = OffsetAPI::DumpOffset( "gworld" );
        offset::game_instance = OffsetAPI::DumpOffset( "OwningGameInstance" );
        offset::game_state = OffsetAPI::DumpOffset( "GameState" );
        offset::local_player = OffsetAPI::DumpOffset( "LocalPlayers" );
        offset::player_controller = OffsetAPI::DumpOffset( "PlayerController" );
        offset::acknowledged_pawn = OffsetAPI::DumpOffset( "AcknowledgedPawn" );
        offset::mesh = OffsetAPI::DumpOffset( "Mesh" );
        offset::PlayerState = OffsetAPI::DumpOffset( "PlayerState" );
        offset::root_component = OffsetAPI::DumpOffset( "RootComponent" );
        offset::velocity = OffsetAPI::DumpOffset( "ComponentVelocity" );
        offset::PawnPriv = OffsetAPI::DumpOffset( "PawnPrivate" );
        offset::team_index = OffsetAPI::DumpOffset( "TeamIndex" );
        offset::player_array = OffsetAPI::DumpOffset( "PlayerArray" );
        offset::CurrentWeapon = OffsetAPI::DumpOffset( "CurrentWeapon" );
        offset::AmmoCount = OffsetAPI::DumpOffset( "AmmoCount" );
        offset::WeaponData = OffsetAPI::DumpOffset( "WeaponData" );
        offset::BoneCache = OffsetAPI::DumpOffset( "BoneCache" );
        offset::BoneArray = OffsetAPI::DumpOffset( "BoneArray" );
        offset::TargetedFortPawn = OffsetAPI::DumpOffset( "TargetedFortPawn" );
        offset::component_to_world = OffsetAPI::DumpOffset( "ComponentToWorld" );
    }
    void Startup( )
    {
        this->SetupOffsets( );
        Sleep( -1 );
        system( "cls" );
        if ( !auth->getAuthStatus( ) ) {
            std::cout << "\n Access denied!";
            Sleep( 3000 );
            exit( 0 );

        }
        else {
            std::cout << "\n Access Granted!";
            Sleep( 3000 );
        }

        system( "cls" );
        std::cout << "\n Welcome to Vokes Private!";

        int msgBoxResult = MessageBox( NULL, L"Load Driver (CLOSE FORTNITE)", L"Vokes", MB_YESNO | MB_ICONQUESTION );

        if ( msgBoxResult == IDYES ) {
            std::printf( "\nLoading Driver\n" );
            system( "C:\\Windows\\Fonts\\Map.exe C:\\Windows\\Fonts\\Driver.sys > NUL 2>&1" );

            remove( "C:\\Windows\\Fonts\\Map.exe > NUL 2>&1" );
            remove( "C:\\Windows\\Fonts\\Driver.sys > NUL 2>&1" );
        }
        else {
            remove( "C:\\Windows\\Fonts\\Map.exe > NUL 2>&1" );
            remove( "C:\\Windows\\Fonts\\Driver.sys > NUL 2>&1" );
        }

        MessageBox( NULL, L"Open Fortnite then press OK in lobby", L"Orqur", MB_OK | MB_ICONINFORMATION );

        driver::init( );

        Sleep( 2000 );

        while ( hwnd == NULL )
        {
            XorS( wind, "Fortnite  " );
            hwnd = FindWindowA( 0, wind.decrypt( ) );
            Sleep( 100 );
        }

        driver::process_id = driver::get_process_id( L"FortniteClient-Win64-Shipping.exe" );
        virtualaddy = driver::get_base_address( );

        driver::CR3( );
        std::thread( [ & ]( ) { { cr3_loop( ); } } ).detach( );

        std::thread( [ & ]( ) { for ( ;; ) { g_cache->actors( ); } } ).detach( );
        std::cout << "\n PID -> " << driver::process_id;

        g_render->Overlay( );

        std::cout << "\n Debug Stuff: ";
        std::cout << "\n ";
    }
};
static startup1* startup = new startup1;
