#pragma once
#include <string>
#include <QString>

class CMd5
{
public:
    CMd5(void);
    ~CMd5(void);
    void GetMd5(const unsigned char input[], //加密数据源字节
                int inputSize, //input数组长度
                unsigned char output[], //加密后输出的数据字节
                int outputSize);//output数组长度，md5算法结果是16个字节，因此该值应该是16合适些

    //返回16位md5的16进制串，例如：39F70DCB69D1CBD5D54817285F77DE3C
    std::string GetMd5(const std::string& strData);

    //返回文件的md5
    QString GetFileMd5(const QString& strFile);

private:
    enum SS
    {
        S11=7,
        S12=12,
        S13=17,
        S14=22,
        S21=5,
        S22=9,
        S23=14,
        S24=20,
        S31=4,
        S32=11,
        S33=16,
        S34=23,
        S41=6,
        S42=10,
        S43=15,
        S44=21
    };
    /* MD5 context. */
    typedef struct {
        unsigned int state[4]; /* state (ABCD) */
        unsigned int count[2]; /* number of bits, modulo 2^64 (lsb first) */
        unsigned char buffer[64]; /* input buffer */
    } MD5_CTX;

    void MD5Init (MD5_CTX *);
    void MD5Update (MD5_CTX *, unsigned char *, unsigned int);
    void MD5Final (unsigned char [16], MD5_CTX *);

//////////////////////////////////////////////////////////////////////////
    void MD5Transform (unsigned int a[4], unsigned char b[64]);
    void Encode (unsigned char *, unsigned int *, unsigned int);
    void Decode (unsigned int *, unsigned char *, unsigned int);

    unsigned int F(unsigned int x, unsigned int y, unsigned int z);
    unsigned int G(unsigned int x, unsigned int y, unsigned int z);
    unsigned int H(unsigned int x, unsigned int y, unsigned int z);
    unsigned int I(unsigned int x, unsigned int y, unsigned int z);
    unsigned int ROTATE_LEFT(unsigned int x, unsigned int n);

    void FF(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac);
    void GG(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac);
    void HH(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac);
    void II(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac);
private:
    static unsigned char PADDING[64];
};
