#include "CombFilter.h"


//=================================
CCombFilterBase::CCombFilterBase(float fMaxDelayLengthInS, int iNumChannels) :
		m_fMaxDelayLengthInS(fMaxDelayLengthInS),
		m_iNumChannels(iNumChannels),
		m_ParamGainValue(0),
		m_ParamDelayValue(0)
{

}

CCombFilterBase::~CCombFilterBase()
{

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
//=================================



//=================================
CCombFilterFIR::CCombFilterFIR(float fMaxDelayLengthInS, int iNumChannels) :
	CCombFilterBase(fMaxDelayLengthInS, iNumChannels)
{

}

CCombFilterFIR::~CCombFilterFIR()
{

}

Error_t CCombFilterFIR::process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames)
{

}
//=================================




//=================================
CCombFilterIIR::CCombFilterIIR(float fMaxDelayLengthInS, int iNumChannels) :
	 CCombFilterBase(fMaxDelayLengthInS, iNumChannels)
{

}

CCombFilterIIR::~CCombFilterIIR()
{

}

Error_t CCombFilterIIR::process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames)
{

}
//=================================



