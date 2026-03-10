// stdafx_server.h - Complete precompiled header for Dedicated Server
// Provides all necessary types and stubs for headless server operation

#pragma once

// Force dedicated server mode
#ifndef _DEDICATED_SERVER
#define _DEDICATED_SERVER 1
#endif

#ifndef _WINDOWS64
#define _WINDOWS64 1
#endif

#define WIN32_LEAN_AND_MEAN

// Windows headers (must come before standard headers for proper CRITICAL_SECTION etc)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// Standard C/C++ headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <cstdarg>
#include <cassert>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <functional>

// Type definitions
#ifndef __uint64
typedef unsigned long long __uint64;
#endif

// Macros
#define AUTO_VAR(_var, _val) auto _var = _val
#ifndef HRESULT_SUCCEEDED
#define HRESULT_SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

// Using declarations
using std::wstring;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::weak_ptr;

//=============================================================================
// DirectX Stubs (server doesn't need graphics)
//=============================================================================
struct XMFLOAT2 { float x, y; XMFLOAT2() : x(0), y(0) {} XMFLOAT2(float _x, float _y) : x(_x), y(_y) {} };
struct XMFLOAT3 { float x, y, z; XMFLOAT3() : x(0), y(0), z(0) {} XMFLOAT3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {} };
struct XMFLOAT4 { float x, y, z, w; XMFLOAT4() : x(0), y(0), z(0), w(0) {} };
struct XMMATRIX { float m[4][4]; };
typedef XMFLOAT4 XMVECTOR;

inline XMVECTOR XMVectorSet(float x, float y, float z, float w) { return XMFLOAT4(); }
inline float XMVectorGetX(XMVECTOR v) { return v.x; }
inline float XMVectorGetY(XMVECTOR v) { return v.y; }
inline float XMVectorGetZ(XMVECTOR v) { return v.z; }

// D3D11 stubs
typedef void* ID3D11Device;
typedef void* ID3D11DeviceContext;
typedef void* ID3D11RenderTargetView;
typedef void* ID3D11Texture2D;
typedef void* ID3D11ShaderResourceView;

//=============================================================================
// PIX Stubs (performance instrumentation)
//=============================================================================
inline void PIXBeginNamedEvent(DWORD, const char*, ...) {}
inline void PIXEndNamedEvent() {}
inline void PIXReportCounter(const wchar_t*, float) {}

//=============================================================================
// Platform Constants
//=============================================================================
#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 4
#endif
#ifndef XUSER_INDEX_ANY
#define XUSER_INDEX_ANY 255
#endif
#ifndef XUSER_INDEX_FOCUS
#define XUSER_INDEX_FOCUS 254
#endif
#define MINECRAFT_NET_MAX_PLAYERS 8

//=============================================================================
// Undefine any Windows macros that conflict with game code
//=============================================================================
#ifdef BIGENDIAN
#undef BIGENDIAN
#endif
#ifdef LITTLEENDIAN
#undef LITTLEENDIAN
#endif

//=============================================================================
// extraX64.h types (MUST come before Minecraft.World includes)
//=============================================================================
typedef unsigned char byte;
typedef ULONGLONG PlayerUID;
typedef ULONGLONG GameSessionUID;

// Compression types needed by compression.h
typedef VOID* XMEMCOMPRESSION_CONTEXT;
typedef VOID* XMEMDECOMPRESSION_CONTEXT;

// XMem codec types
typedef enum _XMEMCODEC_TYPE {
    XMEMCODEC_DEFAULT = 0,
    XMEMCODEC_LZX = 1
} XMEMCODEC_TYPE;

typedef struct _XMEMCODEC_PARAMETERS_LZX {
    DWORD Flags;
    DWORD WindowSize;
    DWORD CompressionPartitionSize;
} XMEMCODEC_PARAMETERS_LZX;

// Stub XMem functions
inline HRESULT XMemCreateCompressionContext(XMEMCODEC_TYPE, const VOID*, DWORD, XMEMCOMPRESSION_CONTEXT*) { return S_OK; }
inline HRESULT XMemCreateDecompressionContext(XMEMCODEC_TYPE, const VOID*, DWORD, XMEMDECOMPRESSION_CONTEXT*) { return S_OK; }
inline HRESULT XMemCompress(XMEMCOMPRESSION_CONTEXT, VOID*, SIZE_T*, const VOID*, SIZE_T) { return S_OK; }
inline HRESULT XMemDecompress(XMEMDECOMPRESSION_CONTEXT, VOID*, SIZE_T*, const VOID*, SIZE_T) { return S_OK; }
inline void XMemDestroyCompressionContext(XMEMCOMPRESSION_CONTEXT) {}
inline void XMemDestroyDecompressionContext(XMEMDECOMPRESSION_CONTEXT) {}

//=============================================================================
// Include core Minecraft.World headers (these are platform-independent)
//=============================================================================
#include "../../Minecraft.World/Definitions.h"
#include "../../Minecraft.World/class.h"
#include "../../Minecraft.World/ArrayWithLength.h"
#include "../../Minecraft.World/SharedConstants.h"
#include "../../Minecraft.World/Random.h"
#include "../../Minecraft.World/compression.h"
#include "../../Minecraft.World/PerformanceTimer.h"

//=============================================================================
// Additional extraX64.h types (networking)
//=============================================================================

// Forward declarations for network types
class IQNetPlayer;
class IQNet;
class IQNetCallbacks;
class IQNetGameSearch;

// Network state enum
typedef enum _QNET_STATE {
    QNET_STATE_IDLE,
    QNET_STATE_SESSION_HOSTING,
    QNET_STATE_SESSION_JOINING,
    QNET_STATE_GAME_LOBBY,
    QNET_STATE_SESSION_REGISTERING,
    QNET_STATE_SESSION_STARTING,
    QNET_STATE_GAME_PLAY,
    QNET_STATE_SESSION_ENDING,
    QNET_STATE_SESSION_LEAVING,
    QNET_STATE_SESSION_DELETING
} QNET_STATE;

// IQNetPlayer stub - represents a network player
class IQNetPlayer {
public:
    BYTE GetSmallId() { return 0; }
    void SendData(IQNetPlayer*, const void*, DWORD, DWORD) {}
    bool IsSameSystem(IQNetPlayer*) { return true; }
    DWORD GetSendQueueSize(IQNetPlayer*, DWORD) { return 0; }
    DWORD GetCurrentRtt() { return 0; }
    bool IsHost() { return true; }
    bool IsGuest() { return false; }
    bool IsLocal() { return true; }
    PlayerUID GetXuid() { return 0; }
    LPCWSTR GetGamertag() { return L"Server"; }
    int GetSessionIndex() { return 0; }
    bool IsTalking() { return false; }
    bool IsMutedByLocalUser(DWORD) { return false; }
    bool HasVoice() { return false; }
    bool HasCamera() { return false; }
    int GetUserIndex() { return 0; }
    void SetCustomDataValue(ULONG_PTR) {}
    ULONG_PTR GetCustomDataValue() { return 0; }
};

// IQNet stub - network session manager
class IQNet {
public:
    HRESULT AddLocalPlayerByUserIndex(DWORD) { return S_OK; }
    IQNetPlayer* GetHostPlayer() { return nullptr; }
    IQNetPlayer* GetLocalPlayerByUserIndex(DWORD) { return nullptr; }
    IQNetPlayer* GetPlayerByIndex(DWORD) { return nullptr; }
    IQNetPlayer* GetPlayerBySmallId(BYTE) { return nullptr; }
    IQNetPlayer* GetPlayerByXuid(PlayerUID) { return nullptr; }
    DWORD GetPlayerCount() { return 0; }
    QNET_STATE GetState() { return QNET_STATE_IDLE; }
    bool IsHost() { return true; }
    HRESULT JoinGameFromInviteInfo(DWORD, DWORD, const void*) { return S_OK; }
    void HostGame() {}
    void EndGame() {}
    static IQNetPlayer m_player[4];
};

// Network constants
const int QNET_SENDDATA_LOW_PRIORITY = 0;
const int QNET_SENDDATA_SECONDARY = 0;
const int QNET_SENDDATA_RELIABLE = 0;
const int QNET_SENDDATA_SEQUENTIAL = 0;
const int QNET_GETSENDQUEUESIZE_SECONDARY_TYPE = 0;
const int QNET_GETSENDQUEUESIZE_MESSAGES = 0;
const int QNET_GETSENDQUEUESIZE_BYTES = 0;
const int QNET_E_SESSION_FULL = 0;
const int QNET_USER_MASK_USER0 = 1;
const int QNET_USER_MASK_USER1 = 2;
const int QNET_USER_MASK_USER2 = 4;
const int QNET_USER_MASK_USER3 = 8;
const int INVALID_XUID = 0;

struct SessionID {
    BYTE data[16];
    SessionID() { memset(data, 0, sizeof(data)); }
    bool operator==(const SessionID& other) const { return memcmp(data, other.data, sizeof(data)) == 0; }
    bool operator!=(const SessionID& other) const { return !(*this == other); }
};

struct INVITE_INFO {
    SessionID sessionId;
    PlayerUID hostId;
};

//=============================================================================
// 4JLibs Stubs - Minimal interfaces for server operation
//=============================================================================

// Input Manager Stub
class CInputManager {
public:
    void Tick() {}
    bool IsPadLocked(int) { return false; }
    int GetNextPad() { return 0; }
    void SetPadLocked(int, bool) {}
    void ClearAllInput() {}
    static CInputManager& GetInstance() { static CInputManager inst; return inst; }
};
#define InputManager CInputManager::GetInstance()

// Profile Manager Stub
class CProfileManager {
public:
    bool IsSignedIn(int) { return true; }
    bool IsSignedInLive(int) { return false; }
    bool IsFullVersion() { return true; }
    int GetPrimaryPad() { return 0; }
    int GetLockedProfile() { return 0; }
    void SetPrimaryPad(int) {}
    void SetLockedProfile(int) {}
    const char* GetGamertag(int) { return "Server"; }
    const wchar_t* GetGamertagW(int) { static wchar_t name[] = L"Server"; return name; }
    bool AllowedToPlayMultiplayer(int) { return true; }
    void QuerySigninStatus() {}
    void SetCurrentGameActivity(int, int, bool) {}
    void Tick() {}
    void* GetUser(int, bool) { return nullptr; }
    bool getNPAvailability(int) { return false; }
    bool HasPlayStationPlus(int) { return false; }
    void GetChatAndContentRestrictions(int, bool, bool*, bool*, void*) {}
    void AllowedPlayerCreatedContent(int, bool, BOOL*, BOOL*) { }
    void DisplaySystemMessage(int, int) {}
    PlayerUID GetPlayerUID(int) { return 0; }
    static CProfileManager& GetInstance() { static CProfileManager inst; return inst; }
};
#define ProfileManager CProfileManager::GetInstance()

// Render Manager Stub
class CRenderManager {
public:
    void StartFrame() {}
    void Present() {}
    void Tick() {}
    bool IsHiDef() { return true; }
    int GetScreenWidth() { return 1920; }
    int GetScreenHeight() { return 1080; }
    static CRenderManager& GetInstance() { static CRenderManager inst; return inst; }
};
#define RenderManager CRenderManager::GetInstance()

// Storage Manager Stub
class CStorageManager {
public:
    bool GetSaveDisabled() { return false; }
    int GetSaveState() { return 0; }
    bool IsSaving() { return false; }
    bool IsLoading() { return false; }
    void Tick() {}
    static CStorageManager& GetInstance() { static CStorageManager inst; return inst; }
};
#define StorageManager CStorageManager::GetInstance()

//=============================================================================
// GUI/Rendering Stubs (server doesn't render)
//=============================================================================

// Forward declarations
class Textures;
class Font;
class Gui;
class Screen;
class BufferedImage;
class ScreenSizeCalculator;

// Textures stub
class Textures {
public:
    int loadTexture(const wstring&) { return 0; }
    void bindTexture(int) {}
    void tick() {}
};

// Font stub
class Font {
public:
    void draw(const wstring&, int, int, int) {}
    int width(const wstring&) { return 0; }
    int width(const char*) { return 0; }
};

// Gui stub
class Gui {
public:
    void tick() {}
    void clearMessages() {}
};

// Screen stub
class Screen {
public:
    virtual void render(int, int, float) {}
    virtual void tick() {}
    virtual void init() {}
    virtual void removed() {}
};

// ScreenSizeCalculator stub
class ScreenSizeCalculator {
public:
    int getWidth() { return 1920; }
    int getHeight() { return 1080; }
    float getScale() { return 1.0f; }
};

// BufferedImage stub
class BufferedImage {
public:
    int width, height;
    BufferedImage() : width(0), height(0) {}
};

//=============================================================================
// Minecraft Client Stubs
//=============================================================================

// Forward declare what we need
class MultiPlayerLevel;
class MultiPlayerGameMode;
class MultiplayerLocalPlayer;
class GameRenderer;
class User;
class Options;
class TexturePackRepository;
class ProgressRenderer;

// Minimal Minecraft class stub for server
class Minecraft {
public:
    ProgressRenderer* progressRenderer;
    GameRenderer* gameRenderer;
    MultiPlayerLevel* level;
    User* user;
    Options* options;
    Textures* textures;
    Font* font;
    Screen* screen;
    Gui* gui;
    TexturePackRepository* skins;

    shared_ptr<MultiplayerLocalPlayer> localplayers[XUSER_MAX_COUNT];
    void* m_pendingLocalConnections[XUSER_MAX_COUNT];
    bool m_connectionFailed[XUSER_MAX_COUNT];
    int m_connectionFailedReason[XUSER_MAX_COUNT];

    Minecraft() : progressRenderer(nullptr), gameRenderer(nullptr), level(nullptr),
                  user(nullptr), options(nullptr), textures(nullptr), font(nullptr),
                  screen(nullptr), gui(nullptr), skins(nullptr) {
        memset(m_pendingLocalConnections, 0, sizeof(m_pendingLocalConnections));
        memset(m_connectionFailed, 0, sizeof(m_connectionFailed));
        memset(m_connectionFailedReason, 0, sizeof(m_connectionFailedReason));
    }

    static Minecraft* GetInstance() { static Minecraft inst; return &inst; }
    bool isTutorial() { return false; }
    void clearConnectionFailed() {}
    void connectionDisconnected(int, int) {}
    void addPendingLocalConnection(int, void*) {}
    void removeLocalPlayerIdx(int) {}
    void clearPendingClientTextureRequests() {}
    void forceStatsSave(int) {}
};

//=============================================================================
// App and UI Stubs
//=============================================================================

// Game host options enum
enum eGameHostOption {
    eGameHostOption_GameType = 0,
    eGameHostOption_Difficulty,
    eGameHostOption_PvP,
    eGameHostOption_FireSpreads,
    eGameHostOption_TNT,
    eGameHostOption_Structures,
    eGameHostOption_BonusChest,
    eGameHostOption_LevelType,
    eGameHostOption_All,
    eGameHostOption_FriendsOfFriends,
    eGameHostOption_Gamertags,
    eGameHostOption_HasBeenInCreative,
    eGameHostOption_Max
};

// XUI Server Actions
enum eXuiServerAction {
    eXuiServerAction_None = 0,
    eXuiServerAction_SaveGame,
    eXuiServerAction_AutoSaveGame,
    eXuiServerAction_PauseGame,
    eXuiServerAction_ResumeGame,
    eXuiServerAction_DropItem,
};

// Forward declare GameRuleManager
class GameRuleManager {
public:
    void loadCurrentGameRules() {}
    void unloadCurrentGameRules() {}
    void saveToFile() {}
};

// Minimal CMinecraftApp stub for server
class CMinecraftApp {
public:
    GameRuleManager m_gameRules;
    DWORD m_hostOptions[eGameHostOption_Max];

    CMinecraftApp() { memset(m_hostOptions, 0, sizeof(m_hostOptions)); }

    DWORD GetGameHostOption(eGameHostOption opt) const {
        if (opt == eGameHostOption_All) {
            DWORD packed = 0;
            packed |= (m_hostOptions[eGameHostOption_GameType] & 0x1);
            packed |= (m_hostOptions[eGameHostOption_Difficulty] & 0x3) << 1;
            packed |= (m_hostOptions[eGameHostOption_PvP] & 0x1) << 3;
            packed |= (m_hostOptions[eGameHostOption_FireSpreads] & 0x1) << 4;
            packed |= (m_hostOptions[eGameHostOption_TNT] & 0x1) << 5;
            packed |= (m_hostOptions[eGameHostOption_Structures] & 0x1) << 6;
            packed |= (m_hostOptions[eGameHostOption_BonusChest] & 0x1) << 7;
            packed |= (m_hostOptions[eGameHostOption_LevelType] & 0x1) << 8;
            return packed;
        }
        return (opt < eGameHostOption_Max) ? m_hostOptions[opt] : 0;
    }

    void SetGameHostOption(eGameHostOption opt, DWORD val) {
        if (opt == eGameHostOption_All) {
            m_hostOptions[eGameHostOption_GameType] = val & 0x1;
            m_hostOptions[eGameHostOption_Difficulty] = (val >> 1) & 0x3;
            m_hostOptions[eGameHostOption_PvP] = (val >> 3) & 0x1;
            m_hostOptions[eGameHostOption_FireSpreads] = (val >> 4) & 0x1;
            m_hostOptions[eGameHostOption_TNT] = (val >> 5) & 0x1;
            m_hostOptions[eGameHostOption_Structures] = (val >> 6) & 0x1;
            m_hostOptions[eGameHostOption_BonusChest] = (val >> 7) & 0x1;
            m_hostOptions[eGameHostOption_LevelType] = (val >> 8) & 0x1;
        } else if (opt < eGameHostOption_Max) {
            m_hostOptions[opt] = val;
        }
    }

    void DebugPrintf(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }

    void EnterSaveNotificationSection() {}
    void LeaveSaveNotificationSection() {}
};

extern CMinecraftApp app;

// UI stub
class CUI {
public:
    void tick() {}
    void render() {}
    void NavigateToScene(int, int, void*, int = 0, int = 0) {}
    void NavigateToHomeMenu() {}
    void UpdatePlayerBasePositions() {}
    bool IsSceneInStack(int, int) { return false; }
    bool IsReloadingSkin() { return false; }
    void CleanUpSkinReload() {}
    void RequestErrorMessage(int, int, unsigned int*, int, int, void* = nullptr, void* = nullptr) {}
    void RequestUGCMessageBox(int, int, int = 0) {}
    void RequestContentRestrictedMessageBox(int, int, int = 0) {}
    void RequestAlertMessage(int, int, unsigned int*, int, int, void*, void*) {}
    void SetTrialTimerLimitSecs(int) {}
    static CUI& GetInstance() { static CUI inst; return inst; }
};
#define ui CUI::GetInstance()

//=============================================================================
// Telemetry Stub
//=============================================================================
class CTelemetryManager {
public:
    void RecordPlayerSessionStart(int) {}
    void RecordPlayerSessionExit(int, int) {}
    void RecordLevelStart(int, int, int, int, int, int) {}
    int GenerateMultiplayerInstanceId() { return 0; }
    void SetMultiplayerInstanceId(int) {}
    static CTelemetryManager* GetInstance() { static CTelemetryManager inst; return &inst; }
};
#define TelemetryManager CTelemetryManager::GetInstance()

//=============================================================================
// Shutdown Manager
//=============================================================================
namespace ShutdownManager {
    enum Thread { eServerThread, ePostProcessThread };
    inline void HasStarted(Thread) {}
    inline void HasFinished(Thread) {}
    inline bool ShouldRun(Thread) { return true; }
}

//=============================================================================
// Memory Tracking Stub
//=============================================================================
inline void MemSect(int) {}

//=============================================================================
// String resources stub
//=============================================================================
#define IDS_PROGRESS_INITIALISING_SERVER 1001
#define IDS_PROGRESS_GENERATING_SPAWN_AREA 1002
#define IDS_PROGRESS_LOADING_SPAWN_AREA 1003
#define IDS_PROGRESS_SAVING_CHUNKS 1004

//=============================================================================
// Network structures
//=============================================================================
struct NetworkGameInitData {
    __int64 seed;
    bool findSeed;
    DWORD settings;
    void* levelGen;
    int texturePackId;
    int xzSize;
    int hellScale;

    NetworkGameInitData() : seed(0), findSeed(true), settings(0), levelGen(nullptr),
                            texturePackId(0), xzSize(0), hellScale(3) {}
};

//=============================================================================
// Include GameNetworkManager (needed for server)
//=============================================================================
#include "../Common/Network/GameNetworkManager.h"

// End of stdafx_server.h
