
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "RingBuffer.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    CRingBuffer<float>* pCRingBuff = 0; 
    
    static const int kBlockSize = 17;

    showClInfo();

    pCRingBuff = new CRingBuffer<float>(kBlockSize);

    for (int i = 0; i < kBlockSize; i++)
    {
        pCRingBuff->putPostInc(1.F*i);
    }

    //for (int i = 5; i < 30; i++)
    //{
    //    pCRingBuff->getNumValuesInBuffer(); // should be five
    //    pCRingBuff->getPostInc(); // should be i-5
    //    pCRingBuff->putPostInc(1.F*i);
    //}

    pCRingBuff->setReadIdx(4);
    pCRingBuff->get(-4);

    // all done
    return 0;

}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}
