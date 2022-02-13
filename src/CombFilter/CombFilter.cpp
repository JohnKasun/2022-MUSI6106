#include "CombFilter.h"
#include <iostream>


//=================================
CCombFilterBase::CCombFilterBase(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) :
		m_fDelayLine(0),
		m_fMaxDelayLengthInS(fMaxDelayLengthInS),
		m_fSampleRateInHz(fSampleRateInHz),
		m_iNumChannels(iNumChannels),
		m_fGainValue(0),
		m_fDelayValueInS(0)
{
	int m_iMaxDelayLengthInSamples = convertSecondsToSamples(fMaxDelayLengthInS);
	m_fDelayLine = new CRingBuffer<float>*[iNumChannels];
	for (int channel = 0; channel < iNumChannels; channel++)
		m_fDelayLine[channel] = new CRingBuffer<float>(m_iMaxDelayLengthInSamples);
}

CCombFilterBase::~CCombFilterBase()
{
	for (int channel = 0; channel < m_iNumChannels; channel++)
		delete[] m_fDelayLine[channel];
	delete[] m_fDelayLine;
	m_fDelayLine = 0;
}

Error_t CCombFilterBase::setParam(FilterParam_t eParam, float fParamValue)
{
	switch (eParam)
	{
	case FilterParam_t::kParamDelay:
		return setDelayValue(fParamValue);
	case FilterParam_t::kParamGain:
		return setGainValue(fParamValue);
	default:
		return Error_t::kFunctionInvalidArgsError;
	}
}

float CCombFilterBase::getParam(FilterParam_t eParam) const
{
	switch (eParam)
	{
	case FilterParam_t::kParamDelay:
		return m_fDelayValueInS;
	case FilterParam_t::kParamGain:
		return m_fGainValue;
	default:
		return -1;
	}
}

int CCombFilterBase::convertSecondsToSamples(float fDelayValue) const
{
	return static_cast<int>(fDelayValue * m_fSampleRateInHz);
}

Error_t CCombFilterBase::setGainValue(float fGainValue)
{
	if (fGainValue < -1.0 || fGainValue > 1.0)
		return Error_t::kFunctionInvalidArgsError;

	m_fGainValue = fGainValue;
	return Error_t::kNoError;
}

Error_t CCombFilterBase::setDelayValue(float fDelayValue)
{
	if (fDelayValue < 0 || fDelayValue > m_fMaxDelayLengthInS)
		return Error_t::kFunctionInvalidArgsError;

	m_fDelayValueInS = fDelayValue;
	int m_iDelayValueInSamples = convertSecondsToSamples(fDelayValue);
	for (int channel = 0; channel < m_iNumChannels; channel++)
	{
		int iCurrentReadIdx = m_fDelayLine[channel]->getReadIdx();
		m_fDelayLine[channel]->setWriteIdx(iCurrentReadIdx + m_iDelayValueInSamples);
	}
	return Error_t::kNoError;
}
//=================================



//=================================
Error_t CCombFilterFIR::process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames)
{
	for (int channel = 0; channel < m_iNumChannels; channel++)
	{
		CRingBuffer<float>* fCurrentDelayLine = m_fDelayLine[channel];
		for (int sample = 0; sample < iNumberOfFrames; sample++)
		{
			float fCurrentSample = ppfAudioInputBuffer[channel][sample];
			fCurrentDelayLine->putPostInc(fCurrentSample);
			ppfAudioOutputBuffer[channel][sample] = fCurrentSample + (m_fGainValue * fCurrentDelayLine->getPostInc());
		}
	}
	return Error_t::kNoError;
}
//=================================


//=================================
Error_t CCombFilterIIR::process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames)
{
	for (int channel = 0; channel < m_iNumChannels; channel++)
	{
		CRingBuffer<float>* fCurrentDelayLine = m_fDelayLine[channel];
		for (int sample = 0; sample < iNumberOfFrames; sample++)
		{
			float fNewOutput = ppfAudioInputBuffer[channel][sample] + (m_fGainValue * fCurrentDelayLine->getPostInc());
			fCurrentDelayLine->putPostInc(fNewOutput);
			ppfAudioOutputBuffer[channel][sample] = fNewOutput;
		}
	}
	return Error_t::kNoError;
}
//=================================



