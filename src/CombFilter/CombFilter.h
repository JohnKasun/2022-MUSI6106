#if !defined(__CombFilter_hdr__)
#define __CombFilter_hdr__

#include "ErrorDef.h"

#include "CombFilterIf.h"


class CCombFilterBase : public CCombFilterIf
{
public:
	CCombFilterBase(float fMaxDelayLengthInS, int iNumChannels);
	virtual ~CCombFilterBase();

	virtual Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) = 0;

	Error_t setGainValue(float fGainValue);
	Error_t setDelayValue(float fDelayValue);
	float getGainValue() const;
	float getDelayValue() const;

private:

	float m_ParamGainValue;
	float m_ParamDelayValue;
	float m_fMaxDelayLengthInS;
	int m_iNumChannels;

};

class CCombFilterFIR : public CCombFilterBase
{
public:
	CCombFilterFIR(float fMaxDelayLengthInS, int iNumChannels);
	virtual ~CCombFilterFIR();

	Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) override;
};

class CCombFilterIIR : public CCombFilterBase
{
public:
	CCombFilterIIR(float fMaxDelayLengthInS, int iNumChannels);
	virtual ~CCombFilterIIR();

	Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) override;
};

#endif //__CombFilter_hdr__