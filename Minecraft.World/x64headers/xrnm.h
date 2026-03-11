#pragma once
// xrnm.h - stub for Xbox network message types used in Socket.cpp
// XRNM_SEND_BUFFER is normally defined in extraX64.h but is skipped for
// _DEDICATED_SERVER builds, so we define it here for that case.
#if defined(_DEDICATED_SERVER) && !defined(XRNM_SEND_BUFFER_DEFINED)
#define XRNM_SEND_BUFFER_DEFINED
struct XRNM_SEND_BUFFER {
	DWORD dwDataSize;
	BYTE* pbyData;
};
#endif
