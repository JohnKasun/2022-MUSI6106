#include "CombFilter.h"


//=================================
CCombFilterBase::CCombFilterBase(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) :
		m_fDelayLine(0),
		m_fMaxDelayLengthInS(fMaxDelayLengthInS),
		m_fSampleRateInHz(fSampleRateInHz),
		m_iNumChannels(iNumChannels),
		m_ParamGainValue(0),
		m_ParamDelayValue(0)
{
	m_fDelayLine = new CRingBuffer<float>(static_cast<int>(fMaxDelayLengthInS));
}

CCombFilterBase::~CCombFilterBase()
{
	delete m_fDelayLine;
	m_fDelayLine = 0;
}

Error_t CCombFilterBase::setGainValue(float fGainValue)
{
	if (fGainValue < -1.0 || fGainValue > 1.0)
		return Error_t::kFunctionInvalidArgsError;
	m_ParamGainValue = fGainValue;
	return Error_t::kNoError;
}

Error_t CCombFilterBase::setDelayValue(float fDelayValue)
{
	if (fDelayValue < 0 || fDelayValue > m_fMaxDelayLengthInS)
		return Error_t::kFunctionInvalidArgsError;
	m_ParamDelayValue = fDelayValue;
	return Error_t::kNoError;
}

float CCombFilterBase::getGainValue() const
{
	return m_ParamGainValue;
}

float CCombFilterBase::getDelayValue() const
{
	return m_ParamDelayValue;
}

int CCombFilterBase::convertSecondsToSamples(float fDelayValue) const
{
	return static_cast<int>(fDelayValue * m_fSampleRateInHz);
}
//=================================



//=================================
CCombFilterFIR::CCombFilterFIR(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) :
	CCombFilterBase(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels)
{

}

CCombFilterFIR::~CCombFilterFIR()
{

}

Error_t CCombFilterFIR::process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames)
{
	m_fDelayLine->reset();
	for (int sample = 0; sample < iNumberOfFrames; sample++)
	{
		for (int channel = 0; channel < m_iNumChannels; channel++)
		{
			ppfAudioOutputBuffer[channel][sample] = ppfAudioInputBuffer[channel][sample];
		}
	}
	return Error_t::kNoError;
}
//=================================




//=================================
CCombFilterIIR::CCombFilterIIR(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) :
	 CCombFilterBase(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels)
{

}

CCombFilterIIR::~CCombFilterIIR()
{

}

Error_t CCombFilterIIR::process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames)
{
	return Error_t::kNoError;
}
//=================================



