// stdafx_server.h - Precompiled header for Dedicated Server
// Only provides PLATFORM stubs - game types come from Minecraft.World headers

#pragma once

// Force dedicated server mode
#ifndef _DEDICATED_SERVER
#define _DEDICATED_SERVER 1
#endif

#ifndef _WINDOWS64
#define _WINDOWS64 1
#endif

#define WIN32_LEAN_AND_MEAN

// Windows headers
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
#include <cmath>
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
#include <atomic>
#include <mutex>
#include <thread>

// Type definitions that must come BEFORE Minecraft.World headers
#ifndef __uint64
typedef unsigned long long __uint64;
#endif

#ifndef byte
typedef unsigned char byte;
#endif

typedef ULONGLONG PlayerUID;
typedef ULONGLONG GameSessionUID;

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
// Undefine Windows macros that conflict with Minecraft.World
//=============================================================================
#ifdef BIGENDIAN
#undef BIGENDIAN
#endif
#ifdef LITTLEENDIAN
#undef LITTLEENDIAN
#endif

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

typedef void* ID3D11Device;
typedef void* ID3D11DeviceContext;
typedef void* ID3D11RenderTargetView;
typedef void* ID3D11Texture2D;
typedef void* ID3D11ShaderResourceView;

//=============================================================================
// PIX Stubs
//=============================================================================
inline void PIXBeginNamedEvent(DWORD, const char*, ...) {}
inline void PIXEndNamedEvent() {}
inline void PIXReportCounter(const wchar_t*, float) {}
inline void PIXAddNamedCounter(int, char*, ...) {}
inline void PIXSetMarkerDeprecated(int, char*, ...) {}

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
// Compression stubs (extraX64.h equivalent)
//=============================================================================
typedef VOID* XMEMCOMPRESSION_CONTEXT;
typedef VOID* XMEMDECOMPRESSION_CONTEXT;

typedef enum _XMEMCODEC_TYPE {
    XMEMCODEC_DEFAULT = 0,
    XMEMCODEC_LZX = 1
} XMEMCODEC_TYPE;

typedef struct _XMEMCODEC_PARAMETERS_LZX {
    DWORD Flags;
    DWORD WindowSize;
    DWORD CompressionPartitionSize;
} XMEMCODEC_PARAMETERS_LZX;

inline HRESULT XMemCreateCompressionContext(XMEMCODEC_TYPE, const VOID*, DWORD, XMEMCOMPRESSION_CONTEXT*) { return S_OK; }
inline HRESULT XMemCreateDecompressionContext(XMEMCODEC_TYPE, const VOID*, DWORD, XMEMDECOMPRESSION_CONTEXT*) { return S_OK; }
inline HRESULT XMemCompress(XMEMCOMPRESSION_CONTEXT, VOID*, SIZE_T*, const VOID*, SIZE_T) { return S_OK; }
inline HRESULT XMemDecompress(XMEMDECOMPRESSION_CONTEXT, VOID*, SIZE_T*, const VOID*, SIZE_T) { return S_OK; }
inline void XMemDestroyCompressionContext(XMEMCOMPRESSION_CONTEXT) {}
inline void XMemDestroyDecompressionContext(XMEMDECOMPRESSION_CONTEXT) {}

//=============================================================================
// IQNet network types (stub implementations)
//=============================================================================
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

class IQNetCallbacks {};
class IQNetGameSearch {};

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

//=============================================================================
// Xbox-like types
//=============================================================================
typedef struct _XOVERLAPPED {} XOVERLAPPED, *PXOVERLAPPED;
typedef struct _XSESSION_SEARCHRESULT {} XSESSION_SEARCHRESULT, *PXSESSION_SEARCHRESULT;
typedef struct _XSESSION_SEARCHRESULT_HEADER {
    DWORD dwSearchResults;
    XSESSION_SEARCHRESULT *pResults;
} XSESSION_SEARCHRESULT_HEADER, *PXSESSION_SEARCHRESULT_HEADER;

typedef struct {
    BYTE bFlags;
    BYTE bReserved;
    WORD cProbesXmit;
    WORD cProbesRecv;
    WORD cbData;
    BYTE *pbData;
    WORD wRttMinInMsecs;
    WORD wRttMedInMsecs;
    DWORD dwUpBitsPerSec;
    DWORD dwDnBitsPerSec;
} XNQOSINFO;

typedef struct {
    UINT cxnqos;
    UINT cxnqosPending;
    XNQOSINFO axnqosinfo[1];
} XNQOS;

typedef struct {
    DWORD dwContextId;
    DWORD dwValue;
} XUSER_CONTEXT, *PXUSER_CONTEXT;

typedef struct _XUSER_SIGNIN_INFO {
    PlayerUID xuid;
    DWORD dwGuestNumber;
} XUSER_SIGNIN_INFO, *PXUSER_SIGNIN_INFO;

const int XUSER_NAME_SIZE = 32;
#define XUSER_GET_SIGNIN_INFO_ONLINE_XUID_ONLY 0x00000001
#define XUSER_GET_SIGNIN_INFO_OFFLINE_XUID_ONLY 0x00000002

inline DWORD XUserGetSigninInfo(DWORD, DWORD, PXUSER_SIGNIN_INFO) { return 0; }

typedef struct _XONLINE_FRIEND {
    PlayerUID xuid;
    CHAR szGamertag[XUSER_NAME_SIZE];
    DWORD dwFriendState;
    BYTE sessionID[16];
    DWORD dwTitleID;
    FILETIME ftUserTime;
    BYTE xnkidInvite[16];
    FILETIME gameinviteTime;
    DWORD cchRichPresence;
} XONLINE_FRIEND, *PXONLINE_FRIEND;

const int XCONTENT_MAX_DISPLAYNAME_LENGTH = 256;
const int XCONTENT_MAX_FILENAME_LENGTH = 256;
typedef int XCONTENTDEVICEID;

typedef struct _XCONTENT_DATA {
    XCONTENTDEVICEID DeviceID;
    DWORD dwContentType;
    WCHAR szDisplayName[XCONTENT_MAX_DISPLAYNAME_LENGTH];
    CHAR szFileName[XCONTENT_MAX_FILENAME_LENGTH];
} XCONTENT_DATA, *PXCONTENT_DATA;

struct XRNM_SEND_BUFFER {
    DWORD dwDataSize;
    byte *pbyData;
};

const int D3DBLEND_CONSTANTALPHA = 0;
const int D3DBLEND_INVCONSTANTALPHA = 0;
const int D3DPT_QUADLIST = 0;

const int XC_LANGUAGE_ENGLISH = 0x01;
const int XC_LANGUAGE_JAPANESE = 0x02;
const int XC_LANGUAGE_GERMAN = 0x03;
const int XC_LANGUAGE_FRENCH = 0x04;
const int XC_LANGUAGE_SPANISH = 0x05;
const int XC_LANGUAGE_ITALIAN = 0x06;
const int XC_LANGUAGE_KOREAN = 0x07;
const int XC_LANGUAGE_TCHINESE = 0x08;
const int XC_LANGUAGE_PORTUGUESE = 0x09;
const int XC_LOCALE_UNITED_STATES = 36;

inline DWORD XGetLanguage() { return XC_LANGUAGE_ENGLISH; }
inline DWORD XGetLocale() { return XC_LOCALE_UNITED_STATES; }
inline DWORD XEnableGuestSignin(BOOL) { return 0; }

inline void XSetThreadProcessor(HANDLE, int) {}

class CXuiStringTable {
public:
    LPCWSTR Lookup(LPCWSTR) { return L""; }
    LPCWSTR Lookup(UINT) { return L""; }
    void Clear() {}
    HRESULT Load(LPCWSTR) { return S_OK; }
};

inline void XMemCpy(void* a, const void* b, size_t s) { memcpy(a, b, s); }
inline void XMemSet(void* a, int t, size_t s) { memset(a, t, s); }
inline void XMemSet128(void* a, int t, size_t s) { memset(a, t, s); }
inline void* XPhysicalAlloc(SIZE_T, ULONG_PTR, ULONG_PTR, DWORD) { return nullptr; }
inline void XPhysicalFree(void*) {}

class D3DXVECTOR3 {
public:
    D3DXVECTOR3() : x(0), y(0), z(0), pad(0) {}
    D3DXVECTOR3(float _x, float _y, float _z) : x(_x), y(_y), z(_z), pad(0) {}
    float x, y, z, pad;
    D3DXVECTOR3& operator+=(const D3DXVECTOR3& add) { x += add.x; y += add.y; z += add.z; return *this; }
};

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
// 4JLibs Stubs
//=============================================================================
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
// GUI/Rendering Stubs
//=============================================================================
class Textures {
public:
    int loadTexture(const wstring&) { return 0; }
    void bindTexture(int) {}
    void tick() {}
};

class Font {
public:
    void draw(const wstring&, int, int, int) {}
    int width(const wstring&) { return 0; }
    int width(const char*) { return 0; }
};

class Gui {
public:
    void tick() {}
    void clearMessages() {}
};

class Screen {
public:
    virtual void render(int, int, float) {}
    virtual void tick() {}
    virtual void init() {}
    virtual void removed() {}
    virtual ~Screen() {}
};

class ScreenSizeCalculator {
public:
    int getWidth() { return 1920; }
    int getHeight() { return 1080; }
    float getScale() { return 1.0f; }
};

class BufferedImage {
public:
    int width, height;
    BufferedImage() : width(0), height(0) {}
};

class GameRenderer {};

class User {
public:
    wstring name;
    User() : name(L"Server") {}
};

class Options {};
class TexturePackRepository {};

// Forward declare Minecraft client types
class MultiPlayerLevel;
class MultiPlayerGameMode;
class MultiplayerLocalPlayer;
class Packet;
class ProgressRenderer;

//=============================================================================
// Minecraft class stub
//=============================================================================
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
// Game host options enum
//=============================================================================
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

enum eXuiServerAction {
    eXuiServerAction_None = 0,
    eXuiServerAction_SaveGame,
    eXuiServerAction_AutoSaveGame,
    eXuiServerAction_PauseGame,
    eXuiServerAction_ResumeGame,
    eXuiServerAction_DropItem,
};

//=============================================================================
// CMinecraftApp stub
//=============================================================================
class GameRuleManager {
public:
    void loadCurrentGameRules() {}
    void unloadCurrentGameRules() {}
    void saveToFile() {}
};

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

//=============================================================================
// CUI stub
//=============================================================================
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
// Telemetry stub
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
// Memory tracking
//=============================================================================
inline void MemSect(int) {}

//=============================================================================
// String resource IDs
//=============================================================================
#define IDS_PROGRESS_INITIALISING_SERVER 1001
#define IDS_PROGRESS_GENERATING_SPAWN_AREA 1002
#define IDS_PROGRESS_LOADING_SPAWN_AREA 1003
#define IDS_PROGRESS_SAVING_CHUNKS 1004

//=============================================================================
// INetworkPlayer (forward declaration for network manager)
//=============================================================================
class INetworkPlayer {
public:
    virtual ~INetworkPlayer() {}
    virtual PlayerUID GetXuid() const { return 0; }
    virtual const wchar_t* GetGamertag() const { return L"Player"; }
    virtual bool IsLocal() const { return false; }
    virtual bool IsHost() const { return false; }
    virtual int GetUserIndex() const { return 0; }
    virtual BYTE GetSmallId() const { return 0; }
    virtual void SendPacket(Packet*) {}
};

//=============================================================================
// Platform Network Manager Stub
//=============================================================================
class CPlatformNetworkManager {
public:
    enum eJoinFailedReason { eJoinFailed_General };
    static CPlatformNetworkManager* GetInstance() { static CPlatformNetworkManager inst; return &inst; }
};

class CPlatformNetworkManagerStub : public CPlatformNetworkManager {};

//=============================================================================
// Session Info
//=============================================================================
struct FriendSessionInfo {
    SessionID sessionId;
    wstring hostGamertag;
    PlayerUID hostXuid;
    int playerCount;
    int maxPlayers;
    bool isPrivate;
    FriendSessionInfo() : hostXuid(0), playerCount(0), maxPlayers(8), isPrivate(false) {}
};

//=============================================================================
// CGameNetworkManager stub
//=============================================================================
class ClientConnection;

class CGameNetworkManager {
public:
    enum eJoinGameResult { JOINGAME_SUCCESS, JOINGAME_FAIL_GENERAL, JOINGAME_FAIL_SERVER_FULL };

    CGameNetworkManager() {}
    void Initialise() {}
    void Terminate() {}
    void DoWork() {}
    bool _RunNetworkGame(LPVOID) { return false; }
    bool StartNetworkGame(Minecraft*, LPVOID) { return false; }
    int CorrectErrorIDS(int IDS) { return IDS; }

    static int GetLocalPlayerMask(int playerIndex) { return 1 << playerIndex; }
    int GetPlayerCount() { return 0; }
    int GetOnlinePlayerCount() { return 0; }
    bool AddLocalPlayerByUserIndex(int) { return false; }
    bool RemoveLocalPlayerByUserIndex(int) { return false; }
    INetworkPlayer* GetLocalPlayerByUserIndex(int) { return nullptr; }
    INetworkPlayer* GetPlayerByIndex(int) { return nullptr; }
    INetworkPlayer* GetPlayerByXuid(PlayerUID) { return nullptr; }
    INetworkPlayer* GetPlayerBySmallId(unsigned char) { return nullptr; }
    wstring GetDisplayNameByGamertag(wstring gt) { return gt; }
    INetworkPlayer* GetHostPlayer() { return nullptr; }
    void RegisterPlayerChangedCallback(int, void (*)(void*, INetworkPlayer*, bool), void*) {}
    void UnRegisterPlayerChangedCallback(int, void (*)(void*, INetworkPlayer*, bool), void*) {}
    void HandleSignInChange() {}
    bool ShouldMessageForFullSession() { return false; }

    bool IsInSession() { return false; }
    bool IsInGameplay() { return false; }
    bool IsLeavingGame() { return false; }
    bool IsReadyToPlayOrIdle() { return true; }

    bool SetLocalGame(bool) { return true; }
    bool IsLocalGame() { return true; }
    void SetPrivateGame(bool) {}
    bool IsPrivateGame() { return false; }
    void HostGame(int, bool, bool, unsigned char = 8, unsigned char = 0) {}
    bool IsHost() { return true; }
    bool IsInStatsEnabledSession() { return false; }

    bool SessionHasSpace(unsigned int = 1) { return true; }
    vector<FriendSessionInfo*>* GetSessionList(int, int, bool) { return nullptr; }
    bool GetGameSessionInfo(int, SessionID, FriendSessionInfo*) { return false; }
    void SetSessionsUpdatedCallback(void (*)(LPVOID), LPVOID) {}
    void GetFullFriendSessionInfo(FriendSessionInfo*, void (*)(bool, void*), void*) {}
    void ForceFriendsSessionRefresh() {}

    bool JoinGameFromInviteInfo(int, int, const INVITE_INFO*) { return false; }
    eJoinGameResult JoinGame(FriendSessionInfo*, int) { return JOINGAME_SUCCESS; }
    static void CancelJoinGame(LPVOID) {}
    bool LeaveGame(bool) { return true; }
    static int JoinFromInvite_SignInReturned(void*, bool, int) { return 0; }
    void UpdateAndSetGameSessionData(INetworkPlayer* = nullptr) {}
    void SendInviteGUI(int) {}
    void ResetLeavingGame() {}

    bool IsNetworkThreadRunning() { return false; }
    static int RunNetworkGameThreadProc(void*) { return 0; }
    static int ServerThreadProc(void*) { return 0; }
#ifdef _DEDICATED_SERVER
    static int DedicatedServerMain(void*) { return 0; }
#endif
    static int ExitAndJoinFromInviteThreadProc(void*) { return 0; }
    static void _LeaveGame() {}
    static int ChangeSessionTypeThreadProc(void*) { return 0; }

    void SystemFlagSet(INetworkPlayer*, int) {}
    bool SystemFlagGet(INetworkPlayer*, int) { return false; }

    void ServerReadyCreate(bool) {}
    void ServerReady() {}
    void ServerReadyWait() {}
    void ServerReadyDestroy() {}
    bool ServerReadyValid() { return false; }

    void ServerStoppedCreate(bool) {}
    void ServerStopped() {}
    void ServerStoppedWait() {}
    void ServerStoppedDestroy() {}
    bool ServerStoppedValid() { return false; }

    wstring GatherStats() { return L""; }
    void renderQueueMeter() {}
    wstring GatherRTTStats() { return L""; }

    static const int messageQueue_length = 512;
    static __int64 messageQueue[messageQueue_length];
    static const int byteQueue_length = 512;
    static __int64 byteQueue[byteQueue_length];
    static int messageQueuePos;

    void FakeLocalPlayerJoined() {}
};

extern CGameNetworkManager g_NetworkManager;

//=============================================================================
// Socket class stub
//=============================================================================
class Socket {
public:
    static void Initialise(void*) {}
    static void Shutdown() {}
};

//=============================================================================
// Color type (needed before Minecraft.World headers)
//=============================================================================
typedef DWORD eMinecraftColour;
const eMinecraftColour MC_WHITE = 0xFFFFFFFF;
const eMinecraftColour MC_BLACK = 0xFF000000;

//=============================================================================
// Include Minecraft.World headers in correct order
// These provide game types (Vec3, AABB, Entity, Level, Random, etc.)
//=============================================================================
#include "../../Minecraft.World/Definitions.h"
#include "../../Minecraft.World/class.h"
#include "../../Minecraft.World/ArrayWithLength.h"
#include "../../Minecraft.World/SharedConstants.h"
#include "../../Minecraft.World/Random.h"
#include "../../Minecraft.World/compression.h"
#include "../../Minecraft.World/PerformanceTimer.h"
#include "../../Minecraft.World/C4JThread.h"

// End of stdafx_server.h
