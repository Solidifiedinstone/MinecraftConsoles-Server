// DedicatedServerApp.cpp
// Server application implementation for dedicated server mode

#ifdef _DEDICATED_SERVER
#include "stdafx_server.h"
#else
#include "stdafx.h"
#endif

#include "DedicatedServerApp.h"
#include "HeadlessProgressRenderer.h"
#include "ServerProperties.h"

#include "../MinecraftServer.h"
#include "../Settings.h"
#include "../PlayerList.h"
#include "../ServerConnection.h"
#include "../ServerLevel.h"
#include "../../Minecraft.World/System.h"
#include "../../Minecraft.World/Socket.h"
#include "../../Minecraft.World/net.minecraft.world.level.h"

#include <iostream>
#include <cstdarg>
#include <ctime>

// Static instance
DedicatedServerApp* DedicatedServerApp::s_pInstance = nullptr;

// Note: The global 'app' (CMinecraftApp) is defined in stdafx.cpp
// DedicatedServerApp uses a separate instance pattern

// Log helper
static void ServerLog(const wchar_t* level, const wchar_t* message)
{
	time_t now = time(nullptr);
	struct tm* timeinfo = localtime(&now);
	wchar_t timestr[32];
	wcsftime(timestr, sizeof(timestr) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", timeinfo);
	std::wcout << L"[" << timestr << L"] [" << level << L"] " << message << std::endl;
}

DedicatedServerApp::DedicatedServerApp()
	: m_bRunning(false)
	, m_bShutdownRequested(false)
	, m_pProgressRenderer(nullptr)
	, m_pServer(nullptr)
	, m_hStdinThread(NULL)
{
	s_pInstance = this;

	// Initialize host options to defaults
	memset(m_hostOptions, 0, sizeof(m_hostOptions));
	m_hostOptions[eGameHostOption_Difficulty] = 2; // Normal
	m_hostOptions[eGameHostOption_PvP] = 1;
	m_hostOptions[eGameHostOption_FireSpreads] = 1;
	m_hostOptions[eGameHostOption_TNT] = 1;
	m_hostOptions[eGameHostOption_Structures] = 1;

	InitializeCriticalSection(&m_commandCS);
}

DedicatedServerApp::~DedicatedServerApp()
{
	DeleteCriticalSection(&m_commandCS);

	if (m_pProgressRenderer)
	{
		delete m_pProgressRenderer;
		m_pProgressRenderer = nullptr;
	}

	s_pInstance = nullptr;
}

int DedicatedServerApp::Run(const DedicatedServerConfig& config)
{
	m_config = config;

	if (!Initialize(config))
	{
		ServerLog(L"ERROR", L"Failed to initialize server");
		return 1;
	}

	MainLoop();
	Shutdown();

	return 0;
}

bool DedicatedServerApp::Initialize(const DedicatedServerConfig& config)
{
	ServerLog(L"INFO", L"Initializing dedicated server...");

	// Create headless progress renderer
	m_pProgressRenderer = new HeadlessProgressRenderer();

	// Set host options from config - both local and on global app
	m_hostOptions[eGameHostOption_GameType] = config.gamemode;
	m_hostOptions[eGameHostOption_Difficulty] = config.difficulty;
	m_hostOptions[eGameHostOption_PvP] = config.pvp ? 1 : 0;
	m_hostOptions[eGameHostOption_Structures] = config.generateStructures ? 1 : 0;

	// Set level type
	if (config.levelType == L"flat")
		m_hostOptions[eGameHostOption_LevelType] = 1;
	else
		m_hostOptions[eGameHostOption_LevelType] = 0;

	// Sync to global app (CMinecraftApp) so server code can access via app.GetGameHostOption()
	app.SetGameHostOption(eGameHostOption_GameType, m_hostOptions[eGameHostOption_GameType]);
	app.SetGameHostOption(eGameHostOption_Difficulty, m_hostOptions[eGameHostOption_Difficulty]);
	app.SetGameHostOption(eGameHostOption_PvP, m_hostOptions[eGameHostOption_PvP]);
	app.SetGameHostOption(eGameHostOption_FireSpreads, m_hostOptions[eGameHostOption_FireSpreads]);
	app.SetGameHostOption(eGameHostOption_TNT, m_hostOptions[eGameHostOption_TNT]);
	app.SetGameHostOption(eGameHostOption_Structures, m_hostOptions[eGameHostOption_Structures]);
	app.SetGameHostOption(eGameHostOption_BonusChest, m_hostOptions[eGameHostOption_BonusChest]);
	app.SetGameHostOption(eGameHostOption_LevelType, m_hostOptions[eGameHostOption_LevelType]);

	// Initialize socket system
	Socket::SetTCPPort(config.port);
	Socket::Initialise(nullptr);

	m_bRunning = true;

	return true;
}

void DedicatedServerApp::MainLoop()
{
	ServerLog(L"INFO", L"Starting server main loop...");

	// Prepare server initialization data
	NetworkGameInitData initData;
	initData.seed = m_config.seed;
	initData.findSeed = !m_config.seedSpecified;
	initData.settings = m_hostOptions[eGameHostOption_All];

	// Run server on main thread (dedicated mode)
	MinecraftServer::main(initData.seed, &initData);

	// The server has stopped
	m_bRunning = false;
}

void DedicatedServerApp::Shutdown()
{
	ServerLog(L"INFO", L"Shutting down server...");

	if (m_pServer)
	{
		m_pServer->halt();
		m_pServer = nullptr;
	}

	m_bRunning = false;
}

void DedicatedServerApp::RequestShutdown()
{
	m_bShutdownRequested = true;
	MinecraftServer::HaltServer();
}

DWORD DedicatedServerApp::GetGameHostOption(eGameHostOption option) const
{
	if (option >= eGameHostOption_Max)
		return 0;

	if (option == eGameHostOption_All)
	{
		// Pack all options into a single DWORD
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

	return m_hostOptions[option];
}

void DedicatedServerApp::SetGameHostOption(eGameHostOption option, DWORD value)
{
	if (option >= eGameHostOption_Max)
		return;

	if (option == eGameHostOption_All)
	{
		// Unpack all options from a single DWORD
		m_hostOptions[eGameHostOption_GameType] = value & 0x1;
		m_hostOptions[eGameHostOption_Difficulty] = (value >> 1) & 0x3;
		m_hostOptions[eGameHostOption_PvP] = (value >> 3) & 0x1;
		m_hostOptions[eGameHostOption_FireSpreads] = (value >> 4) & 0x1;
		m_hostOptions[eGameHostOption_TNT] = (value >> 5) & 0x1;
		m_hostOptions[eGameHostOption_Structures] = (value >> 6) & 0x1;
		m_hostOptions[eGameHostOption_BonusChest] = (value >> 7) & 0x1;
		m_hostOptions[eGameHostOption_LevelType] = (value >> 8) & 0x1;
	}
	else
	{
		m_hostOptions[option] = value;
	}
}

void DedicatedServerApp::DebugPrintf(const char* format, ...)
{
	// Format the message
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	// Convert to wide string and log
	wchar_t wbuffer[1024];
	mbstowcs(wbuffer, buffer, sizeof(wbuffer) / sizeof(wchar_t));

	// Only output if not just whitespace
	bool hasContent = false;
	for (char* p = buffer; *p; p++)
	{
		if (!isspace(*p))
		{
			hasContent = true;
			break;
		}
	}

	if (hasContent)
	{
		// Remove trailing newlines for cleaner output
		size_t len = wcslen(wbuffer);
		while (len > 0 && (wbuffer[len - 1] == L'\n' || wbuffer[len - 1] == L'\r'))
		{
			wbuffer[--len] = L'\0';
		}

		ServerLog(L"DEBUG", wbuffer);
	}
}

void DedicatedServerApp::ProcessConsoleInput()
{
	EnterCriticalSection(&m_commandCS);

	if (!m_pendingCommand.empty())
	{
		wstring cmd = m_pendingCommand;
		m_pendingCommand.clear();

		LeaveCriticalSection(&m_commandCS);

		// Process command
		if (cmd == L"stop" || cmd == L"quit" || cmd == L"exit")
		{
			ServerLog(L"INFO", L"Stop command received");
			RequestShutdown();
		}
		else if (cmd == L"list")
		{
			// List online players
			MinecraftServer* server = MinecraftServer::getInstance();
			if (server && server->getPlayers())
			{
				wchar_t buf[128];
				swprintf(buf, 128, L"Online players: %d", (int)server->getPlayers()->players.size());
				ServerLog(L"INFO", buf);
			}
		}
		else if (cmd == L"save" || cmd == L"save-all")
		{
			ServerLog(L"INFO", L"Saving world...");
			// Trigger save via server action mechanism
		}
		else if (cmd == L"help")
		{
			ServerLog(L"INFO", L"Available commands:");
			ServerLog(L"INFO", L"  stop    - Stop the server");
			ServerLog(L"INFO", L"  list    - List online players");
			ServerLog(L"INFO", L"  save    - Save the world");
			ServerLog(L"INFO", L"  help    - Show this help");
		}
		else
		{
			wstring msg = L"Unknown command: " + cmd;
			ServerLog(L"WARN", msg.c_str());
		}
	}
	else
	{
		LeaveCriticalSection(&m_commandCS);
	}
}
