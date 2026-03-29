#pragma once

#include "Mob.h"

#ifdef __PSVITA__
#include "..\Minecraft.Client\PSVita\PSVitaExtras\CustomMap.h"
#endif

class Player;
class Level;

class MobSpawner
{
private:
	static const int MIN_SPAWN_DISTANCE;

protected:
	static TilePos getRandomPosWithin(Level *level, int cx, int cz);

private:
#ifdef __PSVITA__
	// AP - See CustomMap.h for an explanation of this
	static CustomMap chunksToPoll;
#else
	static unordered_map<ChunkPos,bool,ChunkPosKeyHash,ChunkPosKeyEq> chunksToPoll;
#endif

public:
	static const int tick(ServerLevel *level, bool spawnEnemies, bool spawnFriendlies, bool spawnPersistent);
	static bool isSpawnPositionOk(MobCategory *category, Level *level, int x, int y, int z);

	// Diagnostic counters reset each MOBCOUNT interval
	static int dbg_creatureChunks;    // chunks attempted for creature category
	static int dbg_scanNoSurface;     // surface scan found no valid y
	static int dbg_solidFail;         // isSolidBlockingTile check failed (start pos solid)
	static int dbg_materialFail;      // getMaterial != air check failed
	static int dbg_spawnOkCalls;      // isSpawnPositionOk calls for creature
	static int dbg_spawnOkPass;       // isSpawnPositionOk returned true
	static int dbg_playerTooClose;    // getNearestPlayer blocked spawn
	static int dbg_mobTypeNull;       // getRandomMobSpawnAt returned NULL

public:
	static bool attackSleepingPlayers(Level *level, vector<shared_ptr<Player> > *players);

	static void postProcessSpawnMobs(Level *level, Biome *biome, int xo, int zo, int cellWidth, int cellHeight, Random *random);
};