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
            m_pfInput = new float[m_iInputLength];
            m_pfIr = new float[m_iIRLength];
            m_pfOutput = new float[m_iInputLength + m_iIRLength];

            CVectorFloat::setZero(m_pfInput, m_iInputLength);
            m_pfInput[0] = 1;

            CSynthesis::generateNoise(m_pfIr, m_iIRLength);
            m_pfIr[0] = 1;

            CVectorFloat::setZero(m_pfOutput, m_iInputLength + m_iIRLength);

            m_pCFastConv = new CFastConv();
        }

        virtual void TearDown()
        {
            m_pCFastConv->reset();
            delete m_pCFastConv;

            delete[] m_pfIr;
            delete[] m_pfOutput;
            delete[] m_pfInput;
        }

        float *m_pfInput = 0;
        float *m_pfIr = 0;
        float *m_pfOutput = 0;

        int m_iInputLength = 83099;
        int m_iIRLength = 60001;

        CFastConv *m_pCFastConv = 0;
    };

    TEST_F(FastConv, Params)
    {
        EXPECT_EQ(false, Error_t::kNoError == m_pCFastConv->init(0, 1));
        EXPECT_EQ(false, Error_t::kNoError == m_pCFastConv->init(m_pfIr, 0));
        EXPECT_EQ(false, Error_t::kNoError == m_pCFastConv->init(m_pfIr, 10, -1));
        EXPECT_EQ(false, Error_t::kNoError == m_pCFastConv->init(m_pfIr, 10, 7));
        EXPECT_EQ(true, Error_t::kNoError == m_pCFastConv->init(m_pfIr, 10, 4));
        EXPECT_EQ(true, Error_t::kNoError == m_pCFastConv->reset());
    }

    TEST_F(FastConv, Impulse)
    {
        // impulse with impulse
        int iBlockLength = 4;
        m_pCFastConv->init(m_pfIr, 1, iBlockLength);

        for (auto i = 0; i < 500; i++)
            m_pCFastConv->process(&m_pfOutput[i], &m_pfInput[i], 1);

        EXPECT_NEAR(1.F, m_pfOutput[iBlockLength], 1e-6F);
        EXPECT_NEAR(0.F, CVectorFloat::getMin(m_pfOutput, m_iInputLength), 1e-6F);
        EXPECT_NEAR(1.F, CVectorFloat::getMax(m_pfOutput, m_iInputLength), 1e-6F);

        // impulse with dc
        for (auto i = 0; i < 4; i++)
            m_pfOutput[i] = 1;
        iBlockLength = 8;
        m_pCFastConv->init(m_pfOutput, 4, iBlockLength);

        for (auto i = 0; i < 500; i++)
            m_pCFastConv->process(&m_pfOutput[i], &m_pfInput[i], 1);

        EXPECT_NEAR(0.F, CVectorFloat::getMean(m_pfOutput, 8), 1e-6F);
        EXPECT_NEAR(1.F, CVectorFloat::getMean(&m_pfOutput[8], 4), 1e-6F);
        EXPECT_NEAR(0.F, CVectorFloat::getMean(&m_pfOutput[12], 400), 1e-6F);

        // impulse with noise
        iBlockLength = 8;
        m_pCFastConv->init(m_pfIr, 27, iBlockLength);

        for (auto i = 0; i < m_iInputLength; i++)
            m_pCFastConv->process(&m_pfOutput[i], &m_pfInput[i], 1);

        CHECK_ARRAY_CLOSE(m_pfIr, &m_pfOutput[iBlockLength], 27, 1e-6F);
        CHECK_ARRAY_CLOSE(&m_pfInput[1], &m_pfOutput[iBlockLength + 27], 10, 1e-6F);

        // noise with impulse
        iBlockLength = 8;
        m_pCFastConv->init(m_pfInput, 27, iBlockLength);

        for (auto i = 0; i < m_iIRLength; i++)
            m_pCFastConv->process(&m_pfOutput[i], &m_pfIr[i], 1);

        CHECK_ARRAY_CLOSE(m_pfIr, &m_pfOutput[iBlockLength], m_iIRLength - iBlockLength, 1e-6F);
    }
    TEST_F(FastConv, ImpulseTime)
    {
        // impulse with impulse
        int iBlockLength = 4;
        m_pCFastConv->init(m_pfIr, 1, iBlockLength, CFastConv::kTimeDomain);

        for (auto i = 0; i < 500; i++)
            m_pCFastConv->process(&m_pfOutput[i], &m_pfInput[i], 1);

        EXPECT_NEAR(1.F, m_pfOutput[0], 1e-6F);
        EXPECT_NEAR(0.F, CVectorFloat::getMin(m_pfOutput, m_iInputLength), 1e-6F);
        EXPECT_NEAR(1.F, CVectorFloat::getMax(m_pfOutput, m_iInputLength), 1e-6F);

        // impulse with dc
        for (auto i = 0; i < 4; i++)
            m_pfOutput[i] = 1;
        iBlockLength = 8;
        m_pCFastConv->init(m_pfOutput, 4, iBlockLength, CFastConv::kTimeDomain);

        for (auto i = 0; i < 500; i++)
            m_pCFastConv->process(&m_pfOutput[i], &m_pfInput[i], 1);

        EXPECT_NEAR(1.F, CVectorFloat::getMean(&m_pfOutput[0], 4), 1e-6F);
        EXPECT_NEAR(0.F, CVectorFloat::getMean(&m_pfOutput[4], 400), 1e-6F);

        // impulse with noise
        iBlockLength = 8;
        m_pCFastConv->init(m_pfIr, 27, iBlockLength, CFastConv::kTimeDomain);

        for (auto i = 0; i < m_iInputLength; i++)
            m_pCFastConv->process(&m_pfOutput[i], &m_pfInput[i], 1);

        CHECK_ARRAY_CLOSE(m_pfIr, &m_pfOutput[0], 27, 1e-6F);
        CHECK_ARRAY_CLOSE(&m_pfInput[1], &m_pfOutput[27], 10, 1e-6F);

        // noise with impulse
        iBlockLength = 8;
        m_pCFastConv->init(m_pfInput, 27, iBlockLength, CFastConv::kTimeDomain);

        for (auto i = 0; i < m_iIRLength; i++)
            m_pCFastConv->process(&m_pfOutput[i], &m_pfIr[i], 1);

        CHECK_ARRAY_CLOSE(m_pfIr, &m_pfOutput[0], m_iIRLength , 1e-6F);
    }

    TEST_F(FastConv, BlockLengths)
    {
        // impulse with noise
        int iBlockLength = 4;

        for (auto j = 0; j < 10; j++)
        {
            m_pCFastConv->init(m_pfIr, 51, iBlockLength);

            for (auto i = 0; i < m_iInputLength; i++)
                m_pCFastConv->process(&m_pfOutput[i], &m_pfInput[i], 1);

            CHECK_ARRAY_CLOSE(m_pfIr, &m_pfOutput[iBlockLength], 51 - iBlockLength, 1e-6F);

            iBlockLength <<= 1;
        }
    }

    TEST_F(FastConv, InputLengths)
    {
        // impulse with noise
        int iBlockLength = 4096;

        int iCurrIdx = 0,
            aiInputLength[] = {
            4095,
            17,
            32157,
            99,
            4097,
            1,
            42723

        };

        m_pCFastConv->init(m_pfIr, m_iIRLength, iBlockLength);

        for (auto i = 0; i < 7; i++)
        {
            m_pCFastConv->process(&m_pfOutput[iCurrIdx], &m_pfInput[iCurrIdx], aiInputLength[i]);
            iCurrIdx += aiInputLength[i];
        }

        CHECK_ARRAY_CLOSE(m_pfIr, &m_pfOutput[iBlockLength], m_iIRLength, 1e-6F);
        EXPECT_NEAR(0.F, CVectorFloat::getMean(&m_pfOutput[m_iIRLength + iBlockLength], 10000), 1e-6F);

    }

    TEST_F(FastConv, FlushBuffer)
    {
        // impulse with noise
        int iBlockLength = 8;
        int iIrLength = 27;

        CVectorFloat::setZero(m_pfOutput, m_iInputLength + m_iIRLength);
        m_pCFastConv->init(m_pfIr, iIrLength, iBlockLength);

        m_pCFastConv->process(m_pfOutput, m_pfInput, 1);

        m_pCFastConv->flushBuffer(&m_pfOutput[1]);

        EXPECT_NEAR(0.F, CVectorFloat::getMean(m_pfOutput, iBlockLength), 1e-6F);
        CHECK_ARRAY_CLOSE(m_pfIr, &m_pfOutput[iBlockLength], iIrLength, 1e-6F);

        // same for time domain
        CVectorFloat::setZero(m_pfOutput, m_iInputLength + m_iIRLength);
        m_pCFastConv->init(m_pfIr, iIrLength, iBlockLength,CFastConv::kTimeDomain);

        m_pCFastConv->process(m_pfOutput, m_pfInput, 1);

        m_pCFastConv->flushBuffer(&m_pfOutput[1]);

        CHECK_ARRAY_CLOSE(m_pfIr, &m_pfOutput[0], iIrLength, 1e-6F);
    }
}

#endif //WITH_TESTS



//#include "MUSI6106Config.h"
//
//#ifdef WITH_TESTS
//#include "Synthesis.h"
//
//#include "Vector.h"
//#include "FastConv.h"
//
//#include "gtest/gtest.h"
//
//#include <chrono>
//
//
//namespace fastconv_test {
//    void CHECK_ARRAY_CLOSE(float* buffer1, float* buffer2, int iLength, float fTolerance)
//    {
//        for (int i = 0; i < iLength; i++)
//        {
//            EXPECT_NEAR(buffer1[i], buffer2[i], fTolerance);
//        }
//    }
//
//    class FastConv: public testing::Test
//    {
//    protected:
//        void SetUp() override
//        {
//            srand(time(0));
//            m_pCFastConv = new CFastConv();
//        }
//
//        virtual void TearDown()
//        {
//            m_pCFastConv->reset();
//            delete m_pCFastConv;
//            m_pCFastConv = 0;
//
//            if (m_pfInput)
//            {
//                delete[] m_pfInput;
//                m_pfInput = 0;
//            }
//            if (m_pfIr)
//            {
//                delete[] m_pfIr;
//                m_pfIr = 0;
//            }
//            if (m_pfTestOutput)
//            {
//                delete[] m_pfTestOutput;
//                m_pfTestOutput = 0;
//            }
//            if (m_pfTestTail)
//            {
//                delete[] m_pfTestTail;
//                m_pfTestTail = 0;
//            }
//            if (m_pfGroundOutput)
//            {
//                delete[] m_pfGroundOutput;
//                m_pfGroundOutput = 0;
//            }
//            if (m_pfGroundTail)
//            {
//                delete[] m_pfGroundTail;
//                m_pfGroundTail = 0;
//            }
//        }
//
//        CFastConv* m_pCFastConv = 0;
//        float* m_pfInput = 0;
//        float* m_pfIr = 0;
//        float* m_pfTestOutput = 0;
//        float* m_pfTestTail = 0;
//        float* m_pfGroundOutput = 0;
//        float* m_pfGroundTail = 0;
//    };
//
//    TEST_F(FastConv, Identity_Time)
//    {
//
//        int iInputLength = 10;
//        int iIrLength = 51;
//        int iOutputLength = iInputLength;
//
//        m_pfInput = new float[iInputLength] {};
//        m_pfIr = new float[iIrLength] {};
//        m_pfTestOutput = new float[iOutputLength] {};
//        m_pfGroundOutput = new float[iOutputLength] {};
//
//        m_pfInput[3] = 1;
//        for (int i = 0; i < iIrLength; i++)
//            m_pfIr[i] = static_cast<float>(rand() % 10);
//        CVectorFloat::copy(m_pfGroundOutput + 3, m_pfIr, iOutputLength - 3);
//
//        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//
//        m_pCFastConv->init(m_pfIr, iIrLength, 8192, CFastConv::ConvCompMode_t::kTimeDomain);
//        m_pCFastConv->process(m_pfTestOutput, m_pfInput, iInputLength);
//        m_pCFastConv->reset();
//
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//
//        std::cout << "IdentityTime = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " micro s" << std::endl;
//
//        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iOutputLength, 0);
//    }
//
//    TEST_F(FastConv, FlushBuffer_Time)
//    {
//
//        int iInputLength = 10;
//        int iIrLength = 51;
//        int iOutputLength = iInputLength;
//        int iTailLength = iIrLength - 1;
//
//        m_pfInput = new float[iInputLength] {};
//        m_pfIr = new float[iIrLength] {};
//        m_pfTestOutput = new float[iOutputLength] {};
//        m_pfGroundOutput = new float[iOutputLength] {};
//        m_pfGroundTail = new float[iTailLength] {};
//
//        m_pfInput[3] = 1;
//        for (int i = 0; i < iIrLength; i++)
//            m_pfIr[i] = static_cast<float>(rand() % 10);
//        CVectorFloat::copy(m_pfGroundOutput + 3, m_pfIr, iOutputLength - 3);
//        CVectorFloat::copy(m_pfGroundTail, m_pfIr + 7, iIrLength - 7);
//
//        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//        m_pCFastConv->init(m_pfIr, iIrLength, 8192, CFastConv::ConvCompMode_t::kTimeDomain);
//        m_pCFastConv->process(m_pfTestOutput, m_pfInput, iInputLength);
//
//        m_pfTestTail = new float[m_pCFastConv->getTailLength()]{};
//        m_pCFastConv->flushBuffer(m_pfTestTail);
//        m_pCFastConv->reset();
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//
//        std::cout << "FlushBufferTime = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " micro s" << std::endl;
//
//        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iOutputLength, 0);
//        CHECK_ARRAY_CLOSE(m_pfGroundTail, m_pfTestTail, iTailLength, 0);
//    }
//
//    TEST_F(FastConv, BlockSize_Time)
//    {
//        int iInputLength = 10000;
//        int iIrLength = 10000;
//        int iTailLength = iIrLength - 1;
//        int blockSizes[8]{ 1, 13, 1023, 2048, 1, 17, 5000, 1897 };
//
//        m_pfInput = new float[iInputLength] {};
//        m_pfIr = new float[iIrLength] {};
//        m_pfTestOutput = new float[iInputLength] {};
//        m_pfGroundOutput = new float[iInputLength] {};
//        m_pfGroundTail = new float[iTailLength] {};
//
//        m_pfInput[6] = 1;
//        for (int i = 0; i < iIrLength; i++)
//            m_pfIr[i] = static_cast<float>(rand() % 10);
//        CVectorFloat::copy(m_pfGroundOutput + 6, m_pfIr, iInputLength - 6);
//        CVectorFloat::copy(m_pfGroundTail, m_pfIr + 9994, iIrLength - 9994);
//
//        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//
//        m_pCFastConv->init(m_pfIr, iIrLength, 8192, CFastConv::ConvCompMode_t::kTimeDomain);
//        int iOffset = 0;
//        for (int blockSize : blockSizes)
//        {
//            m_pCFastConv->process(m_pfTestOutput + iOffset, m_pfInput + iOffset, blockSize);
//            iOffset += blockSize;
//        }
//
//        EXPECT_EQ(iTailLength, m_pCFastConv->getTailLength());
//        m_pfTestTail = new float[m_pCFastConv->getTailLength()]{};
//        m_pCFastConv->flushBuffer(m_pfTestTail);
//        m_pCFastConv->reset();
//
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//        std::cout << "BlockSizeTime = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " micro s" << std::endl;
//
//        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iInputLength, 0);
//        CHECK_ARRAY_CLOSE(m_pfGroundTail, m_pfTestTail, iTailLength, 0);
//    }
//
//    TEST_F(FastConv, Identity_Freq)
//    {
//
//        int iInputLength = 10;
//        int iIrLength = 51;
//        int iOutputLength = iInputLength;
//
//        m_pfInput = new float[iInputLength] {};
//        m_pfIr = new float[iIrLength] {};
//        m_pfTestOutput = new float[iOutputLength] {};
//        m_pfGroundOutput = new float[iOutputLength] {};
//
//        m_pfInput[3] = 1;
//        for (int i = 0; i < iIrLength; i++)
//            m_pfIr[i] = static_cast<float>(rand() % 10);
//        CVectorFloat::copy(m_pfGroundOutput + 3, m_pfIr, iOutputLength - 3);
//
//        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//
//        m_pCFastConv->init(m_pfIr, iIrLength, 8, CFastConv::ConvCompMode_t::kFreqDomain);
//        m_pCFastConv->process(m_pfTestOutput, m_pfInput, iInputLength);
//        m_pCFastConv->reset();
//
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//        std::cout << "IdentityFreq = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " micro s" << std::endl;
//
//        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iOutputLength, 1E-3);
//    }
//
//    TEST_F(FastConv, FlushBuffer_Freq)
//    {
//
//        int iInputLength = 10;
//        int iIrLength = 51;
//        int iOutputLength = iInputLength;
//        int iTailLength = iIrLength - 1;
//
//        m_pfInput = new float[iInputLength] {};
//        m_pfIr = new float[iIrLength] {};
//        m_pfTestOutput = new float[iOutputLength] {};
//        m_pfGroundOutput = new float[iOutputLength] {};
//        m_pfGroundTail = new float[iTailLength] {};
//
//        m_pfInput[3] = 1;
//        for (int i = 0; i < iIrLength; i++)
//            m_pfIr[i] = static_cast<float>(rand() % 10);
//        CVectorFloat::copy(m_pfGroundOutput + 3, m_pfIr, iOutputLength - 3);
//        CVectorFloat::copy(m_pfGroundTail, m_pfIr + 7, iIrLength - 7);
//
//        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//
//        m_pCFastConv->init(m_pfIr, iIrLength, 8, CFastConv::ConvCompMode_t::kFreqDomain);
//        m_pCFastConv->process(m_pfTestOutput, m_pfInput, iInputLength);
//
//
//        m_pfTestTail = new float[m_pCFastConv->getTailLength()]{};
//        m_pCFastConv->flushBuffer(m_pfTestTail);
//        m_pCFastConv->reset();
//
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//        std::cout << "FlushBufferFreq = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " micro s" << std::endl;
//
//        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iOutputLength, 1E-3);
//        CHECK_ARRAY_CLOSE(m_pfGroundTail, m_pfTestTail, iTailLength, 1E-3);
//    }
//
//    TEST_F(FastConv, BlockSize_Freq)
//    {
//        int iInputLength = 10000;
//        int iIrLength = 10000;
//        int iTailLength = iIrLength - 1;
//        int blockSizes[8]{ 1, 13, 1023, 2048, 1, 17, 5000, 1897 };
//
//        m_pfInput = new float[iInputLength] {};
//        m_pfIr = new float[iIrLength] {};
//        m_pfTestOutput = new float[iInputLength] {};
//        m_pfGroundOutput = new float[iInputLength] {};
//        m_pfGroundTail = new float[iTailLength] {};
//
//        m_pfInput[6] = 1;
//        for (int i = 0; i < iIrLength; i++)
//            m_pfIr[i] = static_cast<float>(rand() % 10);
//        CVectorFloat::copy(m_pfGroundOutput + 6, m_pfIr, iInputLength - 6);
//        CVectorFloat::copy(m_pfGroundTail, m_pfIr + 9994, iIrLength - 9994);
//
//        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//
//        m_pCFastConv->init(m_pfIr, iIrLength, 8192, CFastConv::ConvCompMode_t::kFreqDomain);
//        int iOffset = 0;
//        for (int blockSize : blockSizes)
//        {
//            m_pCFastConv->process(m_pfTestOutput + iOffset, m_pfInput + iOffset, blockSize);
//            iOffset += blockSize;
//        }
//
//        m_pfTestTail = new float[m_pCFastConv->getTailLength()]{};
//        m_pCFastConv->flushBuffer(m_pfTestTail);
//        m_pCFastConv->reset();
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//        std::cout << "BlockSizeFreq = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " micro s" << std::endl;
//
//        CHECK_ARRAY_CLOSE(m_pfGroundOutput, m_pfTestOutput, iInputLength, 1E-3);
//        CHECK_ARRAY_CLOSE(m_pfGroundTail, m_pfTestTail, iTailLength, 1E-3);
//    }
//}
//
//#endif //WITH_TESTS
//
