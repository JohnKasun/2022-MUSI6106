
#include "FastConv.h"
#include <algorithm>

CFastConv::CFastConv( void )
{
}

CFastConv::~CFastConv( void )
{
    reset();
}

Error_t CFastConv::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength /*= 8192*/, ConvCompMode_t eCompMode /*= kFreqDomain*/)
{
    if (!pfImpulseResponse)
        return Error_t::kMemError;

    if (iLengthOfIr <= 0 || iBlockLength <= 0)
        return Error_t::kFunctionInvalidArgsError;

    switch (eCompMode)
    {
    case kTimeDomain:
        m_pCCFastConvBase = new CFastConvTime(pfImpulseResponse, iLengthOfIr);
        break;
    case kFreqDomain:
        m_pCCFastConvBase = new CFastConvFreq(pfImpulseResponse, iLengthOfIr, iBlockLength);
        break;
    default:
        return Error_t::kFunctionInvalidArgsError;
    }

    return Error_t::kNoError;
}

Error_t CFastConv::reset()
{
    if (m_pCCFastConvBase)
    {
        delete m_pCCFastConvBase;
        m_pCCFastConvBase = 0;
    }
}

Error_t CFastConv::process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers)
{
    if (!pfOutputBuffer || !pfInputBuffer)
        return Error_t::kMemError;

    if (iLengthOfBuffers < 1 || iLengthOfBuffers > 1000000)
        return Error_t::kFunctionInvalidArgsError;

    return m_pCCFastConvBase->process(pfOutputBuffer, pfInputBuffer, iLengthOfBuffers);
}

Error_t CFastConv::flushBuffer(float* pfOutputBuffer)
{
    if (!pfOutputBuffer)
        return Error_t::kMemError;

    return m_pCCFastConvBase->flushBuffer(pfOutputBuffer);
}
int CFastConv::getTailLength() const
{
    m_pCCFastConvBase->getTailLength();
}
//====================================================================

//====================================================================
CFastConvBase::CFastConvBase(float* pfIr, int iLengthOfIr)
{
    m_iLengthOfIr = iLengthOfIr;
    m_pfIr = new float[m_iLengthOfIr] {};

    CVector::copy(m_pfIr, pfIr, m_iLengthOfIr);
}

CFastConvBase::~CFastConvBase()
{
    delete[] m_pfIr;
    m_pfIr = 0;

    m_iLengthOfIr = 0;
}

Error_t CFastConvBase::flushBuffer(float* pfOutputBuffer)
{
    return Error_t();
}

int CFastConvBase::getTailLength() const
{
    return m_iLengthOfTail;
}
//====================================================================

//====================================================================
CFastConvTime::CFastConvTime(float* pfIr, int iLengthOfIr) :
    CFastConvBase(pfIr, iLengthOfIr)
{
    m_iLengthOfTail = iLengthOfIr - 1;
    m_pfTail = new float[m_iLengthOfTail] {};
}

CFastConvTime::~CFastConvTime()
{
    delete[] m_pfTail;
    m_pfTail = 0;

    m_iLengthOfTail = 0;
}

Error_t CFastConvTime::process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers)
{
    int iMinLength = std::min<int>(m_iLengthOfTail, iLengthOfBuffers);
    CVector::copy(pfOutputBuffer, m_pfTail, iMinLength);
    CVector::moveInMem(m_pfTail, 0, iMinLength - 1, m_iLengthOfTail - iMinLength);
    CVector::setZero(m_pfTail + m_iLengthOfTail - iMinLength, iMinLength);

    for (int n = 0; n < iLengthOfBuffers; n++)
    {
        for (int k = 0; k < m_iLengthOfIr && k <= n; k++)
        {
            pfOutputBuffer[n] += pfInputBuffer[n - k] * m_pfIr[k];
        }
    }


    for (int n = 0; n < m_iLengthOfTail; n++)
    {
        for (int k = 0; k < (m_iLengthOfIr - (n + 1)) && k < iLengthOfBuffers; k++)
        {
            m_pfTail[n] += pfInputBuffer[iLengthOfBuffers - 1 - k] * m_pfIr[k + (n + 1)];
        }
    }

}
//====================================================================

//====================================================================
CFastConvFreq::CFastConvFreq(float* pfIr, int iLengthOfIr, int iBlockLength) :
    CFastConvBase(pfIr, iLengthOfIr)
{
}

CFastConvFreq::~CFastConvFreq()
{
}

Error_t CFastConvFreq::process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers)
{
    return Error_t();
}
