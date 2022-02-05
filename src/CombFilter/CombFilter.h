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

	Error_t setGainValue(float fGainValue);
	Error_t setDelayValue(float fDelayValue);
	float getGainValue() const;
	float getDelayValue() const;

protected:

	int convertSecondsToSamples(float fDelayValue) const;

	CRingBuffer<float>** m_fDelayLine;
	float m_fGainValue;
	float m_fDelayValueInS;
	float m_fMaxDelayLengthInS;
	float m_fSampleRateInHz;
	int m_iMaxDelayLengthInSamples;
	int m_iDelayValueInSamples;
	int m_iNumChannels;

};

class CCombFilterFIR : public CCombFilterBase
{
public:
	CCombFilterFIR(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels);
	virtual ~CCombFilterFIR();

	Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) override;
};

class CCombFilterIIR : public CCombFilterBase
{
public:
	CCombFilterIIR(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels);
	virtual ~CCombFilterIIR();

	Error_t process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames) override;
};

#endif //__CombFilter_hdr__