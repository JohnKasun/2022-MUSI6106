
#include <iostream>
#include <ctime>
#include <cctype>
#define _USE_MATH_DEFINES
#include "math.h"

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();
void    displayIOBuffers(float**, float**, int, int);
void    toLower(std::string&);
void    runTests();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string sInputFilePath,                 //!< file paths
                sOutputFilePath;

    static const int kBlockSize = 1024;

    clock_t time = 0;

    float **ppfAudioData = 0;

    CAudioFileIf *phAudioFile = 0;
    std::fstream hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    CCombFilterIf* phCombFilter = 0;
    CCombFilterIf::CombFilterType_t eFilterType;
    float fMaxDelay;
    float fDelayParam;
    float fGainParam;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path! -- running tests...";
        runTests();
        return 0;
    }
    else if (argc < 6)
    {
        cout << "Missing filter parameters!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];

        sOutputFilePath = sInputFilePath + ".txt";

        std::string sFilterType = argv[2];
        toLower(sFilterType);
        if (sFilterType == "fir")
            eFilterType = CCombFilterIf::CombFilterType_t::kCombFIR;
        else if (sFilterType == "iir")
            eFilterType = CCombFilterIf::CombFilterType_t::kCombIIR;
        else
        {
            cout << "Invalid filter type...";
            return -1;
        }

        fMaxDelay = atof(argv[3]);
        fDelayParam = atof(argv[4]);
        fGainParam = atof(argv[5]);
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        CAudioFileIf::destroy(phAudioFile);
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        CAudioFileIf::destroy(phAudioFile);
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kBlockSize];

    if (ppfAudioData == 0)
    {
        CAudioFileIf::destroy(phAudioFile);
        hOutputFile.close();
        return -1;
    }
    if (ppfAudioData[0] == 0)
    {
        CAudioFileIf::destroy(phAudioFile);
        hOutputFile.close();
        return -1;
    }

    time = clock();

    //////////////////////////////////////////////////////////////////////////////
    // initialize Comb Filter
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(eFilterType, fMaxDelay, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, fDelayParam);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, fGainParam);

    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output text file (one column per channel)
    // apply comb filter
    while (!phAudioFile->isEof())
    {
        // set block length variable
        long long iNumFrames = kBlockSize;

        // read data (iNumOfFrames might be updated!)
        phAudioFile->readData(ppfAudioData, iNumFrames);
        phCombFilter->process(ppfAudioData, ppfAudioData, iNumFrames);

        cout << "\r" << "reading and writing";

        // write
        for (int i = 0; i < iNumFrames; i++)
        {
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
            {
                hOutputFile << ppfAudioData[c][i] << "\t";
            }
            hOutputFile << endl;
        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    CAudioFileIf::destroy(phAudioFile);
    CCombFilterIf::destroy(phCombFilter);
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = 0;

    // all done
    return 0;

}

void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout << endl;

    return;
}

bool test1();
void test2();
bool test3();
bool test4();
bool test5();

void runTests()
{
    bool test1passed = test1();
    test2();
    bool test3passed = test3();
    bool test4passed = test4();
    bool test5passed = test5();
}

//Display Input and Output Audio Buffers
void displayIOBuffers(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumChannels, int iNumSamples)
{
    std::cout << "\n=======================================" << std::endl;
    std::cout << "Input: " << std::endl;
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        std::cout << "\tChannel " << channel << ": [";
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            std::cout << ppfAudioInputBuffer[channel][sample] << " ";
        }
        std::cout << "]" << std::endl;
    }

    std::cout << "Output: " << std::endl;
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        std::cout << "\tChannel " << channel << ": [";
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            std::cout << ppfAudioOutputBuffer[channel][sample] << " ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "=======================================" << std::endl;
}

//Displays single Audio Buffer
void displayBuffer(float** ppfAudioBuffer, int iNumChannels, int iNumSamples)
{
    std::cout << "Buffer: " << std::endl;
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        std::cout << "\tChannel " << channel << ": [";
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            std::cout << ppfAudioBuffer[channel][sample] << " ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "=======================================" << std::endl;
}

//Converts string to lowercase
void toLower(std::string &sStringToLower)
{
    for (char& letter : sStringToLower)
        letter = tolower(letter);
}

//Test for output being zero when input freq matches feedforward
//If fPeriod * fSampleRate is a whole number, then all values will be practically zero for all samples once the delay comes in.
//For this set of values, all values will be zero starting at sample 4.
//This works best for high sample rates and high frequencies
bool test1()
{
    const float fTwoPi = 2.0f * M_PI;
    const int iNumChannels = 2;
    const int iNumSamples = 1000;
    const float fSampleRate = 40000;
    const float iInputFreq = 10000;
    const float fPeriod = 1.0f / iInputFreq;
    const int iSampleDelayComesIn = fPeriod * fSampleRate;

    float** ppfInputBuffer = new float* [iNumChannels];
    float** ppfOutputBuffer = new float* [iNumChannels];
    for (int i = 0; i < iNumChannels; i++)
    {
        ppfInputBuffer[i] = new float[iNumSamples] {};
        ppfOutputBuffer[i] = new float[iNumSamples] {};
    }

    for (int channel = 0; channel < iNumChannels; channel++)
    {
        float fAngleDelta = fTwoPi * iInputFreq / fSampleRate;
        float fCurrentAngle = 0.0f;
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            ppfInputBuffer[channel][sample] = sin(fCurrentAngle * sample);
            fCurrentAngle += fAngleDelta;
            while (fCurrentAngle >= fTwoPi)
                fCurrentAngle -= fTwoPi;
        }

    }

    CCombFilterIf* phCombFilter = 0;
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombFIR, 1, fSampleRate, iNumChannels);

    //Set delay value to the period length and set gain to be inverting
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, fPeriod);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, -1);
    phCombFilter->process(ppfInputBuffer, ppfOutputBuffer, iNumSamples);

    bool passed = true;
    for (int channel = 0; channel < iNumChannels; channel++)
        for (int sample = iSampleDelayComesIn; sample < iNumSamples; sample++)
        {
             //Test fails if processed value is greater than 1 * 10^-4
             if (ppfOutputBuffer[channel][sample] > 1E-4)
                 passed = false;
        }

    //display Output buffer here if you'd like
    //displayBuffer(ppfOutputBuffer, iNumChannels, iNumSamples);
    CCombFilterIf::destroy(phCombFilter);
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        delete[] ppfInputBuffer[channel];
        delete[] ppfOutputBuffer[channel];
    }
    delete[] ppfInputBuffer;
    delete[] ppfOutputBuffer;

    return passed;
}

//Test for amount of magnitude increase/decrease if input freq matches feedback
//The output sample is divided by the input sample and stored in the ppfMagnitudeRatios buffer
void test2()
{
    const float fTwoPi = 2.0f * M_PI;
    const int iNumChannels = 2;
    const int iNumSamples = 1000;
    const float fSampleRate = 40000;
    const int iInputFreq = 1000;
    const float fPeriod = 1.0f / iInputFreq;
    const int iSampleDelayComesIn = fPeriod * fSampleRate;

    float** ppfInputBuffer = new float* [iNumChannels];
    float** ppfOutputBuffer = new float* [iNumChannels];
    float** ppfMagnitudeRatios = new float* [iNumChannels];
    for (int i = 0; i < iNumChannels; i++)
    {
        ppfInputBuffer[i] = new float[iNumSamples] {};
        ppfOutputBuffer[i] = new float[iNumSamples] {};
        ppfMagnitudeRatios[i] = new float[iNumSamples] {};
    }

    for (int channel = 0; channel < iNumChannels; channel++)
    {
        float fAngleDelta = fTwoPi * iInputFreq / fSampleRate;
        float fCurrentAngle = 0.0f;
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            ppfInputBuffer[channel][sample] = sin(fCurrentAngle * sample);
            fCurrentAngle += fAngleDelta;
            while (fCurrentAngle >= fTwoPi)
                fCurrentAngle -= fTwoPi;
        }
    }


    CCombFilterIf* phCombFilter = 0;
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombIIR, 1, fSampleRate, iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, fPeriod);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);
    phCombFilter->process(ppfInputBuffer, ppfOutputBuffer, iNumSamples);

    bool passed = true;
    for (int channel = 0; channel < iNumChannels; channel++)
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            float fInputSample = ppfInputBuffer[channel][sample];
            float fOutputSample = ppfOutputBuffer[channel][sample];
            float fMagnitudeRatio{INFINITY};
            if (fInputSample != 0)
                fMagnitudeRatio = fOutputSample / fInputSample;
            ppfMagnitudeRatios[channel][sample] = fMagnitudeRatio;
        }

    //display buffer here if you'd like
    //displayBuffer(ppfMagnitudeRatios, iNumChannels, iNumSamples);
    CCombFilterIf::destroy(phCombFilter);
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        delete[] ppfInputBuffer[channel];
        delete[] ppfOutputBuffer[channel];
        delete[] ppfMagnitudeRatios[channel];
    }
    delete[] ppfInputBuffer;
    delete[] ppfOutputBuffer;
    delete[] ppfMagnitudeRatios;

}

//Helper function for processing a block of audio data with comb filter
//iStartSample is the offset corresponding to the current read/write position of the original buffer
void processBlock(float** ppfInputBuffer, float** ppfOutputBuffer, CCombFilterIf* phCombFilter, int iNumChannels, int iStartSample, int iBlockSize)
{
    float** ppfTempInBuffer = new float* [iNumChannels];
    float** ppfTempOutBuffer = new float* [iNumChannels];
    for (int i = 0; i < iNumChannels; i++)
    {
        ppfTempInBuffer[i] = new float[iBlockSize] {0};
        ppfTempOutBuffer[i] = new float[iBlockSize] {0};
    }

    for (int channel = 0; channel < iNumChannels; channel++)
    {
        for (int sample = 0; sample < iBlockSize; sample++)
        {
            ppfTempInBuffer[channel][sample] = ppfInputBuffer[channel][sample + iStartSample];
        }
        phCombFilter->process(ppfTempInBuffer, ppfTempOutBuffer, iBlockSize);
        for (int sample = 0; sample < iBlockSize; sample++)
        {
            ppfOutputBuffer[channel][sample + iStartSample] = ppfTempOutBuffer[channel][sample];
        }
    }

    for (int channel = 0; channel < iNumChannels; channel++)
    {
        delete[] ppfTempInBuffer[channel];
        delete[] ppfTempOutBuffer[channel];
    }
    delete[] ppfTempInBuffer;
    delete[] ppfTempOutBuffer;
}

//Test for varying input block size
//Uses processBlock(...) helper function with the following block sizes: 1000, 4000, 5000
bool test3()
{
    const int iNumChannels = 1;
    const int iNumSamples = 10000;
    const float fSampleRate = 44100;

    float** ppfInputBuffer = new float* [iNumChannels];
    float** ppfOutputBufferFIR = new float* [iNumChannels];
    float** ppfOutputBufferIIR = new float* [iNumChannels];
    float** ppfOutputBufferFIRBlock = new float* [iNumChannels];
    float** ppfOutputBufferIIRBlock = new float* [iNumChannels];
    for (int i = 0; i < iNumChannels; i++)
    {
        ppfInputBuffer[i] = new float[iNumSamples] {0};
        ppfOutputBufferFIR[i] = new float[iNumSamples] {0};
        ppfOutputBufferIIR[i] = new float[iNumSamples] {0};
        ppfOutputBufferFIRBlock[i] = new float[iNumSamples] {0};
        ppfOutputBufferIIRBlock[i] = new float[iNumSamples] {0};
        for (int sample = 0; sample < iNumSamples; sample++)
            ppfInputBuffer[i][sample] = static_cast<float>(sample);
    }


    CCombFilterIf* phCombFilter = 0;
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombFIR, 1, fSampleRate, iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 0.01);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);

    processBlock(ppfInputBuffer, ppfOutputBufferFIRBlock, phCombFilter, iNumChannels, 0, 1000);
    processBlock(ppfInputBuffer, ppfOutputBufferFIRBlock, phCombFilter, iNumChannels, 1000, 4000);
    processBlock(ppfInputBuffer, ppfOutputBufferFIRBlock, phCombFilter, iNumChannels, 5000, 5000);

    phCombFilter->reset();
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombIIR, 1, fSampleRate, iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 0.01);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);
    
    processBlock(ppfInputBuffer, ppfOutputBufferIIRBlock, phCombFilter, iNumChannels, 0, 1000);
    processBlock(ppfInputBuffer, ppfOutputBufferIIRBlock, phCombFilter, iNumChannels, 1000, 4000);
    processBlock(ppfInputBuffer, ppfOutputBufferIIRBlock, phCombFilter, iNumChannels, 5000, 5000);

    phCombFilter->reset();
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombFIR, 1, fSampleRate, iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 0.01);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);
    phCombFilter->process(ppfInputBuffer, ppfOutputBufferFIR, iNumSamples);

    phCombFilter->reset();
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombIIR, 1, fSampleRate, iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 0.01);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);
    phCombFilter->process(ppfInputBuffer, ppfOutputBufferIIR, iNumSamples);

    bool passed = true;
    for (int channel = 0; channel < iNumChannels; channel++)
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            //Calculates diff between block-by-block processing and all-at-once processing
            float firDiff = ppfOutputBufferFIR[channel][sample] - ppfOutputBufferFIRBlock[channel][sample];
            float iirDiff = ppfOutputBufferIIR[channel][sample] - ppfOutputBufferIIRBlock[channel][sample];
            if (firDiff != 0 || iirDiff != 0)
                passed = false;
        }

    //displayBuffer(ppfOutputBufferFIRBlock, iNumChannels, iNumSamples);
    //displayBuffer(ppfOutputBufferIIRBlock, iNumChannels, iNumSamples);
    CCombFilterIf::destroy(phCombFilter);
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        delete[] ppfInputBuffer[channel];
        delete[] ppfOutputBufferFIR[channel];
        delete[] ppfOutputBufferIIR[channel];
        delete[] ppfOutputBufferFIRBlock[channel];
        delete[] ppfOutputBufferIIRBlock[channel];
    }
    delete[] ppfInputBuffer;
    delete[] ppfOutputBufferFIR;
    delete[] ppfOutputBufferIIR;
    delete[] ppfOutputBufferFIRBlock;
    delete[] ppfOutputBufferIIRBlock;

    return passed;

}

//Test for zero input signal
//Output should be all zeros
bool test4()
{
    const int iNumChannels = 1;
    const int iNumSamples = 10000;
    const float fSampleRate = 44100;

    float** ppfInputBuffer = new float* [iNumChannels];
    float** ppfOutputBufferFIR = new float* [iNumChannels];
    float** ppfOutputBufferIIR = new float* [iNumChannels];
    for (int i = 0; i < iNumChannels; i++)
    {
        ppfInputBuffer[i] = new float[iNumSamples] {0};
        ppfOutputBufferFIR[i] = new float[iNumSamples] {0};
        ppfOutputBufferIIR[i] = new float[iNumSamples] {0};
    }

    CCombFilterIf* phCombFilter = 0;
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombFIR, 1, fSampleRate, iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 0.01);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);
    phCombFilter->process(ppfInputBuffer, ppfOutputBufferFIR, iNumSamples);

    phCombFilter->reset();
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombIIR, 1, fSampleRate, iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 0.01);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);
    phCombFilter->process(ppfInputBuffer, ppfOutputBufferIIR, iNumSamples);

    bool passed = true;
    for (int channel = 0; channel < iNumChannels; channel++)
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            //Ensures output values are zero
            float firValue = ppfOutputBufferFIR[channel][sample];
            float iirValue = ppfOutputBufferIIR[channel][sample];
            if (firValue != 0 || iirValue != 0)
                passed = false;
        }

    //displayBuffer(ppfOutputBufferFIR, iNumChannels, iNumSamples);
    //displayBuffer(ppfOutputBufferIIR, iNumChannels, iNumSamples);
    CCombFilterIf::destroy(phCombFilter);
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        delete[] ppfInputBuffer[channel];
        delete[] ppfOutputBufferFIR[channel];
        delete[] ppfOutputBufferIIR[channel];
    }
    delete[] ppfInputBuffer;
    delete[] ppfOutputBufferFIR;
    delete[] ppfOutputBufferIIR;

    return passed;
}

//Tests changing IIR parameter settings between process blocks
//Here, the delay parameter is changed from 6 samples to 8 samples. This will place the following delay value on sample 14 instead of sample 12.
//The gain value is also changed but it will be updated for the delay on sample 6 and all following delays.
bool test5()
{
    const int iNumChannels = 1;
    const int iNumSamples = 15;
    const float fSampleRate = 1;

    float** ppfInputBuffer = new float* [iNumChannels];
    float** ppfOutputBufferIIR = new float* [iNumChannels];
    for (int i = 0; i < iNumChannels; i++)
    {
        ppfInputBuffer[i] = new float[iNumSamples] {1};
        ppfOutputBufferIIR[i] = new float[iNumSamples] {0};
    }


    CCombFilterIf* phCombFilter = 0;
    CCombFilterIf::create(phCombFilter);
    phCombFilter->init(CCombFilterIf::CombFilterType_t::kCombIIR, 15, fSampleRate, iNumChannels);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 6);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);

    processBlock(ppfInputBuffer, ppfOutputBufferIIR, phCombFilter, iNumChannels, 0, 5);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 8);
    phCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.25);
    processBlock(ppfInputBuffer, ppfOutputBufferIIR, phCombFilter, iNumChannels, 5, 5);
    processBlock(ppfInputBuffer, ppfOutputBufferIIR, phCombFilter, iNumChannels, 10, 5);

    bool passed = true;
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        //Tests for expected values
        if (ppfOutputBufferIIR[channel][0] != 1)
            passed = false;
        else if (ppfOutputBufferIIR[channel][6] != 0.25)
            passed = false;
        else if (ppfOutputBufferIIR[channel][14] != 0.0625)
            passed = false;
    }

    //displayIOBuffers(ppfInputBuffer, ppfOutputBufferIIR, iNumChannels, iNumSamples);
    CCombFilterIf::destroy(phCombFilter);
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        delete[] ppfInputBuffer[channel];
        delete[] ppfOutputBufferIIR[channel];
    }
    delete[] ppfInputBuffer;
    delete[] ppfOutputBufferIIR;

    return passed;
}

