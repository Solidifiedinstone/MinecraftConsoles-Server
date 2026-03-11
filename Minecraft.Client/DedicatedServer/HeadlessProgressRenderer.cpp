// HeadlessProgressRenderer.cpp
// Console output implementation for progress display

#ifdef _DEDICATED_SERVER
#include "stdafx_server.h"
#else
#include "stdafx.h"
#endif

#include "HeadlessProgressRenderer.h"

#include <iostream>
#include <ctime>

// Progress string IDs - must match stdafx_server.h definitions
#define IDS_PROGRESS_INITIALISING_SERVER    1001
#define IDS_PROGRESS_GENERATING_SPAWN_AREA  1002
#define IDS_PROGRESS_LOADING_SPAWN_AREA     1003
#define IDS_PROGRESS_SAVING_CHUNKS          1004
#define IDS_PROGRESS_AUTOSAVING_LEVEL       1005
#define IDS_PROGRESS_SAVING_LEVEL           1006
#define IDS_PROGRESS_SAVING_TO_DISC         1007
#define IDS_PROGRESS_SAVING_PLAYERS         1008

// Log helper
static void ProgressLog(const wchar_t* message)
{
	time_t now = time(nullptr);
	struct tm* timeinfo = localtime(&now);
	wchar_t timestr[32];
	wcsftime(timestr, sizeof(timestr) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", timeinfo);
	std::wcout << L"[" << timestr << L"] [PROGRESS] " << message << std::endl;
}

HeadlessProgressRenderer::HeadlessProgressRenderer()
	: m_currentPercent(0)
	, m_currentTitle(0)
	, m_currentStatus(0)
	, m_lastLoggedPercent(-1)
	, m_type(eProgressStringType_ID)
	, m_noAbort(false)
{
}

HeadlessProgressRenderer::~HeadlessProgressRenderer()
{
}

wstring HeadlessProgressRenderer::getStringById(int stringId)
{
	// Map common string IDs to display text
	// In a full implementation, this would look up the actual string table
	switch (stringId)
	{
	case IDS_PROGRESS_INITIALISING_SERVER:
		return L"Initializing server...";
	case IDS_PROGRESS_GENERATING_SPAWN_AREA:
		return L"Generating spawn area...";
	case IDS_PROGRESS_LOADING_SPAWN_AREA:
		return L"Loading spawn area...";
	case IDS_PROGRESS_SAVING_CHUNKS:
		return L"Saving chunks...";
	case IDS_PROGRESS_AUTOSAVING_LEVEL:
		return L"Autosaving level...";
	case IDS_PROGRESS_SAVING_LEVEL:
		return L"Saving level...";
	case IDS_PROGRESS_SAVING_TO_DISC:
		return L"Saving to disc...";
	case IDS_PROGRESS_SAVING_PLAYERS:
		return L"Saving players...";
	case -1:
		return L"";
	default:
		// Return a generic message with the ID
		wchar_t buf[64];
		swprintf(buf, 64, L"Loading... (stage %d)", stringId);
		return wstring(buf);
	}
}

void HeadlessProgressRenderer::progressStart(int stringId)
{
	m_currentTitle = stringId;
	m_currentStatus = 0;
	m_currentPercent = 0;
	m_lastLoggedPercent = -1;
	m_type = eProgressStringType_ID;
	m_noAbort = false;

	wstring title = getStringById(stringId);
	if (!title.empty())
	{
		ProgressLog(title.c_str());
	}
}

void HeadlessProgressRenderer::progressStartNoAbort(int stringId)
{
	progressStart(stringId);
	m_noAbort = true;
}

void HeadlessProgressRenderer::progressStage(int stringId)
{
	m_currentStatus = stringId;
	m_type = eProgressStringType_ID;

	if (stringId != -1)
	{
		wstring status = getStringById(stringId);
		if (!status.empty())
		{
			ProgressLog(status.c_str());
		}
	}
}

void HeadlessProgressRenderer::progressStage(wstring& wstrText)
{
	m_progressString = wstrText;
	m_type = eProgressStringType_String;

	if (!wstrText.empty())
	{
		ProgressLog(wstrText.c_str());
	}
}

void HeadlessProgressRenderer::progressStagePercentage(int percent)
{
	m_currentPercent = percent;

	// Only log at certain intervals to avoid spam
	// Log at 0%, 25%, 50%, 75%, 100% and when crossing those boundaries
	int logThresholds[] = { 0, 25, 50, 75, 100 };

	bool shouldLog = false;
	for (int threshold : logThresholds)
	{
		if (m_lastLoggedPercent < threshold && percent >= threshold)
		{
			shouldLog = true;
			break;
		}
	}

	if (shouldLog)
	{
		logProgress();
		m_lastLoggedPercent = percent;
	}
}

void HeadlessProgressRenderer::logProgress()
{
	wchar_t buf[256];

	if (m_type == eProgressStringType_String && !m_progressString.empty())
	{
		swprintf(buf, 256, L"%ls: %d%%", m_progressString.c_str(), m_currentPercent);
	}
	else if (m_currentStatus != 0)
	{
		wstring status = getStringById(m_currentStatus);
		swprintf(buf, 256, L"%ls: %d%%", status.c_str(), m_currentPercent);
	}
	else if (m_currentTitle != 0)
	{
		wstring title = getStringById(m_currentTitle);
		swprintf(buf, 256, L"%ls: %d%%", title.c_str(), m_currentPercent);
	}
	else
	{
		swprintf(buf, 256, L"Progress: %d%%", m_currentPercent);
	}

	ProgressLog(buf);
}
