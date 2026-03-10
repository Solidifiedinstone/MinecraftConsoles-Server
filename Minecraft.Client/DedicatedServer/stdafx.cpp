// stdafx.cpp for Dedicated Server
// Creates precompiled header using server-specific stdafx_server.h

#include "stdafx_server.h"

// Global app instance for server
CMinecraftApp app;

// Global network manager instance
CGameNetworkManager g_NetworkManager;

// Static members for CGameNetworkManager
__int64 CGameNetworkManager::messageQueue[CGameNetworkManager::messageQueue_length];
__int64 CGameNetworkManager::byteQueue[CGameNetworkManager::byteQueue_length];
int CGameNetworkManager::messageQueuePos = 0;

// Static members for IQNet
IQNetPlayer IQNet::m_player[4];
