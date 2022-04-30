
#if !defined(__FastConv_HEADER_INCLUDED__)
#define __FastConv_HEADER_INCLUDED__

#pragma once

#include "ErrorDef.h"
#include "Vector.h"
#include "Util.h"
#include "Fft.h"

class CFastConvBase;
class CFastConvTime;
class CFastConvFreq;

/*! \brief interface for fast convolution
*/
class CFastConv
{
public:
    enum ConvCompMode_t
    {
        kTimeDomain,
        kFreqDomain,

        kNumConvCompModes
    };

    CFastConv(void);
    virtual ~CFastConv(void);

    /*! initializes the class with the impulse response and the block length
    \param pfImpulseResponse impulse response samples (mono only)
    \param iLengthOfIr length of impulse response
    \param iBlockLength processing block size
    \return Error_t
    */
    Error_t init(float* pfImpulseResponse, int iLengthOfIr, int iBlockLength = 8192, ConvCompMode_t eCompMode = kFreqDomain);

    /*! resets all internal class members
    \return Error_t
    */
    Error_t reset ();

    /*! computes the output with reverb
    \param pfOutputBuffer (mono)
    \param pfInputBuffer (mono)
    \param iLengthOfBuffers can be anything from 1 sample to 10000000 samples
    \return Error_t
    */
    Error_t process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers);

    /*! return the 'tail' after processing has finished (identical to feeding in zeros
    \param pfOutputBuffer (mono)
    \return Error_t
    */
    Error_t flushBuffer(float* pfOutputBuffer);

    /*! return the 'tail' length. Should be called prior to 'flushBuffer(...)'
    \return int
    */
    int getTailLength() const;

private:

    CFastConvBase* m_pCCFastConvBase = 0;

};

class CFastConvBase
{
public:

    CFastConvBase(float* pfIr, int iLengthOfIr);
    virtual ~CFastConvBase();

    virtual Error_t process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers) = 0;
    Error_t flushBuffer(float* pfOutputBuffer);
    int getTailLength() const;

protected:

    float* m_pfIr = 0;
    float* m_pfTail = 0;
    int m_iLengthOfIr = 0;
    int m_iLengthOfTail = 0;


};

class CFastConvTime : public CFastConvBase
{
public:

    CFastConvTime(float* pfIr, int iLengthOfIr);
    ~CFastConvTime();

    Error_t process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers) override;

private:

};

class CFastConvFreq : public CFastConvBase
{
public:

    CFastConvFreq(float* pfIr, int iLengthOfIr, int iBlockLength);
    ~CFastConvFreq();

    Error_t process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers) override;

private:

    int m_iBlockLength;
    int m_iNumBlocks;

    int m_iReadBlock;
    int m_iWriteBlock;
    int m_iReadIdx;
    int m_iWriteIdx;

    CFft* m_pFFT = 0;

    CFft::complex_t* m_pfComplexBuffer;

    float* m_pfFFTReal;
    float* m_pfFFTImag;
    float* m_pfFFTRealCurr;
    float* m_pfFFTImagCurr;
    float* m_pfIFFT;

    float** m_ppfIRFreqReal;
    float** m_ppfIRFreqImag;

    float* m_pfInputBuffer;
    float** m_ppfOutputBuffer;
    

};


#endif
