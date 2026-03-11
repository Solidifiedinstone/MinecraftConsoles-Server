#pragma once
#include "..\Minecraft.World\ProgressListener.h"

// In dedicated server World-only builds, ServerWorldStubs.h provides a no-op stub.
// Skip the real definition to avoid C2011 redefinition errors.
#ifndef _PROGRESSRENDERER_STUB_DEFINED
class ProgressRenderer : public ProgressListener
{
public:
	enum eProgressStringType
	{
		eProgressStringType_ID,
		eProgressStringType_String, // 4J-PB added for updating the bytes read on a save transfer
	};

	static CRITICAL_SECTION s_progress;

	int getCurrentPercent();
	int getCurrentTitle();
	int getCurrentStatus();
	wstring& getProgressString(void);
	ProgressRenderer::eProgressStringType getType();

private:
	int lastPercent;

private:
	int status;
    Minecraft *minecraft;
    int title;
    __int64 lastTime;
    bool noAbort;
	wstring m_wstrText;
	eProgressStringType m_eType;

	void setType(eProgressStringType eType);

public:
	ProgressRenderer(Minecraft *minecraft);
    virtual void progressStart(int title);
    virtual void progressStartNoAbort(int string);
    void _progressStart(int title);
	virtual void progressStage(int status);
	virtual void progressStage(wstring &wstrText);
    virtual void progressStagePercentage(int i);
};
#endif // _PROGRESSRENDERER_STUB_DEFINED