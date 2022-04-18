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

    TEST_F(FastConv, EmptyTest)
    {
    }
}

#endif //WITH_TESTS

