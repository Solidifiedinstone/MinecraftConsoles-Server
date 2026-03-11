#pragma once
// ServerWorldStubs.h - Server build stubs for Minecraft.World
// Included from Minecraft.World/stdafx.h when _DEDICATED_SERVER is defined.
// Provides client-side types needed by World code that are unavailable for server builds.

//=============================================================================
// C4JRender stub namespace
// Needed by Minecraft.Client/Textures.h which is included by some World .cpp files
//=============================================================================
namespace C4JRender {
	enum eTextureFormat {
		eFMT_DEFAULT = 0,
		eFMT_DXT1,
		eFMT_DXT5,
		eFMT_A8R8G8B8,
		eFMT_R8G8B8,
		eFMT_A8,
		TEXTURE_FORMAT_RxGyBzAw = eFMT_A8R8G8B8
	};
	typedef eTextureFormat TEXTURE_FORMAT;
}

//=============================================================================
// PIX profiling stubs
// Needed by BasicTree.cpp, BiomeDecorator.cpp
//=============================================================================
#ifndef PIX_STUBS_DEFINED
#define PIX_STUBS_DEFINED
inline void PIXBeginNamedEvent(DWORD, const char*, ...) {}
inline void PIXEndNamedEvent() {}
inline void PIXReportCounter(const wchar_t*, float) {}
inline void PIXAddNamedCounter(int, char*, ...) {}
inline void PIXSetMarkerDeprecated(int, char*, ...) {}
#endif

//=============================================================================
// ColourTable stub
// Needed by ArmorItem.cpp, WaterLilyTile.cpp, GrassTile.cpp, etc.
// eMinecraftColour is available from App_enums.h (included before this in stdafx.h)
//=============================================================================
class ColourTable {
public:
	unsigned int getColour(eMinecraftColour id) { return 0xFFFFFF; }
	unsigned int getColor(eMinecraftColour id) { return 0xFFFFFF; }
};

//=============================================================================
// LocalPlayer forward declaration
// Needed by Stat.h (shared_ptr<LocalPlayer> in virtual method)
//=============================================================================
class LocalPlayer;

//=============================================================================
// STRING_VERIFY_RESPONSE stub
// Needed by SignTileEntity.h
//=============================================================================
struct STRING_VERIFY_RESPONSE {
	int result;
	wchar_t *pszVerifiedString;
};

//=============================================================================
// ETelemetryChallenges enum
// Needed by Minecraft.Client/ServerPlayer.h and SetHealthPacket.h
//=============================================================================
#include "..\Minecraft.Client\PSVita\Sentient\TelemetryEnum.h"

//=============================================================================
// INVALID_XUID constant
// Needed by AddPlayerPacket.cpp
//=============================================================================
#ifndef INVALID_XUID
const int INVALID_XUID = 0;
#endif

//=============================================================================
// CMinecraftApp stub and 'app' extern
// Needed by Arrow.cpp, BasicTree.cpp, BiomeCache.cpp, BiomeDecorator.cpp,
//           AttributeModifier.cpp, BeaconTileEntity.cpp, etc.
// The full definition is in stdafx_server.h for Minecraft.Client builds.
//=============================================================================
class LevelGenerationOptions;

#ifndef _CMINECRAFTAPP_DEFINED
#define _CMINECRAFTAPP_DEFINED
class CMinecraftApp {
public:
	void DebugPrintf(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
	LevelGenerationOptions* getLevelGenerationOptions() { return NULL; }
	__int64 getAppTime() { return (__int64)GetTickCount64(); }
	wstring GetString(int id) { return L""; }
	bool DebugSettingsOn() { return false; }
	int GetGameSettingsDebugMask() { return 0; }
	int GetGameSettingsDebugMask(int) { return 0; }
};
#endif // _CMINECRAFTAPP_DEFINED

extern CMinecraftApp app;

//=============================================================================
// String resource IDs (IDS_*)
// Included from PSVitaMedia/strings.h which has the canonical set of IDS_ defines
//=============================================================================
#include "..\Minecraft.Client\PSVitaMedia\strings.h"
