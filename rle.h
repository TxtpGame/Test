#pragma once
#include <cstring>

/// RLE(��)ѹ��,SizeΪ���ݵ�Ԫ����С����
template <size_t Size = 1>
class Rle {
    unsigned char* _buf;        /// ��ʱ������
    size_t _cap;        /// ����������(�ֽ���)
protected:
    enum { MaxRepeatCount = 0x80 };
public:
    Rle() : _buf(0), _cap(0) { static_assert(Size > 0, "Rle template parameter must be greater than 0"); }
    ~Rle() { delete[] _buf; }

public:
    /// ��ȡ�������Ŀ�껺����
    const unsigned char* data() const { return _buf; }

    /// ��ȡ����������
    size_t capacity() const { return _cap; }

    /** ѹ��(��־�����봫������ݵ��ֽ�����ͬ)
    @param src    Դ������
    @param bytes    Դ���������ݳ���(�ֽ���)
    @retval            �ɹ�����ѹ��������ݳ���(�ֽ���)�����򷵻�0
    @remark            �洢��ʽ��...;n-1;n����ֵͬa1;a2;...;an;k+127;k����ֵͬ;b;...
    */
    size_t compress(const void* src, size_t bytes) {
        if (!src || bytes == 0 || (bytes % Size) != 0) // ����У��
            return 0;

        size_t sameDataRepeatCount = 0;            // ��ͬ�����������ֵĴ���
        size_t differentDataRepeatCount = 0;    // ��ͬ�����������ֵĴ���
        size_t pos = 0;                            // д��rle�����λ��
        size_t count = bytes / Size;            // Դ���ݵĸ���
        unsigned char* cur = (unsigned char*)src;
        unsigned char* from = cur;

        if (count == 1) // ֻ��һ������
        {
            writeSameData(pos, 1, from);
        } else {
            for (size_t i = 1; i < count; i++) {
                if (ElemTraits<Size>::equalto(cur, cur + Size)) // ����������ͬ
                {
                    if (differentDataRepeatCount > 0) // д��ǰ��������ͬ������
                    {
                        writeDifferentData(pos, differentDataRepeatCount, from);
                        from = cur;
                        differentDataRepeatCount = 0;
                        sameDataRepeatCount = 1;
                    } else   // ������ͬ��������Ŀ��1
                    {
                        sameDataRepeatCount++;
                    }
                } else   // �������ݲ���ͬ
                {
                    if (sameDataRepeatCount > 0) // д��ǰ��������ͬ������
                    {
                        writeSameData(pos, sameDataRepeatCount + 1, from);
                        from = cur + Size;
                        sameDataRepeatCount = 0;
                        differentDataRepeatCount = 0;
                    } else   // ������ͬ��������Ŀ��1
                    {
                        differentDataRepeatCount++;
                    }
                }

                // �����������Ŀ���ж�
                if (sameDataRepeatCount >= MaxRepeatCount) // ������ͬ��������Ŀ�Ѿ��ﵽ�����ֵ
                {
                    writeSameData(pos, sameDataRepeatCount, from);
                    from = cur + Size;
                    differentDataRepeatCount = 0;
                    sameDataRepeatCount = 0;
                } else if (differentDataRepeatCount >= MaxRepeatCount)   // ������ͬ��������Ŀ�Ѿ��ﵽ�����ֵ
                {
                    writeDifferentData(pos, differentDataRepeatCount, from);
                    from = cur + Size;
                    differentDataRepeatCount = 0;
                    sameDataRepeatCount = 0;
                }

                cur += Size;
            }

            // β������
            if (sameDataRepeatCount > 0) // ���н���ǰ����������ͬ����
            {
                writeSameData(pos, sameDataRepeatCount + 1, from);
            } else if (differentDataRepeatCount > 0)    // ���򣬱��н���ǰ�в�ͬ����������
            {
                writeDifferentData(pos, differentDataRepeatCount + 1, from);
            } else if (from < (unsigned char*)src + bytes)  // ���ʣһ��Ԫ�أ�
            {
                writeSameData(pos, 1, from);
            }
        }

        return pos;
    }


    /** ��ѹ��
    @param src    Դ������
    @param bytes    Դ���������ݳ���(�ֽ���)
    @retval            �ɹ����ؽ�ѹ��������ݳ���(�ֽ���)�����򷵻�0
    */
    size_t decompress(const void* src, size_t bytes) {
        if (!src || bytes == 0) return 0;

        size_t sameDataRepeatCount = 0;            // ��ͬ�����������ֵĴ���
        size_t differentDataRepeatCount = 0;    // ��ͬ�����������ֵĴ���
        size_t pos = 0;                            // д�뻺���λ��
        unsigned char* cur = (unsigned char*)src;

        for (size_t i = 0; i < bytes;) {
            if (cur[i] >= MaxRepeatCount) // ������ͬ������
            {
                sameDataRepeatCount = cur[i++] - MaxRepeatCount + 1;
                readSameData(pos, sameDataRepeatCount, &cur[i]);
                i += Size;
            } else   // ������ͬ������
            {
                differentDataRepeatCount = cur[i++] + 1;
                size_t n = Size * differentDataRepeatCount;
                readDifferentData(pos, n, &cur[i]);
                i += n;
            }
        }
        return pos;
    }

private:
    inline void readSameData(size_t& pos, size_t count, unsigned char* from) {
        resize(pos + Size * count);
        while (count-- > 0) {
            ElemTraits<Size>::assign(&_buf[pos], from);
            pos += Size;
        }
    }
    inline void readDifferentData(size_t& pos, size_t bytes, unsigned char* from) {
        resize(pos + bytes);
        memcpy(&_buf[pos], from, bytes);
        pos += bytes;
    }
    inline void writeSameData(size_t& pos, size_t count, unsigned char* from) {
        resize(pos + 1 + Size);
        _buf[pos] = (unsigned char)(MaxRepeatCount + count - 1);
        pos++;

        ElemTraits<Size>::assign(&_buf[pos], from);
        pos += Size;
    }
    inline void writeDifferentData(size_t& pos, size_t count, unsigned char* from) {
        size_t n = Size * count;
        resize(pos + 1 + n);
        _buf[pos] = (unsigned char)(count - 1);
        pos++;

        memcpy(&_buf[pos], from, n);
        pos += n;
    }
    inline void resize(size_t new_cap) {
        if (new_cap >= _cap) {
            size_t good_cap = _cap ? _cap : 1024;
            while (good_cap <= new_cap)
                good_cap += good_cap >> 1;
            new_cap = (good_cap + 3) & ~3;

            unsigned char* buf = new unsigned char[new_cap];
            if (_buf) {
                memcpy(buf, _buf, _cap);
                delete[] _buf;
            }
            _buf = buf;
            _cap = new_cap;
        }
    }

    /// �ػ���ƫ�ػ�
    template<size_t _Size>
    struct ElemTraits {
        static bool equalto(const void* e1, const void* e2) { return memcmp(e1, e2, _Size) == 0; }
        static void assign(void* d, const void* s) { memcpy(d, s, _Size); }
    };
    template<>
    struct ElemTraits<1> {
        static bool equalto(const void* e1, const void* e2) { return *(const unsigned char*)e1 == *(const unsigned char*)e2; }
        static void assign(void* d, const void* s) { *(unsigned char*)d = *(const unsigned char*)s; }
    };
    template<>
    struct ElemTraits<2> {
        static bool equalto(const void* e1, const void* e2) { return *(const unsigned short*)e1 == *(const unsigned short*)e2; }
        static void assign(void* d, const void* s) { *(unsigned short*)d = *(const unsigned short*)s; }
    };
    template<>
    struct ElemTraits<4> {
        static bool equalto(const void* e1, const void* e2) { return *(const unsigned long*)e1 == *(const unsigned long*)e2; }
        static void assign(void* d, const void* s) { *(unsigned long*)d = *(const unsigned long*)s; }
    };
    template<>
    struct ElemTraits<3> {
        struct __rgb { unsigned char r, g, b; };
        static bool equalto(const void* e1, const void* e2) { return ((const __rgb*)e1)->r == ((const __rgb*)e2)->r && ((const __rgb*)e1)->g == ((const __rgb*)e2)->g && ((const __rgb*)e1)->b == ((const __rgb*)e2)->b; }
        static void assign(void* d, const void* s) { *(__rgb*)d = *(const __rgb*)s; }
    };
    template<>
    struct ElemTraits<8> {
        static bool equalto(const void* e1, const void* e2) { return *(const unsigned __int64*)e1 == *(const unsigned __int64*)e2; }
        static void assign(void* d, const void* s) { *(unsigned __int64*)d = *(const unsigned __int64*)s; }
    };
};

