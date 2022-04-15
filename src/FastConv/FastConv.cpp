
#include "FastConv.h"

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
CFastConvBase::CFastConvBase(float* pfIr, int iLengthOfIr) :
    m_iLengthOfIr(iLengthOfIr),
    m_iLengthOfTail(iLengthOfIr - 1)
{
    m_pfIr = new float[m_iLengthOfIr] {};
    CVector::copy(m_pfIr, pfIr, m_iLengthOfIr);
}

CFastConvBase::~CFastConvBase()
{
    delete[] m_pfIr;
    m_iLengthOfIr = 0;
    m_iLengthOfTail = 0;
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
}

CFastConvTime::~CFastConvTime()
{
}

Error_t CFastConvTime::process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers)
{
    return Error_t();
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
