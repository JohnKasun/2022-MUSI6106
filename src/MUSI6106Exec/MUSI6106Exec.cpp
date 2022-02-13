
#include <iostream>
#include <ctime>
#include <cctype>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();
void    displayIOBuffers(float**, float**, int, int);
void    toLower(std::string&);

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
        cout << "Missing audio input path!";
        return -1;
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
            eFilterType == CCombFilterIf::CombFilterType_t::kCombIIR;

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

void displayIOBuffers(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumChannels, int iNumSamples)
{
    static int iNumBlock = 0;

    std::cout << "===Block " << ++iNumBlock << "=============================" << std::endl;
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

void toLower(std::string &sStringToLower)
{
    for (char& letter : sStringToLower)
        tolower(letter);
}

void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

