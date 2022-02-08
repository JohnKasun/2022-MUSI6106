
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();
void    displayIOBuffers(float**, float**, int, int);

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    //std::string sInputFilePath,                 //!< file paths
    //            sOutputFilePath;

    //static const int kBlockSize = 1024;

    //clock_t time = 0;

    //float **ppfAudioData = 0;

    //CAudioFileIf *phAudioFile = 0;
    //std::fstream hOutputFile;
    //CAudioFileIf::FileSpec_t stFileSpec;

    //showClInfo();

    ////////////////////////////////////////////////////////////////////////////////
    //// parse command line arguments
    //if (argc < 2)
    //{
    //    cout << "Missing audio input path!";
    //    return -1;
    //}
    //else
    //{
    //    sInputFilePath = argv[1];
    //    sOutputFilePath = sInputFilePath + ".txt";
    //}

    ////////////////////////////////////////////////////////////////////////////////
    //// open the input wave file
    //CAudioFileIf::create(phAudioFile);
    //phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    //if (!phAudioFile->isOpen())
    //{
    //    cout << "Wave file open error!";
    //    CAudioFileIf::destroy(phAudioFile);
    //    return -1;
    //}
    //phAudioFile->getFileSpec(stFileSpec);

    ////////////////////////////////////////////////////////////////////////////////
    //// open the output text file
    //hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
    //if (!hOutputFile.is_open())
    //{
    //    cout << "Text file open error!";
    //    CAudioFileIf::destroy(phAudioFile);
    //    return -1;
    //}

    ////////////////////////////////////////////////////////////////////////////////
    //// allocate memory
    //ppfAudioData = new float*[stFileSpec.iNumChannels];
    //for (int i = 0; i < stFileSpec.iNumChannels; i++)
    //    ppfAudioData[i] = new float[kBlockSize];

    //if (ppfAudioData == 0)
    //{
    //    CAudioFileIf::destroy(phAudioFile);
    //    hOutputFile.close();
    //    return -1;
    //}
    //if (ppfAudioData[0] == 0)
    //{
    //    CAudioFileIf::destroy(phAudioFile);
    //    hOutputFile.close();
    //    return -1;
    //}

    //time = clock();

    ////////////////////////////////////////////////////////////////////////////////
    //// get audio data and write it to the output text file (one column per channel)
    //while (!phAudioFile->isEof())
    //{
    //    // set block length variable
    //    long long iNumFrames = kBlockSize;

    //    // read data (iNumOfFrames might be updated!)
    //    phAudioFile->readData(ppfAudioData, iNumFrames);

    //    cout << "\r" << "reading and writing";

    //    // write
    //    for (int i = 0; i < iNumFrames; i++)
    //    {
    //        for (int c = 0; c < stFileSpec.iNumChannels; c++)
    //        {
    //            hOutputFile << ppfAudioData[c][i] << "\t";
    //        }
    //        hOutputFile << endl;
    //    }
    //}

    //cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;

    ////////////////////////////////////////////////////////////////////////////////
    //// clean-up (close files and free memory)
    //CAudioFileIf::destroy(phAudioFile);
    //hOutputFile.close();

    //for (int i = 0; i < stFileSpec.iNumChannels; i++)
    //    delete[] ppfAudioData[i];
    //delete[] ppfAudioData;
    //ppfAudioData = 0;



    int iNumChannels = 1;
    int iNumSamples = 19;
    float **ppfAudioInputBuffer = 0;
    float** ppfAudioOutputBuffer = 0;
    ppfAudioInputBuffer = new float* [iNumChannels];
    ppfAudioOutputBuffer = new float* [iNumChannels];
    for (int channel = 0; channel < iNumChannels; channel++)
    {
        ppfAudioInputBuffer[channel] = new float[iNumSamples] {};
        ppfAudioOutputBuffer[channel] = new float[iNumSamples] {};
        //ppfAudioInputBuffer[channel][0] = 1;
    }


    for (int channel = 0; channel < iNumChannels; channel++)
    {
        for (int sample = 0; sample < iNumSamples; sample++)
        {
            ppfAudioInputBuffer[channel][sample] = static_cast<float>(sample + 1);
        }
    }


    CCombFilterIf* pCombFilter = 0;
    CCombFilterIf::create(pCombFilter);
    pCombFilter->init(CCombFilterIf::CombFilterType_t::kCombFIR, 20, 1.0f, iNumChannels);
    pCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5f);
    pCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 17);

    pCombFilter->process(ppfAudioInputBuffer, ppfAudioOutputBuffer, iNumSamples);
    displayIOBuffers(ppfAudioInputBuffer, ppfAudioOutputBuffer, iNumChannels, iNumSamples);
    //pCombFilter->process(ppfAudioInputBuffer, ppfAudioOutputBuffer, iNumSamples);
    //displayIOBuffers(ppfAudioInputBuffer, ppfAudioOutputBuffer, iNumChannels, iNumSamples);
    //pCombFilter->process(ppfAudioInputBuffer, ppfAudioOutputBuffer, iNumSamples);
    //displayIOBuffers(ppfAudioInputBuffer, ppfAudioOutputBuffer, iNumChannels, iNumSamples);

    for (int i = 0; i < iNumChannels; i++)
    {
        delete[] ppfAudioInputBuffer[i];
        delete[] ppfAudioOutputBuffer[i];
    }
    delete[] ppfAudioOutputBuffer;
    delete[] ppfAudioInputBuffer;
    ppfAudioInputBuffer = 0;
    ppfAudioOutputBuffer = 0;


    // all done
    return 0;

}

void displayIOBuffers(float** ppfAudioInputBuffer, float** ppfAudioOutputBuffer, int iNumChannels, int iNumSamples)
{
    std::cout << "=======================================" << std::endl;
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

void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

