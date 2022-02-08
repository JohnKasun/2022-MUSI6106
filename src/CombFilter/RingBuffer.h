#if !defined(__RingBuffer_hdr__)
#define __RingBuffer_hdr__

#include <cassert>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

/*! \brief implement a circular buffer of type T
*/
template <class T>
class CRingBuffer
{
public:
    explicit CRingBuffer(int iBufferLengthInSamples) :
        m_iBuffLength(iBufferLengthInSamples),
        m_iReadPosition(0),
        m_iWritePosition(0)
    {
        assert(iBufferLengthInSamples > 0);

        m_buffer = new T[iBufferLengthInSamples]{};
    }

    virtual ~CRingBuffer()
    {
        delete[] m_buffer;
        m_buffer = 0;
    }

    /*! add a new value of type T to write index and increment write index
    \param tNewValue the new value
    \return void
    */
    void putPostInc(T tNewValue)
    {
        put(tNewValue);
        m_iWritePosition = wrapAround(++m_iWritePosition);
    }

    /*! add a new value of type T to write index
    \param tNewValue the new value
    \return void
    */
    void put(T tNewValue)
    {
        m_buffer[m_iWritePosition] = tNewValue;
    }

    /*! return the value at the current read index and increment the read pointer
    \return float the value from the read index
    */
    T getPostInc()
    {
        T value = get();
        m_iReadPosition = wrapAround(++m_iReadPosition);
        return value;
    }

    /*! return the value at the current read index
    \return float the value from the read index
    */
    T get(int iOffset = 0) const
    {
        return m_buffer[wrapAround(m_iReadPosition + iOffset)];
    }

    /*! set buffer content and indices to 0
    \return void
    */
    void reset()
    {
        m_iReadPosition = 0;
        m_iWritePosition = 0;
        for (int i = 0; i < m_iBuffLength; i++)
            m_buffer[i] = 0;
    }

    /*! return the current index for writing/put
    \return int
    */
    int getWriteIdx() const
    {
        return m_iWritePosition;
    }

    /*! move the write index to a new position
    \param iNewWriteIdx: new position
    \return void
    */
    void setWriteIdx(int iNewWriteIdx)
    {
        //assert(iNewWriteIdx >= 0 && iNewWriteIdx < m_iDelayLength);
        m_iWritePosition = handleIndexBounds(iNewWriteIdx);
    }

    /*! return the current index for reading/get
    \return int
    */
    int getReadIdx() const
    {
        return m_iReadPosition;
    }

    /*! move the read index to a new position
    \param iNewReadIdx: new position
    \return void
    */
    void setReadIdx(int iNewReadIdx)
    {
        //assert(iNewReadIdx >= 0 && iNewReadIdx < m_iDelayLength);
        m_iReadPosition = handleIndexBounds(iNewReadIdx);
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    int getNumValuesInBuffer() const
    {
        return wrapAround(m_iWritePosition - m_iReadPosition);
    }

    /*! returns the length of the internal buffer
    \return int
    */
    int getLength() const
    {
        return m_iBuffLength;
    }

    /* displays contents of buffer
    \return void
    */
    void display()
    {
        std::cout << "[ ";
        for (int i = 0; i < m_iBuffLength; i++)
        {
            std::cout << m_buffer[i] << " ";
        }
        std::cout << "]";

        std::cout << " Number of Values: " << getNumValuesInBuffer() << std::endl;
    }

private:
    CRingBuffer();
    CRingBuffer(const CRingBuffer& that);

    int m_iBuffLength;              //!< length of the internal buffer
    int m_iReadPosition;
    int m_iWritePosition;
    T* m_buffer = 0;

    int wrapAround(int iValue) const
    {
        while (iValue < 0)
            iValue += m_iBuffLength;
        iValue %= m_iBuffLength;
        return iValue;
    }

    int handleIndexBounds(int iValue) const
    {
        if (iValue < 0)
            iValue = 0;
        else if (iValue >= m_iBuffLength)
            iValue = m_iBuffLength - 1;
        return iValue;
    }
};
#endif // __RingBuffer_hdr__
