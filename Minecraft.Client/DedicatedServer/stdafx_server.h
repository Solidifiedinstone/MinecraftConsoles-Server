// stdafx_server.h - Precompiled header for Dedicated Server
// This replaces stdafx.h for headless server builds without DirectX dependencies

#pragma once

#ifndef _DEDICATED_SERVER
#define _DEDICATED_SERVER 1
#endif

#ifndef _WINDOWS64
#define _WINDOWS64 1
#endif

#define WIN32_LEAN_AND_MEAN

// Windows headers first (MinGW compatible) - they define types we need
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// Standard C++ headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

// Type definitions (only if not already defined)
#ifndef __uint64
typedef unsigned long long __uint64;
#endif

// Helper macros
#define AUTO_VAR(_var, _val) auto _var = _val

#ifndef HRESULT_SUCCEEDED
#define HRESULT_SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

// String helpers
using std::wstring;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

// Stub out DirectX types that aren't needed for server
struct XMFLOAT3 { float x, y, z; };
struct XMFLOAT4 { float x, y, z, w; };
struct XMMATRIX { float m[4][4]; };

// PIX stubs (performance instrumentation)
inline void PIXBeginNamedEvent(int, const char*, ...) {}
inline void PIXEndNamedEvent() {}

// Memory tracking stubs
inline void MemSect(int) {}

// Platform stubs (only define if not already defined by Windows headers)
#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 4
#endif

#ifndef XUSER_INDEX_ANY
#define XUSER_INDEX_ANY -1
#endif

#ifndef CPU_CORE_SERVER
#define CPU_CORE_SERVER 0
#endif

// Helper to set thread name (debug only)
inline void SetThreadName(int, const char*) {}
