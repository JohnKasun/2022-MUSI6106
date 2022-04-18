#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include "Synthesis.h"

#include "Vector.h"
#include "FastConv.h"

#include "gtest/gtest.h"


namespace fastconv_test {
    void CHECK_ARRAY_CLOSE(float* buffer1, float* buffer2, int iLength, float fTolerance)
    {
        for (int i = 0; i < iLength; i++)
        {
            EXPECT_NEAR(buffer1[i], buffer2[i], fTolerance);
        }
    }

    class FastConv: public testing::Test
    {
    protected:
        void SetUp() override
        {
            srand(time(0));
            m_pCFastConv = new CFastConv();
        }

        virtual void TearDown()
        {
            m_pCFastConv->reset();
            delete m_pCFastConv;
            m_pCFastConv = 0;

            if (m_pfInput)
            {
                delete[] m_pfInput;
                m_pfInput = 0;
            }
            if (m_pfIr)
            {
                delete[] m_pfIr;
                m_pfIr = 0;
            }
            if (m_pfTestOutput)
            {
                delete[] m_pfTestOutput;
                m_pfTestOutput = 0;
            }
            if (m_pfTestTail)
            {
                delete[] m_pfTestTail;
                m_pfTestTail = 0;
            }
            if (m_pfGroundOutput)
            {
                delete[] m_pfGroundOutput;
                m_pfGroundOutput = 0;
            }
            if (m_pfGroundTail)
            {
                delete[] m_pfGroundTail;
                m_pfGroundTail = 0;
            }
        }

        CFastConv* m_pCFastConv = 0;
        float* m_pfInput = 0;
        float* m_pfIr = 0;
        float* m_pfTestOutput = 0;
        float* m_pfTestTail = 0;
        float* m_pfGroundOutput = 0;
        float* m_pfGroundTail = 0;
    };

    TEST_F(FastConv, Identity)
    {

        int iInputLength = 52;
        int iIrLength = 51;
        int iOutputLength = iInputLength;

        m_pfInput = new float[iInputLength] {};
        m_pfIr = new float[iIrLength] {};
        m_pfTestOutput = new float[iOutputLength] {};
        m_pfGroundOutput = new float[iOutputLength] {};

        m_pfInput[3] = 1;
        for (int i = 0; i < iIrLength; i++)
            m_pfIr[i] = static_cast<float>(rand() % 10);
        CVectorFloat::copy(m_pfGroundOutput + 3, m_pfIr, iOutputLength - 3);

        m_pCFastConv->init(m_pfIr, iIrLength, 8192, CFastConv::ConvCompMode_t::kTimeDomain);
        m_pCFastConv->process(m_pfTestOutput, m_pfInput, iInputLength);
        m_pCFastConv->reset();

        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iOutputLength, 0);
    }

    TEST_F(FastConv, FlushBuffer)
    {

        int iInputLength = 10;
        int iIrLength = 51;
        int iOutputLength = iInputLength;
        int iTailLength = iIrLength - 1;

        m_pfInput = new float[iInputLength] {};
        m_pfIr = new float[iIrLength] {};
        m_pfTestOutput = new float[iOutputLength] {};
        m_pfGroundOutput = new float[iOutputLength] {};
        m_pfGroundTail = new float[iTailLength] {};

        m_pfInput[3] = 1;
        for (int i = 0; i < iIrLength; i++)
            m_pfIr[i] = static_cast<float>(rand() % 10);
        CVectorFloat::copy(m_pfGroundOutput + 3, m_pfIr, iOutputLength - 3);
        CVectorFloat::copy(m_pfGroundTail, m_pfIr + 7, iIrLength - 7);

        m_pCFastConv->init(m_pfIr, iIrLength, 8192, CFastConv::ConvCompMode_t::kTimeDomain);
        m_pCFastConv->process(m_pfTestOutput, m_pfInput, iInputLength);

        m_pfTestTail = new float[m_pCFastConv->getTailLength()]{};
        m_pCFastConv->flushBuffer(m_pfTestTail);
        m_pCFastConv->reset();

        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iOutputLength, 0);
        CHECK_ARRAY_CLOSE(m_pfGroundTail, m_pfTestTail, iTailLength, 0);
    }

    TEST_F(FastConv, BlockSize)
    {
        int iInputLength = 10000;
        int iIrLength = 10000;
        int iTailLength = iIrLength - 1;
        int blockSizes[8]{ 1, 13, 1023, 2048, 1, 17, 5000, 1897 };

        m_pfInput = new float[iInputLength] {};
        m_pfIr = new float[iIrLength] {};
        m_pfTestOutput = new float[iInputLength] {};
        m_pfGroundOutput = new float[iInputLength] {};
        m_pfGroundTail = new float[iTailLength] {};

        m_pfInput[6] = 1;
        for (int i = 0; i < iIrLength; i++)
            m_pfIr[i] = static_cast<float>(rand() % 10);
        CVectorFloat::copy(m_pfGroundOutput + 6, m_pfIr, iInputLength - 6);
        CVectorFloat::copy(m_pfGroundTail, m_pfIr + 9994, iIrLength - 9994);

        m_pCFastConv->init(m_pfIr, iIrLength, 8192, CFastConv::ConvCompMode_t::kTimeDomain);
        int iOffset = 0;
        for (int blockSize : blockSizes)
        {
            m_pCFastConv->process(m_pfTestOutput + iOffset, m_pfInput + iOffset, blockSize);
            iOffset += blockSize;
        }

        EXPECT_EQ(iTailLength, m_pCFastConv->getTailLength());
        m_pfTestTail = new float[m_pCFastConv->getTailLength()]{};
        m_pCFastConv->flushBuffer(m_pfTestTail);
        m_pCFastConv->reset();

        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iInputLength, 0);
       // CHECK_ARRAY_CLOSE(m_pfGroundTail, m_pfTestTail, iTailLength, 0);
    }
}

#endif //WITH_TESTS

