// stdafx_server.h - Precompiled header for Dedicated Server
// Only provides PLATFORM stubs - game types come from Minecraft.World headers

#pragma once

// Mark that stdafx_server.h has been included.
// ServerWorldStubs.h checks this to avoid redefining types already defined here.
#define _STDAFX_SERVER_H_INCLUDED

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

// Type definitions
#ifndef __uint64
typedef unsigned long long __uint64;
#endif

// Define types normally provided by extraX64.h
// These MUST be defined before any code that uses them
#ifndef _EXTRA_X64_TYPES_DEFINED
#define _EXTRA_X64_TYPES_DEFINED

#define _PLAYER_UID_DEFINED
typedef unsigned char byte;
typedef ULONGLONG PlayerUID;
typedef PlayerUID GameSessionUID;
typedef PlayerUID *PPlayerUID;

const int XUSER_INDEX_ANY = 255;
const int XUSER_INDEX_FOCUS = 254;
const int XUSER_MAX_COUNT = 4;
const int MINECRAFT_NET_MAX_PLAYERS = 8;

inline bool IsEqualXUID(PlayerUID a, PlayerUID b) { return a == b; }

#endif // _EXTRA_X64_TYPES_DEFINED

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

// D3D11 types come from d3d11.h if included, or are forward declared elsewhere

//=============================================================================
// PIX Stubs
//=============================================================================
inline void PIXBeginNamedEvent(DWORD, const char*, ...) {}
inline void PIXEndNamedEvent() {}
inline void PIXReportCounter(const wchar_t*, float) {}
inline void PIXAddNamedCounter(int, char*, ...) {}
inline void PIXSetMarkerDeprecated(int, char*, ...) {}

//=============================================================================
// Platform Constants (already defined above with other extraX64.h types)
//=============================================================================

//=============================================================================
// Compression stubs (extraX64.h equivalent)
//=============================================================================
#define _SERVER_XMEM_DEFINED
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

// Network protocol version (matches VER_NETWORK in Common/BuildVer.h)
#define MINECRAFT_NET_VERSION 560

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
    bool AreXUIDSEqual(PlayerUID a, PlayerUID b) { return a == b; }
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
    void GetSaveUniqueFilename(char *buf) { if(buf) buf[0] = '\0'; }
    static CStorageManager& GetInstance() { static CStorageManager inst; return inst; }
};
#define StorageManager CStorageManager::GetInstance()

//=============================================================================
// C4JRender stub namespace (for headers that reference C4JRender types)
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
// C4JStorage stub namespace (for headers that reference C4JStorage types)
//=============================================================================
namespace C4JStorage {
    // Note: eGlobalStorage, eTMS_FILETYPEVAL are used as TYPE NAMES in code
    // e.g. "C4JStorage::eGlobalStorage eStorageFacility;" - so they must be typedefs
    enum eStorageFacilityEnum {
        eFACILITY_NONE = 0,
        eFACILITY_LOCAL,
        eFACILITY_REMOTE
    };
    typedef eStorageFacilityEnum eStorageFacility;
    typedef eStorageFacilityEnum eGlobalStorage;  // Alias type name used in client code

    enum eFileTypeValEnum {
        eFILE_NONE = 0,
        eFILE_SAVE,
        eFILE_WORLD
    };
    typedef eFileTypeValEnum eFileTypeVal;
    typedef eFileTypeValEnum eTMS_FILETYPEVAL;  // Alias type name used in client code

    enum EMessageResult {
        eRESULT_OK = 0,
        eRESULT_CANCEL,
        eRESULT_ERROR
    };
    typedef void* PTMSPP_FILEDATA;

    // Callback type alias
    typedef int eOptionsCallback;
}

//=============================================================================
// Constants needed by various headers
//=============================================================================
const int MAX_SAVEFILENAME_LENGTH = 256;

//=============================================================================
// Telemetry enum - no platform dependencies, safe to include directly
//=============================================================================
#include "../PSVita/Sentient/TelemetryEnum.h"

//=============================================================================
// Iggy SDK stubs (font/UI rendering library - not needed for dedicated server)
//=============================================================================
#ifndef RADLINK
#define RADLINK __cdecl
#endif

typedef unsigned int U32;
typedef int S32;
typedef float F32;
typedef int rrbool;

struct IggyFontMetrics { float ascent; float descent; float advance; float leading; };
struct IggyGlyphMetrics { float advance; float x; float y; float width; float height; };
struct IggyBitmapCharacter { int width; int height; int x; int y; void *data; };
class IggyBitmapFontProvider {};

//=============================================================================
// Forward declarations for GUI/Rendering and other client types
// Note: These types are defined in Minecraft.Client headers - do NOT define stubs here
//=============================================================================
class ColourTable;
struct STRING_VERIFY_RESPONSE;
class LevelChunk;
class LevelRuleset;
class Textures;
class Font;
class Gui;
class Screen;
class ScreenSizeCalculator;
class BufferedImage;
class GameRenderer;
class User;
class Options;
class TexturePackRepository;
class MultiPlayerLevel;
class MultiPlayerGameMode;
class MultiplayerLocalPlayer;
class Packet;
class Minecraft;
class LocalPlayer;
class DLCPack;
class DLCSkinFile;
// SkinBox.h is self-contained; include it directly to get proper SKIN_BOX typedef
#include "../SkinBox.h"

// Minimal StringTable stub - DLCTexturePack.h calls getString() through pointer
class StringTable {
public:
    wstring getString(const wstring& key) { return L""; }
    wstring getString(const wstring& key, const wstring& defaultVal) { return defaultVal; }
    bool hasString(const wstring& key) { return false; }
};

// CDLCManager stub - DLC skin management (not needed for dedicated server)
class CDLCManager {
public:
    DLCSkinFile* getSkinFile(const wstring&) { return nullptr; }
};

// ProgressRenderer - server-safe stub (replaces client ProgressRenderer.h)
// Must have a default constructor since HeadlessProgressRenderer inherits from it
// and doesn't pass a Minecraft* to the parent ctor
#include "../../Minecraft.World/ProgressListener.h"
class ProgressRenderer : public ProgressListener {
public:
    ProgressRenderer() {}
    virtual ~ProgressRenderer() {}
    virtual void progressStart(int title) {}
    virtual void progressStartNoAbort(int str) {}
    virtual void progressStage(int status) {}
    virtual void progressStage(wstring &wstrText) {}
    virtual void progressStagePercentage(int i) {}
};

// DebugSetCameraPosition stub (from Common/UI/UIStructs.h - not included for dedicated server)
struct DebugSetCameraPosition {
    int player;
    float m_camX, m_camY, m_camZ;
    float m_yRot, m_elev;
};

//=============================================================================
// App defines - constants needed by App_structs.h (MAX_CAPENAME_SIZE etc.)
//=============================================================================
#include "../Common/App_Defines.h"

//=============================================================================
// Game host options and other enums - include from real header
//=============================================================================
#include "../Common/App_enums.h"

// Add eGameHostOption_Max which isn't in the original enum
const int eGameHostOption_Max = eGameHostOption_DoDaylightCycle + 1;

//=============================================================================
// CMinecraftApp stub
//=============================================================================
#define GAME_RULE_SAVENAME L"requiredGameRules.grf"

// Forward declaration needed by GameRuleManager methods
class LevelGenerationOptions;

class GameRuleManager {
public:
    void loadCurrentGameRules() {}
    void unloadCurrentGameRules() {}
    void saveToFile() {}
    LevelGenerationOptions* getLevelGenerationOptions() { return NULL; }
    LevelGenerationOptions* loadGameRules(byte* dIn, UINT dSize) { return NULL; }
    void loadGameRules(LevelGenerationOptions* lgo, byte* dIn, UINT dSize) {}
    void saveGameRules(byte** dOut, UINT* dSize) { if(dOut) *dOut = NULL; if(dSize) *dSize = 0; }
};

class CMinecraftApp {
public:
    GameRuleManager m_gameRules;
    CDLCManager m_dlcManager;
    DWORD m_hostOptions[eGameHostOption_Max];

    CMinecraftApp() { memset(m_hostOptions, 0, sizeof(m_hostOptions)); }

    // 2-arg version: unpack specific option from a packed settings DWORD
    DWORD GetGameHostOption(DWORD packed, eGameHostOption opt) const {
        switch(opt) {
            case eGameHostOption_GameType: return packed & 0x1;
            case eGameHostOption_Difficulty: return (packed >> 1) & 0x3;
            case eGameHostOption_PvP: return (packed >> 3) & 0x1;
            case eGameHostOption_FireSpreads: return (packed >> 4) & 0x1;
            case eGameHostOption_TNT: return (packed >> 5) & 0x1;
            case eGameHostOption_Structures: return (packed >> 6) & 0x1;
            case eGameHostOption_BonusChest: return (packed >> 7) & 0x1;
            case eGameHostOption_LevelType: return (packed >> 8) & 0x1;
            default: return 0;
        }
    }

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

    // Methods called by server-side .cpp files
    LevelRuleset* getGameRuleDefinitions() { return NULL; }
    bool IsFileInMemoryTextures(const wstring&) { return false; }
    void AddMemoryTextureFile(const wstring&, void*, int) {}
    void processSchematicsLighting(LevelChunk*) {}
    LevelGenerationOptions* getLevelGenerationOptions() { return m_gameRules.getLevelGenerationOptions(); }
    bool DebugSettingsOn() { return false; }
    int GetGameSettingsDebugMask() { return 0; }
    int GetGameSettingsDebugMask(int) { return 0; }
    void SetGameSettingsDebugMask(int, unsigned int) {}
    wstring GetString(int) { return L""; }
    bool GetTerrainFeaturePosition(_eTerrainFeatureType, int* px, int* pz) { return false; }
    eXuiServerAction GetXuiServerAction(int) { return eXuiServerAction_Idle; }
    LPVOID GetXuiServerActionParam(int) { return NULL; }
    void SetXuiServerAction(int, eXuiServerAction) {}
    bool isTutorial() { return false; }
    int GetGameNewWorldSize() { return 0; }
    bool GetGameNewWorldSizeUseMoat() { return false; }
    void GetMemFileDetails(const wstring&, PBYTE* pbData, DWORD* dwBytes) {
        if(pbData) *pbData = NULL; if(dwBytes) *dwBytes = 0;
    }
    vector<SKIN_BOX*>* GetAdditionalSkinBoxes(DWORD) { return nullptr; }
    unsigned int GetAnimOverrideBitmask(DWORD) { return 0; }
    void SetAdditionalSkinBoxes(DWORD, void*, int) {}
    void SetAnimOverrideBitmask(DWORD, unsigned int) {}
    int getSkinIdFromPath(const wstring&) { return 0; }
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
// Note: Don't use #define ui as it conflicts with Windows oleauto.h
// Use CUI::GetInstance() directly or define a unique macro name
inline CUI& GetUIInstance() { return CUI::GetInstance(); }
#define UI_INST GetUIInstance()

//=============================================================================
// Telemetry stub
//=============================================================================
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

//=============================================================================
// Shutdown Manager
//=============================================================================
namespace ShutdownManager {
    enum Thread { eServerThread, ePostProcessThread, eRunUpdateThread };
    inline void HasStarted(Thread) {}
    inline void HasStarted(Thread, void*) {}
    inline void HasFinished(Thread) {}
    inline void HasFinished(Thread, void*) {}
    inline bool ShouldRun(Thread) { return true; }
    inline bool ShouldRun(Thread, void*) { return true; }
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
#define IDS_PROGRESS_AUTOSAVING_LEVEL 1005
#define IDS_PROGRESS_SAVING_LEVEL 1006
#define IDS_PROGRESS_SAVING_TO_DISC 1007
#define IDS_PROGRESS_SAVING_PLAYERS 1008
#define IDS_PLAYER_LEFT_END 1009
#define IDS_PLAYER_ENTERED_END 1010
// Bed/spawn/animal message IDs (from ServerPlayer::displayClientMessage)
#define IDS_TILE_BED_OCCUPIED 2001
#define IDS_TILE_BED_NO_SLEEP 2002
#define IDS_TILE_BED_NOT_VALID 2003
#define IDS_TILE_BED_NOTSAFE 2004
#define IDS_TILE_BED_PLAYERSLEEP 2005
#define IDS_TILE_BED_MESLEEP 2006
#define IDS_MAX_PIGS_SHEEP_COWS_CATS_SPAWNED 2007
#define IDS_MAX_CHICKENS_SPAWNED 2008
#define IDS_MAX_SQUID_SPAWNED 2009
#define IDS_MAX_BATS_SPAWNED 2010
#define IDS_MAX_WOLVES_SPAWNED 2011
#define IDS_MAX_MOOSHROOMS_SPAWNED 2012
#define IDS_MAX_ENEMIES_SPAWNED 2013
#define IDS_MAX_VILLAGERS_SPAWNED 2014
#define IDS_MAX_PIGS_SHEEP_COWS_CATS_BRED 2015
#define IDS_MAX_CHICKENS_BRED 2016
#define IDS_MAX_MUSHROOMCOWS_BRED 2017
#define IDS_MAX_WOLVES_BRED 2018
#define IDS_CANT_SHEAR_MOOSHROOM 2019
#define IDS_MAX_HANGINGENTITIES 2020
#define IDS_CANT_SPAWN_IN_PEACEFUL 2021
#define IDS_MAX_BOATS 2022
// Entity name IDs
#define IDS_ENDERDRAGON 3001

//=============================================================================
// INetworkPlayer - full definition needed since server code calls methods through this pointer
//=============================================================================
#include "../Common/Network/NetworkPlayerInterface.h"

//=============================================================================
// Forward declarations for Network types
// Note: These are defined in Minecraft.Client headers - do NOT define stubs here
//=============================================================================
class CPlatformNetworkManager;
class CPlatformNetworkManagerStub;
struct FriendSessionInfo;
class ClientConnection;

// Full stub for CGameNetworkManager (dedicated server - no real networking needed)
class CGameNetworkManager {
public:
    CGameNetworkManager() : m_bInitialised(false), m_bLastDisconnectWasLostRoomOnly(false), m_bFullSessionMessageOnNextSessionChange(false) {}

    void Initialise() {}
    void Terminate() {}
    void DoWork() {}
    bool IsInSession() { return true; }
    bool IsLocalGame() { return true; }
    bool IsHost() { return true; }
    bool IsInGameplay() { return true; }
    bool IsLeavingGame() { return false; }
    bool IsReadyToPlayOrIdle() { return true; }
    bool IsPrivateGame() { return false; }
    bool IsInStatsEnabledSession() { return false; }
    bool IsNetworkThreadRunning() { return false; }
    bool ShouldMessageForFullSession() { return false; }
    bool SessionHasSpace(unsigned int spaceRequired = 1) { return true; }
    int GetPlayerCount() { return 0; }
    int GetOnlinePlayerCount() { return 0; }
    static int GetLocalPlayerMask(int) { return 0; }
    bool AddLocalPlayerByUserIndex(int) { return true; }
    bool RemoveLocalPlayerByUserIndex(int) { return true; }
    INetworkPlayer* GetLocalPlayerByUserIndex(int) { return nullptr; }
    INetworkPlayer* GetPlayerByIndex(int) { return nullptr; }
    INetworkPlayer* GetPlayerBySmallId(unsigned char) { return nullptr; }
    INetworkPlayer* GetPlayerByXuid(PlayerUID) { return nullptr; }
    INetworkPlayer* GetHostPlayer() { return nullptr; }
    wstring GetDisplayNameByGamertag(wstring g) { return g; }
    void RegisterPlayerChangedCallback(int, void(*)(void*, INetworkPlayer*, bool), void*) {}
    void UnRegisterPlayerChangedCallback(int, void(*)(void*, INetworkPlayer*, bool), void*) {}
    void HandleSignInChange() {}
    void ServerReady() {}
    void ServerReadyCreate(bool) {}
    void ServerReadyWait() {}
    void ServerReadyDestroy() {}
    bool ServerReadyValid() { return false; }
    void ServerStopped() {}
    void ServerStoppedCreate(bool) {}
    void ServerStoppedWait() {}
    void ServerStoppedDestroy() {}
    bool ServerStoppedValid() { return false; }
    void SystemFlagSet(INetworkPlayer*, int) {}
    bool SystemFlagGet(INetworkPlayer*, int) { return false; }
    void UpdateAndSetGameSessionData(INetworkPlayer* = nullptr) {}
    void HostGame(int, bool, bool, unsigned char = 8, unsigned char = 0) {}
    bool LeaveGame(bool) { return true; }
    bool SetLocalGame(bool) { return true; }
    void SetPrivateGame(bool) {}
    void ResetLeavingGame() {}
    bool JoinGameFromInviteInfo(int, int, const INVITE_INFO*) { return false; }
    void SendInviteGUI(int) {}
    void HandleDisconnect(bool) {}
    int GetJoiningReadyPercentage() { return 100; }
    int CorrectErrorIDS(int ids) { return ids; }
    bool _RunNetworkGame(LPVOID) { return true; }
    bool StartNetworkGame(void*, LPVOID) { return true; }
    wstring GatherStats() { return L""; }
    wstring GatherRTTStats() { return L""; }
    void renderQueueMeter() {}

private:
    bool m_bInitialised;
    bool m_bLastDisconnectWasLostRoomOnly;
    bool m_bFullSessionMessageOnNextSessionChange;
};

// External reference to global network manager (defined in DedicatedServer/stdafx.cpp)
extern CGameNetworkManager g_NetworkManager;

//=============================================================================
// Socket class is defined in Minecraft.World/Socket.h - don't stub it here
//=============================================================================

//=============================================================================
// Color constants (eMinecraftColour type is now from App_enums.h)
//=============================================================================
// eMinecraftColour is an enum defined in App_enums.h

//=============================================================================
// Forward declarations needed by Minecraft.Client headers
//=============================================================================
class LevelGenerationOptions;
class Connection;
class Explosion;
class Container;

//=============================================================================
// Constants needed by MinecraftServer.h
//=============================================================================
const unsigned int LEVEL_LEGACY_WIDTH = 864;
const unsigned char HELL_LEVEL_LEGACY_SCALE = 3;

//=============================================================================
// ESavePlatform is defined in Minecraft.World/FileHeader.h - forward declare only
//=============================================================================
enum ESavePlatform;

//=============================================================================
// Include Minecraft.World headers in correct order
// These provide game types (Vec3, AABB, Entity, Level, Random, etc.)
//=============================================================================
#include "../../Minecraft.World/File.h"
#include "../../Minecraft.World/Definitions.h"
#include "../../Minecraft.World/ArrayWithLength.h"
#include "../../Minecraft.World/SharedConstants.h"
#include "../../Minecraft.World/Random.h"
#include "../../Minecraft.World/Pos.h"
#include "../../Minecraft.World/compression.h"
#include "../../Minecraft.World/PerformanceTimer.h"
#include "../../Minecraft.World/C4JThread.h"

// Include extraX64.h to provide XLockFreeStack and other x64 platform types
// needed by World headers (SparseLightStorage.h, CompressedTileStorage.h, etc.)
// The _EXTRA_X64_TYPES_DEFINED and _PLAYER_UID_DEFINED guards prevent redefinitions.
#include "../../Minecraft.World/x64headers/extraX64.h"

// End of stdafx_server.h
