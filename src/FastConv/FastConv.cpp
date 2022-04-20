
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
    return Error_t::kNoError;
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
    return m_pCCFastConvBase->getTailLength();
}
//====================================================================

//====================================================================
CFastConvBase::CFastConvBase(float* pfIr, int iLengthOfIr)
{
    m_iLengthOfIr = iLengthOfIr;
    m_pfIr = new float[m_iLengthOfIr] {};

    CVectorFloat::copy(m_pfIr, pfIr, m_iLengthOfIr);
}

CFastConvBase::~CFastConvBase()
{
    delete[] m_pfIr;
    m_pfIr = 0;

    m_iLengthOfIr = 0;
}

Error_t CFastConvBase::flushBuffer(float* pfOutputBuffer)
{
    CVectorFloat::copy(pfOutputBuffer, m_pfTail, m_iLengthOfTail);
    return Error_t::kNoError;
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

    // Copy as many tail values as possible into output buffer
    int iMinLength = std::min<int>(m_iLengthOfTail, iLengthOfBuffers);
    CVectorFloat::copy(pfOutputBuffer, m_pfTail, iMinLength);

    // Move remaining tail values up to front of buffer
    CVectorFloat::moveInMem(m_pfTail, 0, iMinLength, m_iLengthOfTail - iMinLength);

    // Set extra values at end of tail to zero
    CVectorFloat::setZero(m_pfTail + m_iLengthOfTail - iMinLength, iMinLength);

    // Do convolution for the amount that will fit in IO buffers
    for (int n = 0; n < iLengthOfBuffers; n++)
    {
        for (int k = 0; k < m_iLengthOfIr && k <= n; k++)
        {
            pfOutputBuffer[n] += pfInputBuffer[n - k] * m_pfIr[k];
        }
    }

    // Complete convolution and add values to current tail
    for (int n = 0; n < m_iLengthOfTail; n++)
    {
        for (int k = 0; k < (m_iLengthOfIr - (n + 1)) && k < iLengthOfBuffers; k++)
        {
            m_pfTail[n] += pfInputBuffer[iLengthOfBuffers - 1 - k] * m_pfIr[k + (n + 1)];
        }
    }

    return Error_t::kNoError;

}
//====================================================================

//====================================================================
CFastConvFreq::CFastConvFreq(float* pfIr, int iLengthOfIr, int iBlockLength) :
    CFastConvBase(pfIr, iLengthOfIr)
{
    
    // init fft
    CFft* m_pFFT = 0;
    CFft::createInstance(m_pFFT);
    m_pFFT->initInstance(m_iBlockLength, 2, CFft::kWindowHann, CFft::kNoWindow);

    m_iBlockLength = m_pFFT->getLength(CFft::kLengthData);
    m_iFftLength   = m_pFFT->getLength(CFft::kLengthFft);
    m_iNumIrBlocks = static_cast<int>(m_iLengthOfIr / static_cast<float>(m_iBlockLength)) + 1;

    // index flags
    m_iReadBlock = m_iNumIrBlocks - 1;
    m_iWriteBlock = 0;
    m_iReadIdx = 0;
    m_iWriteIdx = 0;
    
    // init real/imag buffers
    m_pfComplexBuffer = new CFft::complex_t[m_iFftLength];

    m_pfFFTReal = new float[m_iBlockLength + 1];
    m_pfFFTImag = new float[m_iBlockLength + 1];
    m_pfFFTRealCurr = new float[m_iBlockLength + 1];
    m_pfFFTImagCurr = new float[m_iBlockLength + 1];
    m_pfIFFT = new float[m_iFftLength];

    m_ppfIRFreqReal = new float* [m_iNumIrBlocks];
    m_ppfIRFreqImag = new float* [m_iNumIrBlocks];
    for (int i = 0; i < m_iNumIrBlocks; i++)
    {
        // init ir frequency domain buffers
        m_ppfIRFreqReal[i] = new float[m_iBlockLength + 1];
        m_ppfIRFreqImag[i] = new float[m_iBlockLength + 1];
    }
    
    m_pfProcessBuf = new float[m_iFftLength] {};
    
    
    float* cur = m_pfIr;
    for (int block = 0; block < m_iNumIrBlocks; block++)
    {
        CVectorFloat::setZero(m_pfProcessBuf, m_iFftLength);
        CVectorFloat::copy(m_pfProcessBuf, cur, std::min<int>(m_iBlockLength, m_iLengthOfIr - (block * m_iBlockLength)));

        m_pFFT->doFft(m_pfProcessBuf, m_pfProcessBuf);
        CVectorFloat::mulC_I(m_pfProcessBuf, m_iFftLength, m_iFftLength);
        m_pFFT->splitRealImag(m_ppfIRFreqReal[block], m_ppfIRFreqImag[block], m_pfProcessBuf);

        cur += iBlockLength;
    }
    
}

CFastConvFreq::~CFastConvFreq()
{
    
    delete[] m_pfFFTReal;
    delete[] m_pfFFTImag;
    delete[] m_pfFFTRealCurr;
    delete[] m_pfFFTImagCurr;
    delete[] m_pfIFFT;
    delete[] m_pfComplexBuffer;
    delete[] m_pfProcessBuf;

    for (int i = 0; i < m_iNumIrBlocks; i++)
    {
        delete[] m_ppfIRFreqReal[i];
        delete[] m_ppfIRFreqImag[i];
    }
    
    delete[] m_ppfIRFreqReal;
    delete[] m_ppfIRFreqImag;
    
    CFft::destroyInstance(m_pFFT);
}

Error_t CFastConvFreq::process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers)
{

    // Copy as many tail values as possible into output buffer
    int iMinLength = std::min<int>(m_iLengthOfTail, iLengthOfBuffers);
    CVectorFloat::copy(pfOutputBuffer, m_pfTail, iMinLength);

    // Move remaining tail values up to front of buffer
    CVectorFloat::moveInMem(m_pfTail, 0, iMinLength, m_iLengthOfTail - iMinLength);

    // Set extra values at end of tail to zero
    CVectorFloat::setZero(m_pfTail + m_iLengthOfTail - iMinLength, iMinLength);

    //TODO: Add asserts/errors
    int iUpdateWriteIdx;
    
    for (int i = 0; i < iLengthOfBuffers; i++)
    {
        m_pfInputBuffer[m_iWriteIdx + m_iBlockLength] = pfInputBuffer[i];
        pfOutputBuffer[i] = m_ppfOutputBuffer[m_iReadBlock][m_iWriteIdx];
        
        m_iWriteIdx++;
        
        if (m_iWriteIdx == m_iBlockLength)
        {
            m_iWriteIdx = 0;
            
            // not sure if this is necessary
            for (int j = 0; j < m_iBlockLength; i++)
            {
                m_ppfOutputBuffer[m_iReadBlock][j] = 0;
            }
            
            m_pFFT->doFft(m_pfComplexBuffer, m_pfIFFT);
            m_pFFT->splitRealImag(m_pfFFTRealCurr, m_pfFFTImagCurr, m_pfComplexBuffer);
            
            for (int k = 0; k < m_iNumBlocks; i++)
            {
                // complex multiplication
                for (int m = 0; m <= m_iBlockLength; m++)
                {
                    m_pfFFTReal[m] = (m_pfFFTRealCurr[m] * m_ppfIRFreqReal[k][m] - m_pfFFTImagCurr[m] * m_ppfIRFreqImag[k][m]) * 2 * m_iBlockLength;
                    m_pfFFTImag[m] = (m_pfFFTRealCurr[m] * m_ppfIRFreqImag[k][m] + m_pfFFTImagCurr[k] * m_ppfIRFreqReal[k][m]) * 2 * m_iBlockLength;
                }
                
                m_pFFT->mergeRealImag(m_pfComplexBuffer, m_pfFFTReal, m_pfFFTImag);
                m_pFFT->doInvFft(m_pfIFFT, m_pfComplexBuffer);
                
                iUpdateWriteIdx = (m_iWriteBlock + k) % m_iNumBlocks;
                
                for (int m = 0; m <= m_iBlockLength; m++)
                {
                    m_ppfOutputBuffer[iUpdateWriteIdx][m] += m_pfIFFT[k + m_iBlockLength];
                }
            }
            
            for (int n = 0; n < m_iBlockLength; n++)
            {
                m_pfInputBuffer[n] = m_pfInputBuffer[n + m_iBlockLength];
            }
            
            m_iReadBlock = m_iWriteBlock;
            m_iWriteBlock = (m_iWriteBlock + 1) % m_iNumBlocks;
        }
    }
    //TODO: Flush buffer for the freqFastConv - I think it is block length + ir length - 1 for the latency
    return Error_t();
}

