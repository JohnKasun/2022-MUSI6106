#if !defined(__CombFilter_hdr__)
#define __CombFilter_hdr__

#include "ErrorDef.h"

#include "CombFilterIf.h"
#include "RingBuffer.h"


class CCombFilterBase : public CCombFilterIf
{
public:
	CCombFilterBase(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels);
	virtual ~CCombFilterBase();

	virtual Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) = 0;

	Error_t setParam(FilterParam_t eParam, float fParamValue);
	float getParam(FilterParam_t eParam) const;

protected:

	int convertSecondsToSamples(float fDelayValue) const;
	Error_t setGainValue(float fGainValue);
	Error_t setDelayValue(float fDelayValue);

	CRingBuffer<float>** m_fDelayLine;
	float m_fGainValue;
	float m_fDelayValueInS;
	float m_fMaxDelayLengthInS;
	float m_fSampleRateInHz;
	int m_iNumChannels;

};

class CCombFilterFIR : public CCombFilterBase
{
public:
	CCombFilterFIR(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) : 
		CCombFilterBase(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels) {};
	virtual ~CCombFilterFIR() = default;

	Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) override;
};

class CCombFilterIIR : public CCombFilterBase
{
public:
	CCombFilterIIR(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) :
		CCombFilterBase(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels) {};
	virtual ~CCombFilterIIR() = default;

	Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) override;
};

#endif //__CombFilter_hdr__