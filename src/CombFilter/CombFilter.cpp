#include "CombFilter.h"


//=================================
CCombFilterBase::CCombFilterBase() :
		m_ParamGainValue(0),
		m_ParamDelayValue(0)
{

}

CCombFilterBase::~CCombFilterBase()
{

}

Error_t CCombFilterBase::setGainValue(float fGainValue)
{
	if (fGainValue < -1 || fGainValue > 1)
		return Error_t::kFunctionInvalidArgsError;
	m_ParamGainValue = fGainValue;
	return Error_t::kNoError;
}

Error_t CCombFilterBase::setDelayValue(float fDelayValue)
{
	if (fDelayValue < 0)
		return Error_t::kFunctionInvalidArgsError;
	m_ParamDelayValue = fDelayValue;
	return Error_t::kNoError;
}

float CCombFilterBase::getGainValue() const
{
	return m_ParamGainValue;
}

float CCombFilterBase::getGainValue() const
{
	return m_ParamDelayValue;
}
//=================================



//=================================
CCombFilterFIR::CCombFilterFIR()
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
CCombFilterIIR::CCombFilterIIR()
{

}

CCombFilterIIR::~CCombFilterIIR()
{

}

Error_t CCombFilterIIR::process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames)
{

}
//=================================



