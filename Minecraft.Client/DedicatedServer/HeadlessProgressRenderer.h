// HeadlessProgressRenderer.h
// Console output for progress (replaces GUI ProgressRenderer)
// Implements ProgressListener interface for headless operation

#pragma once

#include "../ProgressRenderer.h"
#include <string>

using namespace std;

class HeadlessProgressRenderer : public ProgressRenderer
{
public:
	enum eProgressStringType
	{
		eProgressStringType_ID,
		eProgressStringType_String,
	};

public:
	HeadlessProgressRenderer();
	virtual ~HeadlessProgressRenderer();

	// ProgressListener interface
	virtual void progressStart(int stringId) override;
	virtual void progressStartNoAbort(int stringId) override;
	virtual void progressStage(int stringId) override;
	virtual void progressStage(wstring& wstrText) override;
	virtual void progressStagePercentage(int percent) override;

	// Query methods (compatible with ProgressRenderer)
	int getCurrentPercent() const { return m_currentPercent; }
	int getCurrentTitle() const { return m_currentTitle; }
	int getCurrentStatus() const { return m_currentStatus; }
	wstring& getProgressString() { return m_progressString; }
	eProgressStringType getType() const { return m_type; }

private:
	// Log progress to console
	void logProgress();

	// Convert string ID to display text
	wstring getStringById(int stringId);

private:
	int m_currentPercent;
	int m_currentTitle;
	int m_currentStatus;
	int m_lastLoggedPercent;
	wstring m_progressString;
	eProgressStringType m_type;
	bool m_noAbort;
};
