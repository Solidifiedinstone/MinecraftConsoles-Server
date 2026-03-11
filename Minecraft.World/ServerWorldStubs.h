#pragma once
// ServerWorldStubs.h - Server build stubs for Minecraft.World
// Included from Minecraft.World/stdafx.h when _DEDICATED_SERVER is defined.
// Provides client-side types needed by World code that are unavailable for server builds.
//
// IMPORTANT: When Minecraft.Client server .cpp files are compiled with /FI stdafx_server.h,
// they may also include Minecraft.World/stdafx.h (which includes this file). Since
// stdafx_server.h already provides all these stubs, we skip this entire file in that case.
#ifdef _STDAFX_SERVER_H_INCLUDED
// stdafx_server.h already provides all stubs - skip to avoid redefinition errors
#else // !_STDAFX_SERVER_H_INCLUDED - World-only build, provide stubs

//=============================================================================
// C4JRender stub namespace
// Needed by Minecraft.Client/Textures.h which is included by some World .cpp files
//=============================================================================
#ifndef C4JRENDER_STUBS_DEFINED
#define C4JRENDER_STUBS_DEFINED
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
#endif

//=============================================================================
// C4JStorage stub namespace additions
// Needed by ClientConnection.h (EMessageResult), App_structs.h
//=============================================================================
namespace C4JStorage {
	// Save game state values
	enum ESaveGame {
		ESaveGame_Idle = 0,
		ESaveGame_Saving,
		ESaveGame_Loading,
		ESaveGame_Error
	};

	// Message result (needed by ClientConnection.h)
	enum EMessageResult {
		eRESULT_OK = 0,
		eRESULT_CANCEL,
		eRESULT_ERROR
	};

	// Storage facility (needed by some client headers included from World)
	enum eStorageFacilityEnum {
		eFACILITY_NONE = 0,
		eFACILITY_LOCAL,
		eFACILITY_REMOTE
	};
	typedef eStorageFacilityEnum eGlobalStorage;

	enum eFileTypeValEnum {
		eFILE_NONE = 0,
		eFILE_SAVE,
		eFILE_WORLD
	};
	typedef eFileTypeValEnum eTMS_FILETYPEVAL;
	typedef void* PTMSPP_FILEDATA;
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
// XMem / XPhysical function stubs
// Needed by ByteArrayInputStream.cpp, ByteArrayOutputStream.cpp,
//           CompressedTileStorage.cpp, FlatLevelSource.cpp, etc.
//=============================================================================
#ifndef _SERVER_XMEM_DEFINED
#define _SERVER_XMEM_DEFINED
#include <cstring>
#include <cstdlib>
inline void XMemCpy(void* dst, const void* src, size_t sz) { memcpy(dst, src, sz); }
inline void XMemSet(void* dst, int val, size_t sz) { memset(dst, val, sz); }
inline void XMemSet128(void* dst, int val, size_t sz) { memset(dst, val, sz); }
inline void* XPhysicalAlloc(SIZE_T sz, ULONG_PTR, ULONG_PTR, DWORD) { return malloc(sz); }
inline void XPhysicalFree(void* p) { free(p); }
#endif

// XSetThreadProcessor stub (needed by C4JThread.cpp)
#ifndef XSetThreadProcessor
inline void XSetThreadProcessor(HANDLE, int) {}
#endif

//=============================================================================
// ColourTable - use the real header.
// eMinecraftColour is available from App_enums.h (included before this in stdafx.h).
// Including the real header avoids C2011 redefinition errors when .cpp files also include it
// (MSVC #pragma once prevents double processing via PCH).
//=============================================================================
#include "..\Minecraft.Client\Common\Colours\ColourTable.h"

//=============================================================================
// INetworkPlayer interface
// Needed by Socket.h (included from Connection.cpp, etc.)
//=============================================================================
#include "..\Minecraft.Client\Common\Network\NetworkPlayerInterface.h"

//=============================================================================
// Forward declarations needed by CMinecraftApp stub methods
//=============================================================================
class LocalPlayer;
class LevelChunk;
class LevelRuleset;
class LevelGenerationOptions;

//=============================================================================
// STRING_VERIFY_RESPONSE stub
// Needed by SignTileEntity.h
//=============================================================================
#ifndef STRING_VERIFY_RESPONSE_DEFINED
#define STRING_VERIFY_RESPONSE_DEFINED
struct STRING_VERIFY_RESPONSE {
	int result;
	wchar_t *pszVerifiedString;
};
#endif

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
// Xbox content type constants
// Needed by ConsoleSaveFileOriginal.cpp, ConsoleSaveFileSplit.cpp, DLCPack.h
//=============================================================================
#ifndef XCONTENT_MAX_FILENAME_LENGTH
const int XCONTENT_MAX_FILENAME_LENGTH = 256;
#endif
#ifndef XCONTENT_MAX_DISPLAYNAME_LENGTH
const int XCONTENT_MAX_DISPLAYNAME_LENGTH = 256;
#endif
#ifndef XCONTENTDEVICEID
typedef int XCONTENTDEVICEID;
#endif

//=============================================================================
// VER_PRODUCTBUILD constant
//=============================================================================
#ifndef VER_PRODUCTBUILD
#define VER_PRODUCTBUILD 0
#endif

//=============================================================================
// GAME_RULE_SAVENAME
// Needed by ConsoleSaveFileConverter.cpp
//=============================================================================
#ifndef GAME_RULE_SAVENAME
#define GAME_RULE_SAVENAME L"requiredGameRules.grf"
#endif

//=============================================================================
// NON_QNET_SENDDATA_ACK_REQUIRED
// Needed by Connection.cpp, Socket.cpp
//=============================================================================
#ifndef NON_QNET_SENDDATA_ACK_REQUIRED
const int NON_QNET_SENDDATA_ACK_REQUIRED = 0x100;
#endif

//=============================================================================
// boolean type - Java-ism used in some World .cpp files
//=============================================================================
#ifndef _BOOLEAN_TYPEDEF_DEFINED
#define _BOOLEAN_TYPEDEF_DEFINED
typedef bool boolean;
#endif

//=============================================================================
// hash_value - needed by Hasher.cpp, Player.cpp
// MSVC doesn't have std::hash_value; provide it via std::hash
//=============================================================================
#ifndef _HASH_VALUE_DEFINED
#define _HASH_VALUE_DEFINED
#include <functional>
template<typename T>
inline size_t hash_value(const T& v) { return std::hash<T>()(v); }
#endif

//=============================================================================
// StringTable stub class
// Needed by DLCTexturePack.h, LevelRuleset.h
// The real StringTable.h uses byteArray which may not be available here.
//=============================================================================
#ifndef STRINGTABLE_STUB_DEFINED
#define STRINGTABLE_STUB_DEFINED
class StringTable {
public:
	StringTable() {}
	StringTable(PBYTE, DWORD) {}
	~StringTable() {}
	LPCWSTR getString(const wstring &key) { return L""; }
	LPCWSTR getString(int id) { return L""; }
	void getData(PBYTE *, UINT *) {}
	void ReloadStringTable() {}
};
#endif

//=============================================================================
// CMinecraftApp stub and 'app' extern
// Needed by Arrow.cpp, BasicTree.cpp, BiomeCache.cpp, BiomeDecorator.cpp,
//           AttributeModifier.cpp, BeaconTileEntity.cpp, etc.
// The full definition is in stdafx_server.h for Minecraft.Client builds.
//=============================================================================

// eGameHostOption_Max - needed by CMinecraftApp
#ifndef EGAMEHOSTOPTION_MAX_DEFINED
#define EGAMEHOSTOPTION_MAX_DEFINED
const int eGameHostOption_Max = eGameHostOption_DoDaylightCycle + 1;
#endif

#ifndef _CMINECRAFTAPP_DEFINED
#define _CMINECRAFTAPP_DEFINED
class CMinecraftApp {
public:
	// Debug output
	void DebugPrintf(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
	bool DebugSettingsOn() { return false; }
	bool DebugArtToolsOn() { return false; }
	int GetGameSettingsDebugMask() { return 0; }
	int GetGameSettingsDebugMask(int) { return 0; }
	void SetGameSettingsDebugMask(int, unsigned int) {}

	// Time / string
	__int64 getAppTime() { return (__int64)GetTickCount64(); }
	LPCWSTR GetString(int id) { return L""; }
	int GetHTMLColor(eMinecraftColour colour) { return 0xFFFFFF; }

	// Game host options
	DWORD GetGameHostOption(DWORD /*packed*/, eGameHostOption /*opt*/) const { return 0; }
	DWORD GetGameHostOption(eGameHostOption /*opt*/) const { return 0; }
	void SetGameHostOption(eGameHostOption /*opt*/, DWORD /*val*/) {}

	// Game settings
	unsigned char GetGameSettings(int /*iPad*/, eGameSetting /*eVal*/) { return 0; }
	unsigned char GetGameSettings(eGameSetting /*eVal*/) { return 0; }

	// Terrain features
	void AddTerrainFeaturePosition(_eTerrainFeatureType /*t*/, int /*x*/, int /*z*/) {}
	bool GetTerrainFeaturePosition(_eTerrainFeatureType /*t*/, int* /*px*/, int* /*pz*/) { return false; }

	// World generation
	LevelGenerationOptions* getLevelGenerationOptions() { return NULL; }
	LevelRuleset* getGameRuleDefinitions() { return NULL; }
	void processSchematics(LevelChunk* /*lc*/) {}
	void processSchematicsLighting(LevelChunk* /*lc*/) {}
	bool GetResetNether() { return false; }
	int GetGameNewWorldSize() { return 0; }
	bool GetGameNewWorldSizeUseMoat() { return false; }
	unsigned int GetGameNewHellScale() { return 3; }
	bool isTutorial() { return false; }

	// Mob settings
	bool GetMobsDontAttackEnabled() { return false; }
	bool GetMobsDontTickEnabled() { return false; }

	// Save/load helpers
	bool GetWriteSavesToFolderEnabled() { return false; }
	void FatalLoadError() {}
	unsigned int CreateImageTextData(PBYTE /*bTextMetadata*/, __int64 /*seed*/, bool /*hasSeed*/,
		unsigned int /*uiHostOptions*/, unsigned int /*uiTexturePackId*/) { return 0; }
	void GetMemFileDetails(const wstring&, PBYTE* pbData, DWORD* dwBytes) {
		if (pbData) *pbData = NULL; if (dwBytes) *dwBytes = 0;
	}
	bool IsFileInMemoryTextures(const wstring&) { return false; }
	void AddMemoryTextureFile(const wstring&, void*, int) {}
	void EnterSaveNotificationSection() {}
	void LeaveSaveNotificationSection() {}

	// Player identity
	bool isXuidNotch(PlayerUID /*xuid*/) { return false; }

	// XUI server action (needed by Level.cpp etc.)
	eXuiServerAction GetXuiServerAction(int) { return eXuiServerAction_Idle; }
	LPVOID GetXuiServerActionParam(int) { return NULL; }
	void SetXuiServerAction(int, eXuiServerAction) {}
};
#endif // _CMINECRAFTAPP_DEFINED

extern CMinecraftApp app;

//=============================================================================
// ProfileManager stub
// Needed by BiomeSource.cpp, DirectoryLevelStorage.cpp, etc.
//=============================================================================
#ifndef _PROFILEMANAGER_STUB_DEFINED
#define _PROFILEMANAGER_STUB_DEFINED
class CProfileManager {
public:
	bool IsSignedIn(int) { return true; }
	bool IsFullVersion() { return true; }
	int GetPrimaryPad() { return 0; }
	PlayerUID GetPlayerUID(int) { return 0; }
	const wchar_t* GetGamertagW(int) { static wchar_t name[] = L"Server"; return name; }
	bool AllowedToPlayMultiplayer(int) { return true; }
	void QuerySigninStatus() {}
	static CProfileManager& GetInstance() { static CProfileManager inst; return inst; }
};
#define ProfileManager CProfileManager::GetInstance()
#endif

//=============================================================================
// StorageManager stub
// Needed by File.cpp, ConsoleSaveFileOriginal.cpp, DirectoryLevelStorage.cpp, etc.
//=============================================================================
#ifndef _STORAGEMANAGER_STUB_DEFINED
#define _STORAGEMANAGER_STUB_DEFINED
class CStorageManager {
public:
	bool GetSaveDisabled() { return false; }
	int GetSaveState() { return C4JStorage::ESaveGame_Idle; }
	bool IsSaving() { return false; }
	bool IsLoading() { return false; }
	C4JStorage::ESaveGame GetSaveGameState() { return C4JStorage::ESaveGame_Idle; }
	wstring GetMountedPath() { return L""; }
	wstring GetMountedPath(int) { return L""; }
	void Tick() {}
	static CStorageManager& GetInstance() { static CStorageManager inst; return inst; }
};
#define StorageManager CStorageManager::GetInstance()
#endif

//=============================================================================
// TelemetryManager stub
// Needed by ConsoleSaveFileOriginal.cpp, ConsoleSaveFileSplit.cpp
// Note: Common/Telemetry/TelemetryManager.h is a no-op for _DEDICATED_SERVER builds,
// so this stub does not conflict with that header.
//=============================================================================
#ifndef _TELEMETRYMANAGER_STUB_DEFINED
#define _TELEMETRYMANAGER_STUB_DEFINED
class CTelemetryManager {
public:
	void RecordPlayerSessionStart(int) {}
	void RecordPlayerSessionExit(int, int) {}
	void RecordLevelStart(int, int, int, int, int, int) {}
	int GenerateMultiplayerInstanceId() { return 0; }
	int GetMultiplayerInstanceID() { return 0; }
	void SetMultiplayerInstanceId(int) {}
	static CTelemetryManager* GetInstance() { static CTelemetryManager inst; return &inst; }
};
#define TelemetryManager CTelemetryManager::GetInstance()
#endif

//=============================================================================
// CGameNetworkManager forward declaration + g_NetworkManager extern
// Needed by Connection.cpp, Level.cpp, LevelChunk.cpp
// The full CGameNetworkManager class is defined in stdafx_server.h
//=============================================================================
#ifndef _CNETWORKMANAGER_FORWARD_DEFINED
#define _CNETWORKMANAGER_FORWARD_DEFINED
class CGameNetworkManager;
extern CGameNetworkManager g_NetworkManager;
#endif

//=============================================================================
// ShutdownManager namespace
// Needed by C4JThread.cpp, Connection.cpp
//=============================================================================
namespace ShutdownManager {
	enum Thread {
		eServerThread,
		ePostProcessThread,
		eRunUpdateThread,
		eEventQueueThreads,
		eConnectionReadThreads,
		eConnectionWriteThreads
	};
	inline void HasStarted(Thread) {}
	inline void HasStarted(Thread, void*) {}
	inline void HasFinished(Thread) {}
	inline void HasFinished(Thread, void*) {}
	inline bool ShouldRun(Thread) { return true; }
	inline bool ShouldRun(Thread, void*) { return true; }
}

//=============================================================================
// String resource IDs (IDS_*)
// Included from PSVitaMedia/strings.h which has the canonical set of IDS_ defines
//=============================================================================
#include "..\Minecraft.Client\PSVitaMedia\strings.h"

#endif // !_STDAFX_SERVER_H_INCLUDED
