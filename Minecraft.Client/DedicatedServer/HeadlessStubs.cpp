// HeadlessStubs.cpp
// Minimal stubs for unavoidable graphics/UI references in server code
// These provide no-op implementations to satisfy linker requirements

#ifdef _DEDICATED_SERVER
#include "stdafx_server.h"
#else
#include "stdafx.h"
#endif

#ifdef _DEDICATED_SERVER

#include "DedicatedServerApp.h"
#include "HeadlessProgressRenderer.h"

// Forward declarations for stub types
class GameRenderer;
class LevelRenderer;
class Tesselator;
class ParticleEngine;
class ItemInHandRenderer;
class Gui;
class Font;
class Textures;
class TexturePackRepository;
class TexturePack;
class Screen;
class Options;
class User;
class MultiPlayerLevel;
class MultiPlayerGameMode;
class MultiplayerLocalPlayer;

// Stub Minecraft class for dedicated server
// This provides minimal compatibility with code that references Minecraft::GetInstance()
class MinecraftStub
{
public:
	static MinecraftStub* s_pInstance;

	HeadlessProgressRenderer* progressRenderer;
	GameRenderer* gameRenderer;
	MultiPlayerLevel* level;
	User* user;
	Options* options;

	// Stubs for local player tracking
	shared_ptr<MultiplayerLocalPlayer> localplayers[4];
	void* m_pendingLocalConnections[4];
	bool m_connectionFailed[4];
	int m_connectionFailedReason[4];

	Gui* gui;
	TexturePackRepository* skins;

	MinecraftStub()
		: progressRenderer(nullptr)
		, gameRenderer(nullptr)
		, level(nullptr)
		, user(nullptr)
		, options(nullptr)
		, gui(nullptr)
		, skins(nullptr)
	{
		memset(m_pendingLocalConnections, 0, sizeof(m_pendingLocalConnections));
		memset(m_connectionFailed, 0, sizeof(m_connectionFailed));
		memset(m_connectionFailedReason, 0, sizeof(m_connectionFailedReason));
		s_pInstance = this;

		// Create headless progress renderer
		progressRenderer = new HeadlessProgressRenderer();
	}

	~MinecraftStub()
	{
		if (progressRenderer)
		{
			delete progressRenderer;
			progressRenderer = nullptr;
		}
		s_pInstance = nullptr;
	}

	static MinecraftStub* GetInstance() { return s_pInstance; }

	bool isTutorial() { return false; }
	void clearConnectionFailed() {}
	void connectionDisconnected(int, int) {}
	void addPendingLocalConnection(int, void*) {}
	void removeLocalPlayerIdx(int) {}
	void clearPendingClientTextureRequests() {}
	void forceStatsSave(int) {}
};

MinecraftStub* MinecraftStub::s_pInstance = nullptr;

// Global stub instance
static MinecraftStub g_minecraftStub;

// Provide Minecraft::GetInstance() compatibility
// Note: In actual compilation, this is handled via preprocessor redirects

// Stub for Gui class
class GuiStub
{
public:
	void clearMessages() {}
	void renderGraph(int, int, __int64*, int, int, __int64*, int, int) {}
};

// Stub for User class
class UserStub
{
public:
	wstring name;
	UserStub() : name(L"Server") {}
};

// Stub for Options class
class OptionsStub
{
public:
	int difficulty;
	OptionsStub() : difficulty(2) {}
};

// Stub for TexturePackRepository
class TexturePackRepositoryStub
{
public:
	bool needsUIUpdate() { return false; }
	void* getSelected() { return nullptr; }
};

// Stub for TexturePack
class TexturePackStub
{
public:
	bool isLoadingData() { return false; }
	int getDLCParentPackId() { return 0; }
	int getDLCSubPackId() { return 0; }
};

// Global stub instances that server code might reference
static GuiStub g_guiStub;
static UserStub g_userStub;
static OptionsStub g_optionsStub;
static TexturePackRepositoryStub g_skinsStub;

// PIX event stubs (for performance instrumentation that won't work headless)
// Note: PIXBeginNamedEvent and PIXEndNamedEvent are defined in stdafx_server.h
void PIXReportCounter(const wchar_t*, float) {}

// Storage manager stub
class StorageManagerStub
{
public:
	bool GetSaveDisabled() { return false; }
	int GetSaveState() { return 0; } // ESaveGame_Idle
};

static StorageManagerStub g_storageManagerStub;

// Profile manager stub
class ProfileManagerStub
{
public:
	bool IsSignedIn(int) { return false; }
	bool IsSignedInLive(int) { return false; }
	bool IsFullVersion() { return true; }
	int GetPrimaryPad() { return 0; }
	int GetLockedProfile() { return 0; }
	void SetPrimaryPad(int) {}
	void SetLockedProfile(int) {}
	char* GetGamertag(int) { return (char*)"Server"; }
	bool AllowedToPlayMultiplayer(int) { return true; }
	void QuerySigninStatus() {}
	void SetCurrentGameActivity(int, int, bool) {}
	void Tick() {}
	void* GetUser(int, bool) { return nullptr; }
	bool getNPAvailability(int) { return 0; }
	bool HasPlayStationPlus(int) { return false; }
	void GetChatAndContentRestrictions(int, bool, bool*, bool*, void*) {}
	void AllowedPlayerCreatedContent(int, bool, BOOL*, BOOL*) {}
	void DisplaySystemMessage(int, int) {}
};

static ProfileManagerStub g_profileManagerStub;

// Render manager stub
class RenderManagerStub
{
public:
	void StartFrame() {}
	void Present() {}
	void Tick() {}
	bool IsHiDef() { return true; }
};

static RenderManagerStub g_renderManagerStub;

// Input manager stub
class InputManagerStub
{
public:
	void Tick() {}
	bool IsPadLocked(int) { return false; }
};

static InputManagerStub g_inputManagerStub;

// UI stub
class UIStub
{
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
};

static UIStub g_uiStub;

// XEnable functions stub
void XEnableGuestSignin(BOOL) {}

// Shutdown manager stubs
namespace ShutdownManager
{
	enum Thread { eServerThread, ePostProcessThread };
	void HasStarted(Thread) {}
	void HasFinished(Thread) {}
	bool ShouldRun(Thread) { return true; }
}

// Telemetry manager stub
class TelemetryManagerStub
{
public:
	void RecordPlayerSessionStart(int) {}
	void RecordPlayerSessionExit(int, int) {}
	void RecordLevelStart(int, int, int, int, int, int) {}
	int GenerateMultiplayerInstanceId() { return 0; }
	void SetMultiplayerInstanceId(int) {}
};

static TelemetryManagerStub g_telemetryManagerStub;
TelemetryManagerStub* TelemetryManager = &g_telemetryManagerStub;

#endif // _DEDICATED_SERVER
