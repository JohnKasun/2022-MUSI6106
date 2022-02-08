#include "CombFilter.h"
#include <iostream>


//=================================
CCombFilterBase::CCombFilterBase(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) :
		m_fDelayLine(0),
		m_fMaxDelayLengthInS(fMaxDelayLengthInS),
		m_fSampleRateInHz(fSampleRateInHz),
		m_iNumChannels(iNumChannels),
		m_fGainValue(0),
		m_fDelayValueInS(0),
		m_iDelayValueInSamples(0)
{
	m_iMaxDelayLengthInSamples = convertSecondsToSamples(fMaxDelayLengthInS);
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
	m_iDelayValueInSamples = convertSecondsToSamples(fDelayValue);
	for (int channel = 0; channel < m_iNumChannels; channel++)
	{
		int iCurrentReadIdx = m_fDelayLine[channel]->getReadIdx();
		m_fDelayLine[channel]->setWriteIdx(iCurrentReadIdx + m_iDelayValueInSamples);
	}
	return Error_t::kNoError;
}

float CCombFilterBase::getGainValue() const
{
	return m_fGainValue;
}

float CCombFilterBase::getDelayValue() const
{
	return m_fDelayValueInS;
}

int CCombFilterBase::convertSecondsToSamples(float fDelayValue) const
{
	return static_cast<int>(fDelayValue * m_fSampleRateInHz);
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
			ppfAudioOutputBuffer[channel][sample] = fCurrentSample + (m_fGainValue * fCurrentDelayLine->getPostInc());
			fCurrentDelayLine->putPostInc(fCurrentSample);
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
			ppfAudioOutputBuffer[channel][sample] = fNewOutput;
			fCurrentDelayLine->putPostInc(fNewOutput);
		}
	}
	return Error_t::kNoError;
}
//=================================



