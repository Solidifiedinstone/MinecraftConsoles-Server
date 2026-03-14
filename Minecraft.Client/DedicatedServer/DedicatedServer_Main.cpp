// DedicatedServer_Main.cpp
// Entry point for MinecraftConsoles Dedicated Server
// Runs the server headlessly without any graphics or GUI dependencies

#ifdef _DEDICATED_SERVER
#include "stdafx_server.h"
#else
#include "stdafx.h"
#endif

#include "DedicatedServerApp.h"
#include "ServerProperties.h"
#include "HeadlessProgressRenderer.h"

#include "../MinecraftServer.h"
#include "../../Minecraft.World/System.h"
#include "../../Minecraft.World/ThreadName.h"
#include "../../Minecraft.World/AABB.h"
#include "../../Minecraft.World/Vec3.h"
#include "../../Minecraft.World/IntCache.h"
#include "../../Minecraft.World/compression.h"
#include "../../Minecraft.World/TilePos.h"
#include "../../Minecraft.World/FireworksRecipe.h"
#include "../../Minecraft.World/OldChunkStorage.h"
#include "../../Minecraft.World/net.minecraft.world.level.tile.h"
#include "../../Minecraft.World/Entity.h"
#include "../../Minecraft.World/Minecraft.World.h"

#include <csignal>
#include <iostream>
#include <ctime>

// Global dedicated server instance
static DedicatedServerApp* g_pDedicatedServer = nullptr;
static volatile bool g_bRunning = true;

// Signal handler for clean shutdown
void SignalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		std::wcout << L"\n[Server] Received shutdown signal, stopping server..." << std::endl;
		g_bRunning = false;
		if (g_pDedicatedServer)
		{
			g_pDedicatedServer->RequestShutdown();
		}
	}
}

// Log with timestamp
void LogWithTimestamp(const wchar_t* level, const wchar_t* message)
{
	time_t now = time(nullptr);
	struct tm* timeinfo = localtime(&now);
	wchar_t timestr[32];
	wcsftime(timestr, sizeof(timestr) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", timeinfo);
	std::wcout << L"[" << timestr << L"] [" << level << L"] " << message << std::endl;
}

void PrintUsage(const wchar_t* programName)
{
	std::wcout << L"MinecraftConsoles Dedicated Server" << std::endl;
	std::wcout << L"Usage: " << programName << L" [options]" << std::endl;
	std::wcout << std::endl;
	std::wcout << L"Options:" << std::endl;
	std::wcout << L"  --world <name>      World/level name (default: world)" << std::endl;
	std::wcout << L"  --port <port>       Server port (default: 25565)" << std::endl;
	std::wcout << L"  --max-players <n>   Maximum players (default: 8)" << std::endl;
	std::wcout << L"  --gamemode <mode>   Game mode: survival, creative (default: survival)" << std::endl;
	std::wcout << L"  --difficulty <d>    Difficulty: peaceful, easy, normal, hard (default: normal)" << std::endl;
	std::wcout << L"  --seed <seed>       World seed (default: random)" << std::endl;
	std::wcout << L"  --properties <file> Path to server.properties file" << std::endl;
	std::wcout << L"  --help              Show this help message" << std::endl;
	std::wcout << std::endl;
	std::wcout << L"Configuration can also be set in server.properties file." << std::endl;
}

int wmain(int argc, wchar_t* argv[])
{
	// Setup console output
	std::wcout << L"============================================" << std::endl;
	std::wcout << L"  MinecraftConsoles Dedicated Server" << std::endl;
	std::wcout << L"  Version: 1.0.0" << std::endl;
	std::wcout << L"============================================" << std::endl;
	std::wcout << std::endl;

	// Parse command line arguments
	wstring worldName = L"world";
	int port = 25565;
	int maxPlayers = 8;
	int gamemode = 0; // 0 = survival, 1 = creative
	int difficulty = 2; // 0 = peaceful, 1 = easy, 2 = normal, 3 = hard
	__int64 seed = 0;
	bool seedSpecified = false;
	wstring propertiesFile = L"server.properties";

	for (int i = 1; i < argc; i++)
	{
		wstring arg = argv[i];

		if (arg == L"--help" || arg == L"-h")
		{
			PrintUsage(argv[0]);
			return 0;
		}
		else if (arg == L"--world" && i + 1 < argc)
		{
			worldName = argv[++i];
		}
		else if (arg == L"--port" && i + 1 < argc)
		{
			port = _wtoi(argv[++i]);
		}
		else if (arg == L"--max-players" && i + 1 < argc)
		{
			maxPlayers = _wtoi(argv[++i]);
		}
		else if (arg == L"--gamemode" && i + 1 < argc)
		{
			wstring mode = argv[++i];
			if (mode == L"creative" || mode == L"1")
				gamemode = 1;
			else
				gamemode = 0;
		}
		else if (arg == L"--difficulty" && i + 1 < argc)
		{
			wstring diff = argv[++i];
			if (diff == L"peaceful" || diff == L"0")
				difficulty = 0;
			else if (diff == L"easy" || diff == L"1")
				difficulty = 1;
			else if (diff == L"normal" || diff == L"2")
				difficulty = 2;
			else if (diff == L"hard" || diff == L"3")
				difficulty = 3;
		}
		else if (arg == L"--seed" && i + 1 < argc)
		{
			seed = _wtoi64(argv[++i]);
			seedSpecified = true;
		}
		else if (arg == L"--properties" && i + 1 < argc)
		{
			propertiesFile = argv[++i];
		}
	}

	// Setup signal handlers
	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);

	// Load server.properties file (command line args override properties file)
	LogWithTimestamp(L"INFO", L"Loading server properties...");
	ServerProperties properties(propertiesFile);

	// Apply properties (command line overrides file)
	if (worldName == L"world")
		worldName = properties.getString(L"level-name", L"world");
	if (port == 25565)
		port = properties.getInt(L"server-port", 25565);
	if (maxPlayers == 8)
		maxPlayers = properties.getInt(L"max-players", 8);
	if (gamemode == 0)
		gamemode = properties.getInt(L"gamemode", 0);
	if (difficulty == 2)
		difficulty = properties.getInt(L"difficulty", 2);
	if (!seedSpecified)
	{
		wstring seedStr = properties.getString(L"level-seed", L"");
		if (!seedStr.empty())
			seed = _wtoi64(seedStr.c_str());
	}

	// Get MOTD from properties
	wstring motd = properties.getString(L"motd", L"A MinecraftConsoles Server");

	// Log configuration
	wchar_t logBuffer[256];
	swprintf(logBuffer, 256, L"MOTD: %ls", motd.c_str());
	LogWithTimestamp(L"INFO", logBuffer);
	swprintf(logBuffer, 256, L"World name: %ls", worldName.c_str());
	LogWithTimestamp(L"INFO", logBuffer);
	swprintf(logBuffer, 256, L"Server port: %d", port);
	LogWithTimestamp(L"INFO", logBuffer);
	swprintf(logBuffer, 256, L"Max players: %d", maxPlayers);
	LogWithTimestamp(L"INFO", logBuffer);
	swprintf(logBuffer, 256, L"Game mode: %ls", (gamemode == 1) ? L"Creative" : L"Survival");
	LogWithTimestamp(L"INFO", logBuffer);

	const wchar_t* diffNames[] = { L"Peaceful", L"Easy", L"Normal", L"Hard" };
	swprintf(logBuffer, 256, L"Difficulty: %ls", diffNames[difficulty]);
	LogWithTimestamp(L"INFO", logBuffer);

	// Initialize thread storage
	SetThreadName(-1, "Dedicated Server Main");
	AABB::CreateNewThreadStorage();
	Vec3::CreateNewThreadStorage();
	IntCache::CreateNewThreadStorage();
	Compression::CreateNewThreadStorage();
	Entity::useSmallIds();
	// Note: Level::enableLightingCache() not needed for headless server
	Tile::CreateNewThreadStorage();
	FireworksRecipe::CreateNewThreadStorage();
	OldChunkStorage::CreateNewThreadStorage();

	// Initialize all World lib static singletons (LevelType, Tile, Item, Biome, etc.)
	MinecraftWorld_RunStaticCtors();

	// Create and run dedicated server
	LogWithTimestamp(L"INFO", L"Starting dedicated server...");

	g_pDedicatedServer = new DedicatedServerApp();

	DedicatedServerConfig config;
	config.worldName = worldName;
	config.port = port;
	config.maxPlayers = maxPlayers;
	config.gamemode = gamemode;
	config.difficulty = difficulty;
	config.seed = seed;
	config.seedSpecified = seedSpecified;
	config.pvp = properties.getBoolean(L"pvp", true);
	config.spawnAnimals = properties.getBoolean(L"spawn-animals", true);
	config.spawnMonsters = properties.getBoolean(L"spawn-monsters", true);
	config.spawnNpcs = properties.getBoolean(L"spawn-npcs", true);
	config.generateStructures = properties.getBoolean(L"generate-structures", true);
	config.levelType = properties.getString(L"level-type", L"default");

	int result = g_pDedicatedServer->Run(config);

	// Cleanup
	delete g_pDedicatedServer;
	g_pDedicatedServer = nullptr;

	// Release thread storage
	Tile::ReleaseThreadStorage();
	AABB::ReleaseThreadStorage();
	Vec3::ReleaseThreadStorage();
	IntCache::ReleaseThreadStorage();

	LogWithTimestamp(L"INFO", L"Server stopped.");
	return result;
}

// Windows entry point wrapper
#ifdef _WINDOWS64
int main(int argc, char* argv[])
{
	// Convert char** to wchar_t** for wmain
	wchar_t** wargv = new wchar_t*[argc];
	for (int i = 0; i < argc; i++)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, nullptr, 0);
		wargv[i] = new wchar_t[len];
		MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, wargv[i], len);
	}

	int result = wmain(argc, wargv);

	for (int i = 0; i < argc; i++)
	{
		delete[] wargv[i];
	}
	delete[] wargv;

	return result;
}
#endif
