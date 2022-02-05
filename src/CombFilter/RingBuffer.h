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
        m_iEndPosition(iBufferLengthInSamples)
    {
        assert(iBufferLengthInSamples > 0);

        m_buffer = new T[iBufferLengthInSamples]{};
    }

    virtual ~CRingBuffer()
    {
        delete m_buffer;
    }

    /*! add a new value of type T to write index and increment write index
    \param tNewValue the new value
    \return void
    */
    void putPostInc(T tNewValue)
    {
        put(tNewValue);
        m_iWritePosition++;
        m_iWritePosition %= m_iEndPosition;
    }

    /*! add a new value of type T to write index
    \param tNewValue the new value
    \return void
    */
    void put(T tNewValue)
    {
        m_buffer[m_iWritePosition] = tNewValue;
        if (m_numValues < m_iEndPosition)
            m_numValues++;
    }

    /*! return the value at the current read index and increment the read pointer
    \return float the value from the read index
    */
    T getPostInc()
    {
        T value = get();
        m_iReadPosition++;
        m_iReadPosition %= m_iEndPosition;
        return value;
    }

    /*! return the value at the current read index
    \return float the value from the read index
    */
    T get(int iOffset = 0) const
    {
        int readPosition = (m_iReadPosition + iOffset) % m_iEndPosition;
        return m_buffer[readPosition];
    }

    /*! set buffer content and indices to 0
    \return void
    */
    void reset()
    {
        m_iReadPosition = 0;
        m_iWritePosition = 0;
        m_numValues = 0;
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
        assert(iNewWriteIdx >= 0);
        m_iWritePosition = (iNewWriteIdx % m_iEndPosition);
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
        assert(iNewReadIdx >= 0);
        m_iReadPosition = (iNewReadIdx % m_iEndPosition);
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    int getNumValuesInBuffer() const
    {
        return m_numValues;
    }

    /*! returns the length of the internal buffer
    \return int
    */
    int getTotalLength() const
    {
        return m_iBuffLength;
    }

    /* returns the length of the buffer section actively being used -- length between index 0 and (m_iEndPosition - 1)
    \return int
    */
    int getCurrentLength() const
    {
        return m_iEndPosition;
    }

    /* sets the wraparound position
    \return void
    */
    void setEndPosition(int iEndPosition)
    {
        assert(iEndPosition <= m_iBuffLength);
        m_iEndPosition = iEndPosition;
    }

    /* returns the value at the end of buffer section actively being used
    \return int
    */
    float getBack() const
    {
        return m_buffer[m_iEndPosition - 1];
    }

    void display()
    {
        setReadIdx(0);
        std::cout << "[ ";
        for (int i = 0; i < getCurrentLength(); i++)
        {
            std::cout << getPostInc() << " ";
        }
        std::cout << "]" << std::endl;
    }

private:
    CRingBuffer();
    CRingBuffer(const CRingBuffer& that);

    int m_iBuffLength;              //!< length of the internal buffer
    int m_iEndPosition;
    T* m_buffer = 0;
    int m_iReadPosition = 0;
    int m_iWritePosition = 0;
    int m_numValues = 0;
};
#endif // __RingBuffer_hdr__
