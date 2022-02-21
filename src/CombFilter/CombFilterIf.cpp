
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilterIf.h"
#include "CombFilter.h"


static const char*  kCMyProjectBuildDate = __DATE__;


CCombFilterIf::CCombFilterIf () :
    m_bIsInitialized(false),
    m_pCCombFilter(0),
    m_fSampleRate(0)
{
    // this should never hurt
    this->reset ();
}


CCombFilterIf::~CCombFilterIf ()
{
    this->reset ();
}

const int  CCombFilterIf::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MUSI6106_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MUSI6106_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MUSI6106_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CCombFilterIf::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CCombFilterIf::create (CCombFilterIf*& pCCombFilter)
{
    if (pCCombFilter)
        return Error_t::kMemError;

    pCCombFilter = new CCombFilterIf();
    return Error_t::kNoError;
}

Error_t CCombFilterIf::destroy (CCombFilterIf*& pCCombFilter)
{
    delete pCCombFilter;
    pCCombFilter = 0;
    return Error_t::kNoError;
}

Error_t CCombFilterIf::init(CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels)
{
    if (!m_bIsInitialized)
    {
        if (fMaxDelayLengthInS < 0.0f || fSampleRateInHz < 0.0f || iNumChannels < 0)
            return Error_t::kFunctionInvalidArgsError;

        switch (eFilterType) 
        {
        case CombFilterType_t::kCombFIR:
            m_pCCombFilter = new CCombFilterFIR(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels);
            break;
        case CombFilterType_t::kCombIIR:
            m_pCCombFilter = new CCombFilterIIR(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels);
            break;
        default:
            return Error_t::kFunctionInvalidArgsError;
        }
        m_bIsInitialized = true;
        m_fSampleRate = fSampleRateInHz;
    }
    return Error_t::kNoError;
}

Error_t CCombFilterIf::reset ()
{
    m_bIsInitialized = false;
    m_fSampleRate = 0.0f;
    delete m_pCCombFilter;
    m_pCCombFilter = 0;
    return Error_t::kNoError;
}

Error_t CCombFilterIf::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    return m_pCCombFilter->process(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
}

Error_t CCombFilterIf::setParam(FilterParam_t eParam, float fParamValue)
{
    return m_pCCombFilter->setParam(eParam, fParamValue);
}

float CCombFilterIf::getParam (FilterParam_t eParam) const
{
    return m_pCCombFilter->getParam(eParam);
}
