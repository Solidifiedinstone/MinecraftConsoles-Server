# MinecraftConsoles Dedicated Server Implementation Notes

## Overview

This document details the work done to add dedicated server support to MinecraftConsoles Legacy Console Edition. The goal is to create a headless server executable that can host multiplayer games without requiring graphics/rendering infrastructure.

## Architecture

**Original Design**: Client runs server on background thread, requires full graphics initialization
**Target Design**: Standalone server executable (Minecraft.Server.exe), console-only, no graphics dependencies

## Repository

- **Fork**: https://github.com/Solidifiedinstone/MinecraftConsoles-Server
- **CI/CD**: GitHub Actions builds on Windows with Visual Studio 2022

---

## Files Created

### 1. `Minecraft.Client/DedicatedServer/` Directory

| File | Purpose |
|------|---------|
| `DedicatedServer_Main.cpp` | Entry point with `main(argc, argv)` |
| `DedicatedServerApp.h/cpp` | Server application class (replaces CMinecraftApp) |
| `ServerProperties.h/cpp` | Load/save `server.properties` configuration file |
| `HeadlessProgressRenderer.h/cpp` | Console output for progress (replaces GUI progress bars) |
| `HeadlessStubs.cpp` | Minimal stubs for unavoidable graphics/client references |
| `stdafx_server.h` | **CRITICAL** - Server-specific precompiled header with all stubs |
| `stdafx.h` | Redirect file that includes stdafx_server.h |

### 2. `Minecraft.Client/Minecraft.Server.vcxproj`

Visual Studio project file for building the dedicated server. Key configuration:
- Target: Console application (not Windows app)
- Preprocessor: `_DEDICATED_SERVER;_WINDOWS64;_CONSOLE`
- ForcedIncludeFiles: `stdafx_server.h`
- Links: `ws2_32.lib` for networking
- Disabled warnings: 5208 (unnamed classes), 4996 (deprecated functions)

### 3. `.github/workflows/build-server.yml`

GitHub Actions CI/CD workflow that:
- Builds Debug and Release configurations
- Creates artifacts with Minecraft.Server.exe
- Runs on Windows with VS 2022

---

## Files Modified

### Minecraft.World/stdafx.h

Added `_DEDICATED_SERVER` guards around:
- d3d11.h include (line 23-25)
- All 4JLibs includes (lines 153-246)
- Platform-specific app headers (Windows64_App.h, etc.)

### Minecraft.World/x64headers/extraX64.h

Added `_DEDICATED_SERVER` guards around:
- SkinBox.h include
- Type definitions (byte, PlayerUID, etc.) with `_EXTRA_X64_TYPES_DEFINED` guard
- XMem functions
- IsEqualXUID function

### Minecraft.Client/stdafx.h

Added redirect at top of file:
```cpp
#ifdef _DEDICATED_SERVER
#include "DedicatedServer/stdafx_server.h"
#else
// ... original client header content
#endif
```

### Minecraft.Client/Common/GameRules/GameRuleManager.h

Added `_DEDICATED_SERVER` guard to skip full implementation (stub in stdafx_server.h)

### Minecraft.Client/Common/Telemetry/TelemetryManager.h

Added `_DEDICATED_SERVER` guard to skip full implementation (stub in stdafx_server.h)

### Minecraft.Client/Common/Consoles_App.h

Added `_DEDICATED_SERVER` guard to skip CMinecraftApp (stub in stdafx_server.h)

### Minecraft.Client/Common/Audio/Consoles_SoundEngine.h

Added `_DEDICATED_SERVER` guard around Miles Sound System includes

---

## stdafx_server.h - Detailed Contents

This is the critical file that provides all platform stubs for the dedicated server:

### 1. Basic Types (defined before any includes)
```cpp
typedef unsigned char byte;
typedef ULONGLONG PlayerUID;
typedef PlayerUID GameSessionUID;
const int XUSER_MAX_COUNT = 4;
const int MINECRAFT_NET_MAX_PLAYERS = 8;
```

### 2. DirectX Stubs
- XMFLOAT2, XMFLOAT3, XMFLOAT4, XMMATRIX, XMVECTOR
- XMVectorSet, XMVectorGetX/Y/Z functions
- PIXBeginNamedEvent, PIXEndNamedEvent stubs

### 3. Compression Stubs (XMem)
```cpp
typedef VOID* XMEMCOMPRESSION_CONTEXT;
typedef VOID* XMEMDECOMPRESSION_CONTEXT;
// XMemCreateCompressionContext, XMemCompress, etc.
```

### 4. IQNet Network Stubs
```cpp
class IQNetPlayer { ... };
class IQNet { ... };
enum QNET_STATE { ... };
// Constants: QNET_SENDDATA_*, QNET_USER_MASK_*, etc.
```

### 5. Xbox-like Types
- XOVERLAPPED, XSESSION_SEARCHRESULT_HEADER, XNQOSINFO, XNQOS
- XUSER_CONTEXT, XUSER_SIGNIN_INFO, XONLINE_FRIEND
- XCONTENT_DATA, SessionID, INVITE_INFO
- Language/locale constants (XC_LANGUAGE_*, XC_LOCALE_*)
- CXuiStringTable stub class

### 6. 4JLibs Manager Stubs
```cpp
class CInputManager { static CInputManager& GetInstance(); };
#define InputManager CInputManager::GetInstance()

class CProfileManager { static CProfileManager& GetInstance(); };
#define ProfileManager CProfileManager::GetInstance()

class CRenderManager { static CRenderManager& GetInstance(); };
#define RenderManager CRenderManager::GetInstance()

class CStorageManager { static CStorageManager& GetInstance(); };
#define StorageManager CStorageManager::GetInstance()
```

### 7. C4JRender Namespace Stub
```cpp
namespace C4JRender {
    enum eTextureFormat {
        eFMT_DEFAULT, eFMT_DXT1, eFMT_DXT5, eFMT_A8R8G8B8, eFMT_R8G8B8, eFMT_A8,
        TEXTURE_FORMAT_RxGyBzAw = eFMT_A8R8G8B8
    };
    typedef eTextureFormat TEXTURE_FORMAT;
}
```

### 8. C4JStorage Namespace Stub
```cpp
namespace C4JStorage {
    enum eStorageFacilityEnum { ... };
    typedef eStorageFacilityEnum eStorageFacility;
    typedef eStorageFacilityEnum eGlobalStorage;

    enum eFileTypeValEnum { ... };
    typedef eFileTypeValEnum eFileTypeVal;
    typedef eFileTypeValEnum eTMS_FILETYPEVAL;

    enum EMessageResult { eRESULT_OK, eRESULT_CANCEL, eRESULT_ERROR };
    typedef void* PTMSPP_FILEDATA;
}
```

### 9. CMinecraftApp Stub
- Contains GameRuleManager m_gameRules
- m_hostOptions array
- GetGameHostOption/SetGameHostOption methods
- DebugPrintf implementation

### 10. Other Stubs
- CUI class with GetInstance()
- CTelemetryManager class with GetInstance()
- ShutdownManager namespace
- String resource IDs (IDS_PROGRESS_*)
- eGameHostOption enum (included from App_enums.h)
- Constants: MAX_SAVEFILENAME_LENGTH, LEVEL_LEGACY_WIDTH, etc.

---

## Current Build Status

**Status**: Still failing
**Remaining issues**:

1. **LevelRuleset.h** - Uses StringTable type which isn't defined
2. **LevelGenerationOptions.h** - Same StringTable issue
3. **DedicatedServer_Main.cpp** - Level::enableLightingCache/destroyLightingCache undefined

## Next Steps to Complete

1. Add StringTable forward declaration or stub
2. Add `_DEDICATED_SERVER` guards to LevelRuleset.h and LevelGenerationOptions.h
3. Remove or stub the lighting cache calls in DedicatedServer_Main.cpp
4. Add remaining stubs for any other missing types

---

## Build Commands

### GitHub Actions (automatic)
Push to master triggers build automatically.

### Local Build (if VS2022 installed)
```bash
cd MinecraftConsoles-141217/Minecraft.Client
msbuild Minecraft.Server.vcxproj /p:Configuration=Release /p:Platform=x64
```

### Monitor Build
```bash
gh run list --limit 1
gh run view <run-id> --log-failed
```

---

## Testing Plan

Once build succeeds:
1. Download artifact from GitHub Actions
2. Run with Wine on Linux:
   ```bash
   wine Minecraft.Server.exe
   # Or with arguments:
   wine Minecraft.Server.exe --world "MyWorld" --port 25565
   ```
3. Verify server starts and loads world
4. Test client connection via servers.txt
5. Test clean shutdown with CTRL+C

---

## Key Design Decisions

1. **Stub approach over exclusion**: Rather than excluding all client code, we stub out the platform dependencies. This preserves the existing server logic.

2. **Forced include stdafx_server.h**: All source files automatically include our stubs first via `/FI` compiler flag.

3. **stdafx.h redirect**: Modified the main stdafx.h to redirect to our server version when `_DEDICATED_SERVER` is defined.

4. **Guards in client headers**: Added `#ifdef _DEDICATED_SERVER` guards to client headers to skip problematic definitions.

5. **Namespace stubs**: Created C4JRender and C4JStorage namespaces with stub types matching the real 4JLibs API.

---

## Common Error Patterns and Solutions

| Error Pattern | Solution |
|--------------|----------|
| Type redefinition | Add `_DEDICATED_SERVER` guard to original header |
| Missing type | Add forward declaration or stub to stdafx_server.h |
| Missing 4JLibs class | Add stub to appropriate namespace (C4JRender, C4JStorage) |
| Missing include | Add `_DEDICATED_SERVER` guard around include |
| Function conflict | Remove from HeadlessStubs.cpp if defined inline in header |

---

## Repository Structure

```
MinecraftConsoles-141217/
├── Minecraft.Client/
│   ├── DedicatedServer/
│   │   ├── stdafx_server.h      # Server stubs
│   │   ├── stdafx.h             # Redirect to stdafx_server.h
│   │   ├── DedicatedServer_Main.cpp
│   │   ├── DedicatedServerApp.cpp/h
│   │   ├── ServerProperties.cpp/h
│   │   ├── HeadlessProgressRenderer.cpp/h
│   │   └── HeadlessStubs.cpp
│   ├── Minecraft.Server.vcxproj  # Server build config
│   ├── stdafx.h                  # Modified with redirect
│   └── Common/
│       ├── App_enums.h           # Game enums (included, not stubbed)
│       ├── Consoles_App.h        # CMinecraftApp (guarded)
│       ├── GameRules/
│       │   └── GameRuleManager.h # (guarded)
│       ├── Telemetry/
│       │   └── TelemetryManager.h # (guarded)
│       └── Audio/
│           └── Consoles_SoundEngine.h # (guarded)
├── Minecraft.World/
│   ├── stdafx.h                  # Modified with guards
│   └── x64headers/
│       └── extraX64.h            # Modified with guards
└── .github/
    └── workflows/
        └── build-server.yml      # CI/CD
```

---

*Last updated: 2026-03-10*
*Build attempts: 10+*
*Current status: In progress - fixing remaining type dependencies*
