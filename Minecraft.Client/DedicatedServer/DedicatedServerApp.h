// DedicatedServerApp.h
// Server application class for dedicated server mode
// Replaces CMinecraftApp for headless operation

#pragma once

#include <string>
#include <unordered_map>

using namespace std;

// Forward declarations
class MinecraftServer;
class HeadlessProgressRenderer;
class ServerProperties;

// Configuration structure for server startup
struct DedicatedServerConfig
{
	wstring worldName;
	int port;
	int maxPlayers;
	int gamemode;
	int difficulty;
	__int64 seed;
	bool seedSpecified;
	bool pvp;
	bool spawnAnimals;
	bool spawnMonsters;
	bool spawnNpcs;
	bool generateStructures;
	wstring levelType;

	DedicatedServerConfig()
		: worldName(L"world")
		, port(25565)
		, maxPlayers(8)
		, gamemode(0)
		, difficulty(2)
		, seed(0)
		, seedSpecified(false)
		, pvp(true)
		, spawnAnimals(true)
		, spawnMonsters(true)
		, spawnNpcs(true)
		, generateStructures(true)
		, levelType(L"default")
	{
	}
};

// Note: eGameHostOption enum is defined in stdafx_server.h

class DedicatedServerApp
{
public:
	DedicatedServerApp();
	~DedicatedServerApp();

	// Main server entry point
	int Run(const DedicatedServerConfig& config);

	// Request server shutdown
	void RequestShutdown();

	// Get/set game host options
	DWORD GetGameHostOption(eGameHostOption option) const;
	void SetGameHostOption(eGameHostOption option, DWORD value);

	// Progress renderer for headless mode
	HeadlessProgressRenderer* GetProgressRenderer() const { return m_pProgressRenderer; }

	// Debug output (console logging)
	void DebugPrintf(const char* format, ...);

	// Check if server is running
	bool IsRunning() const { return m_bRunning; }

	// Check if explicit shutdown was requested (via "stop" command or SIGTERM)
	bool IsShutdownRequested() const { return m_bShutdownRequested; }

	// Singleton access (for compatibility with code that expects 'app')
	static DedicatedServerApp* GetInstance() { return s_pInstance; }

private:
	// Initialize server systems
	bool Initialize(const DedicatedServerConfig& config);

	// Main server loop
	void MainLoop();

	// Cleanup
	void Shutdown();

	// Process console input
	void ProcessConsoleInput();

private:
	static DedicatedServerApp* s_pInstance;

	bool m_bRunning;
	bool m_bShutdownRequested;

	DedicatedServerConfig m_config;
	DWORD m_hostOptions[eGameHostOption_Max];

	HeadlessProgressRenderer* m_pProgressRenderer;
	MinecraftServer* m_pServer;

	// Console input handling
	HANDLE m_hStdinThread;
	wstring m_pendingCommand;
	CRITICAL_SECTION m_commandCS;

	// Start stdin reading thread
	void StartConsoleThread();
	static DWORD WINAPI ConsoleThreadProc(LPVOID lpParam);
};

// Note: Global 'app' (CMinecraftApp) is defined in stdafx_server.h
// Use DedicatedServerApp::GetInstance() for server-specific functionality
