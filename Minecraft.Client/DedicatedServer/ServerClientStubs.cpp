// ServerClientStubs.cpp
// Stub implementations for client-side source files removed from the dedicated server build.
// These provide the minimum symbols needed for the server to link successfully.
// stdafx_server.h is force-included before this file.

#include "stdafx.h"

// ============================================================================
// Forward declarations needed before including class headers
// ============================================================================
class DLCPack;

// ============================================================================
// GameRenderer stubs
// Needed because LevelChunk.cpp (World lib) calls AddForDelete/FinishedReassigning
// ============================================================================
#include "../GameRenderer.h"

// Static member definitions
vector<byte *> GameRenderer::m_deleteStackByte;
vector<SparseLightStorage *> GameRenderer::m_deleteStackSparseLightStorage;
vector<CompressedTileStorage *> GameRenderer::m_deleteStackCompressedTileStorage;
vector<SparseDataStorage *> GameRenderer::m_deleteStackSparseDataStorage;
CRITICAL_SECTION GameRenderer::m_csDeleteStack;
bool GameRenderer::anaglyph3d = false;
int  GameRenderer::anaglyphPass = 0;
ResourceLocation GameRenderer::RAIN_LOCATION;
ResourceLocation GameRenderer::SNOW_LOCATION;

void GameRenderer::AddForDelete(byte *deleteThis)
{
    delete[] deleteThis;
}

void GameRenderer::AddForDelete(SparseLightStorage *deleteThis)
{
    delete deleteThis;
}

void GameRenderer::AddForDelete(CompressedTileStorage *deleteThis)
{
    delete deleteThis;
}

void GameRenderer::AddForDelete(SparseDataStorage *deleteThis)
{
    delete deleteThis;
}

void GameRenderer::FinishedReassigning()
{
    // No-op on dedicated server
}

// ============================================================================
// LevelRenderer stubs
// Needed because ServerPlayer.cpp calls getGlobalIndexForChunk
// ============================================================================
#include "../LevelRenderer.h"

// Static member definitions
const int LevelRenderer::MAX_LEVEL_RENDER_SIZE[3] = { 80, 44, 44 };
const int LevelRenderer::DIMENSION_OFFSETS[3] = {
    0,
    (80 * 80 * (Level::maxBuildHeight / 16)),
    (80 * 80 * (Level::maxBuildHeight / 16)) + (44 * 44 * (Level::maxBuildHeight / 16))
};

int LevelRenderer::getDimensionIndexFromId(int id)
{
    return (3 - id) % 3;
}

int LevelRenderer::getGlobalIndexForChunk(int x, int y, int z, Level *level)
{
    return getGlobalIndexForChunk(x, y, z, level->dimension->id);
}

int LevelRenderer::getGlobalIndexForChunk(int x, int y, int z, int dimensionId)
{
    int dimIdx = getDimensionIndexFromId(dimensionId);
    int xx = (x / CHUNK_XZSIZE) + (MAX_LEVEL_RENDER_SIZE[dimIdx] / 2);
    int yy = y / CHUNK_SIZE;
    int zz = (z / CHUNK_XZSIZE) + (MAX_LEVEL_RENDER_SIZE[dimIdx] / 2);

    if ((xx < 0) || (xx >= MAX_LEVEL_RENDER_SIZE[dimIdx])) return -1;
    if ((zz < 0) || (zz >= MAX_LEVEL_RENDER_SIZE[dimIdx])) return -1;
    if ((yy < 0) || (yy >= CHUNK_Y_COUNT)) return -1;

    int dimOffset = DIMENSION_OFFSETS[dimIdx];
    int offset = dimOffset;
    offset += (zz * MAX_LEVEL_RENDER_SIZE[dimIdx] + xx) * CHUNK_Y_COUNT;
    offset += yy;
    return offset;
}

// ============================================================================
// StatsCounter stubs
// Needed because StatsCounter.cpp has uncompilable profile system dependencies
// ============================================================================
#include "../StatsCounter.h"
#include "../../Minecraft.World/Stat.h"
#include "../../Minecraft.World/Achievement.h"

// Static member definitions
Stat** StatsCounter::LARGE_STATS[StatsCounter::LARGE_STATS_COUNT] = {};
unordered_map<Stat*, int> StatsCounter::statBoards;

StatsCounter::StatsCounter()
{
    requiresSave = false;
    saveCounter = 0;
    modifiedBoards = 0;
    flushCounter = 0;
}

void StatsCounter::award(Stat*, unsigned int, unsigned int) {}
bool StatsCounter::hasTaken(Achievement*) { return true; }
bool StatsCounter::canTake(Achievement*) { return true; }
unsigned int StatsCounter::getValue(Stat*, unsigned int) { return 0; }
unsigned int StatsCounter::getTotalValue(Stat*) { return 0; }
void StatsCounter::tick(int) {}
void StatsCounter::parse(void*) {}
void StatsCounter::clear() {}
void StatsCounter::save(int, bool) {}
void StatsCounter::flushLeaderboards() {}
void StatsCounter::saveLeaderboards() {}
void StatsCounter::setupStatBoards() {}
void StatsCounter::WipeLeaderboards() {}

// ============================================================================
// LevelGenerationOptions stubs
// Needed because MinecraftServer.cpp calls methods on getLevelGenerationOptions()
// (which returns NULL in dedicated server mode, but linker still needs the symbols)
// ============================================================================
#include "../Common/GameRules/LevelGenerationOptions.h"

LevelGenerationOptions::LevelGenerationOptions(DLCPack*)
{
    m_src = eSrc_none;
    m_pSrc = nullptr;
    m_hasLoadedData = true;
    m_pbBaseSaveData = nullptr;
    m_dwBaseSaveSize = 0;
    m_seed = 0;
    m_useFlatWorld = false;
    m_spawnPos = nullptr;
    m_bHasBeenInCreative = 0;
    m_bHaveMinY = false;
    m_minY = 0;
    m_bRequiresGameRules = false;
    m_requiredGameRules = nullptr;
    m_stringTable = nullptr;
    m_parentDLCPack = nullptr;
    m_bLoadingData = false;
}

LevelGenerationOptions::~LevelGenerationOptions()
{
    delete m_spawnPos;
    delete[] m_pbBaseSaveData;
}

ConsoleGameRules::EGameRuleType LevelGenerationOptions::getActionType()
{
    return ConsoleGameRules::eGameRuleType_LevelGenerationOptions;
}

bool LevelGenerationOptions::hasLoadedData() { return m_hasLoadedData; }
void LevelGenerationOptions::setLoadedData() { m_hasLoadedData = true; }
bool LevelGenerationOptions::ready() { return true; }
bool LevelGenerationOptions::getuseFlatWorld() { return m_useFlatWorld; }
bool LevelGenerationOptions::requiresBaseSave() { return false; }
PBYTE LevelGenerationOptions::getBaseSaveData(DWORD &size) { size = 0; return nullptr; }
bool LevelGenerationOptions::hasBaseSaveData() { return false; }
void LevelGenerationOptions::deleteBaseSaveData() {}
Pos* LevelGenerationOptions::getSpawnPos() { return m_spawnPos; }
bool LevelGenerationOptions::isFromDLC() { return m_src == eSrc_fromDLC; }
bool LevelGenerationOptions::isFromSave() { return m_src == eSrc_fromSave; }
bool LevelGenerationOptions::isTutorial() { return false; }
int LevelGenerationOptions::getLevelHasBeenInCreative() { return m_bHasBeenInCreative; }
__int64 LevelGenerationOptions::getLevelSeed() { return m_seed; }

void LevelGenerationOptions::setSrc(eSrc src) { m_src = src; }
LevelGenerationOptions::eSrc LevelGenerationOptions::getSrc() { return m_src; }

bool LevelGenerationOptions::requiresTexturePack() { return false; }
UINT LevelGenerationOptions::getRequiredTexturePackId() { return 0; }
wstring LevelGenerationOptions::getDefaultSaveName() { return L""; }
LPCWSTR LevelGenerationOptions::getWorldName() { return L""; }
LPCWSTR LevelGenerationOptions::getDisplayName() { return L""; }
wstring LevelGenerationOptions::getGrfPath() { return L""; }
bool LevelGenerationOptions::requiresGameRules() { return false; }
void LevelGenerationOptions::setRequiredGameRules(LevelRuleset*) {}
LevelRuleset* LevelGenerationOptions::getRequiredGameRules() { return nullptr; }

void LevelGenerationOptions::setGrSource(GrSource*) {}
void LevelGenerationOptions::setRequiresTexturePack(bool) {}
void LevelGenerationOptions::setRequiredTexturePackId(UINT) {}
void LevelGenerationOptions::setDefaultSaveName(const wstring&) {}
void LevelGenerationOptions::setWorldName(const wstring&) {}
void LevelGenerationOptions::setDisplayName(const wstring&) {}
void LevelGenerationOptions::setGrfPath(const wstring&) {}
void LevelGenerationOptions::setBaseSavePath(const wstring&) {}
void LevelGenerationOptions::setBaseSaveData(PBYTE, DWORD) {}
wstring LevelGenerationOptions::getBaseSavePath() { return L""; }

void LevelGenerationOptions::writeAttributes(DataOutputStream*, UINT) {}
void LevelGenerationOptions::getChildren(vector<GameRuleDefinition*>*) {}
GameRuleDefinition* LevelGenerationOptions::addChild(ConsoleGameRules::EGameRuleType) { return nullptr; }
void LevelGenerationOptions::addAttribute(const wstring&, const wstring&) {}

void LevelGenerationOptions::processSchematics(LevelChunk*) {}
void LevelGenerationOptions::processSchematicsLighting(LevelChunk*) {}
bool LevelGenerationOptions::checkIntersects(int, int, int, int, int, int) { return false; }
ConsoleSchematicFile* LevelGenerationOptions::loadSchematicFile(const wstring&, PBYTE, DWORD) { return nullptr; }
ConsoleSchematicFile* LevelGenerationOptions::getSchematicFile(const wstring&) { return nullptr; }
void LevelGenerationOptions::releaseSchematicFile(const wstring&) {}
unordered_map<wstring, ConsoleSchematicFile*>* LevelGenerationOptions::getUnfinishedSchematicFiles() { return nullptr; }

void LevelGenerationOptions::getBiomeOverride(int, BYTE&, BYTE&) {}
bool LevelGenerationOptions::isFeatureChunk(int, int, StructureFeature::EFeatureTypes, int*) { return false; }

void LevelGenerationOptions::loadStringTable(StringTable*) {}
LPCWSTR LevelGenerationOptions::getString(const wstring &key) { return key.c_str(); }

void LevelGenerationOptions::loadBaseSaveData() {}
int LevelGenerationOptions::packMounted(LPVOID, int, DWORD, DWORD) { return 0; }
void LevelGenerationOptions::reset_start() {}
void LevelGenerationOptions::reset_finish() {}
void LevelGenerationOptions::clearSchematics() {}
GrSource* LevelGenerationOptions::info() { return nullptr; }

// GameRuleDefinition base class stubs (needed for vtable/methods not already in GameRuleDefinition.cpp)
// GameRuleDefinition.cpp is in the build so its methods are already defined.
// Only LevelGenerationOptions-specific methods are needed here.
