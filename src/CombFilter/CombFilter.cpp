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
	m_fDelayLine = new CRingBuffer<float>(static_cast<int>(m_iMaxDelayLengthInSamples));
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

	m_fGainValue = fGainValue;
	return Error_t::kNoError;
}

Error_t CCombFilterBase::setDelayValue(float fDelayValue)
{
	if (fDelayValue < 0 || fDelayValue > m_fMaxDelayLengthInS)
		return Error_t::kFunctionInvalidArgsError;

	m_fDelayValueInS = fDelayValue;
	m_iDelayValueInSamples = convertSecondsToSamples(fDelayValue);
	m_fDelayLine->setEndPosition(m_iDelayValueInSamples);
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
CCombFilterFIR::CCombFilterFIR(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) :
	CCombFilterBase(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels)
{

}

CCombFilterFIR::~CCombFilterFIR()
{

}

Error_t CCombFilterFIR::process(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumberOfFrames)
{
	for (int channel = 0; channel < m_iNumChannels; channel++)
	{
		m_fDelayLine->reset();
		for (int sample = 0; sample < iNumberOfFrames; sample++)
		{
			float fCurrentSample = ppfAudioInputBuffer[channel][sample];
			float fNewOutput = fCurrentSample + (m_fGainValue * m_fDelayLine->getPostInc());
			ppfAudioOutputBuffer[channel][sample] = fNewOutput;
			for (int i = 0; i < m_fDelayLine->getCurrentLength() - 1; i++)
			{
				float fValueToBeShifted = m_fDelayLine->getPostInc();
				m_fDelayLine->putPostInc(fValueToBeShifted);
			}
			m_fDelayLine->put(fCurrentSample);
			m_fDelayLine->setReadIdx(0);
			m_fDelayLine->setWriteIdx(0);
			m_fDelayLine->display();
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



