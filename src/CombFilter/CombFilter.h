#if !defined(__CombFilter_hdr__)
#define __CombFilter_hdr__

#include "ErrorDef.h"

#include "CombFilterIf.h"


class CCombFilterBase : public CCombFilterIf
{
public:
	CCombFilterBase();
	virtual ~CCombFilterBase();

	virtual Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) = 0;

private:


};

class CCombFilterFIR : public CCombFilterBase
{
public:
	CCombFilterFIR();
	virtual ~CCombFilterFIR();

	Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) override;
};

class CCombFilterIIR : public CCombFilterBase
{
public:
	CCombFilterIIR();
	virtual ~CCombFilterIIR();

	Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) override;
};

#endif //__CombFilter_hdr__