#include "dataproc.h"

complex x[WAVELENGTH];
complex W[WAVELENGTH];
double result[WAVERESULT];

/*快速傅里叶变换*/
void fft()
{
    int i = 0, j = 0, k = 0, l = 0;
    complex up, down, product;
    change();  //调用变址函数
    int n = (int)(log(WAVELENGTH) / log(2));
    for (i = 0; i < n; i++)        /*一级蝶形运算 stage */
    {
        l = 1 << i;
        for (j = 0; j < WAVELENGTH; j += 2 * l)     /*一组蝶形运算 group,每个group的蝶形因子乘数不同*/
        {
            for (k = 0; k < l; k++)        /*一个蝶形运算 每个group内的蝶形运算*/
            {
                mul(x[j + k + l], W[WAVELENGTH * k / 2 / l], &product);
                add(x[j + k], product, &up);
                sub(x[j + k], product, &down);
                x[j + k] = up;
                x[j + k + l] = down;
            }
        }
    }
}

/*变址计算，将x(n)码位倒置*/
void change()
{
    complex temp;
    unsigned short i = 0, j = 0, k = 0;
    int t;
    for (i = 0; i < WAVELENGTH; i++)
    {
        k = i; j = 0;
        t = (int)(log(WAVELENGTH) / log(2));
        while ((t--) > 0)    //利用按位与以及循环实现码位颠倒
        {
            j = j << 1;
            j |= (k & 1);
            k = k >> 1;
        }
        if (j > i)    //将x(n)的码位互换
        {
            temp = x[i];
            x[i] = x[j];
            x[j] = temp;
        }
    }

}



/*输出傅里叶变换的结果*/
double output()
{
    int i;
    double sum = 0;
    double thd;
    // printf("The result are as follows：\n");
    double temp;
    for (i = 0; i < 32; i++)
    {
        int a = (int)(log(WAVELENGTH) / log(2)) - 1;
        temp = pow((x[i].real * x[i].real) / pow(2, 2 * (double)a) + (x[i].img * x[i].img) / pow(2, 2 * (double)a), 0.5);
        //printf("%.4f\n", temp); //输出幅值
        result[i] = temp;
    }
    for (i = 2; i < 32; i++)
    {
        sum += pow(x[i].real, 2);
        thd = pow(sum / (x[1].real * x[1].real), 0.5);
    }
    //printf("THD=%f", thd);
    return thd;
}


void add(complex a, complex b, complex* c)  //复数加法的定义
{
    c->real = a.real + b.real;
    c->img = a.img + b.img;
}


void mul(complex a, complex b, complex* c)  //复数乘法的定义
{
    c->real = a.real * b.real - a.img * b.img;
    c->img = a.real * b.img + a.img * b.real;
}


void sub(complex a, complex b, complex* c)  //复数减法的定义
{
    c->real = a.real - b.real;
    c->img = a.img - b.img;
}

void CutoffBuffer1(char* buffer)
{
    //printf("length buffer: %d\n", (int)strlen(buffer));
    char* ptr = NULL;
    char tofind = '}';
    ptr = strrchr(buffer, tofind);
    if (ptr)
    {
        ptr++;
        int len = (int)strlen(buffer);
        int plen = (int)strlen(ptr);
        //printf("content buffer: %s\ncontent ptr: %s\n", buffer, ptr);
        //printf("length buffer: %d\nlength ptr: %d\n", len, plen);

        buffer[len - plen] = 0;
    }
    //printf("work out: %s\n", buffer);
}

void setHelper(Value* v, char* key, double item)
{
    Value* temp = ObjGet(v, key);
    if (temp == 0)
    {
        return;
    }
    SetDouble(temp, item);
}


void makeJsonWaveFront(char temp[], int cnum, char* sid, char* tid, char* tm, char* rid)
{
    const char* mod1 = "{\"substationid\":";
    //TEMPSID 
    const char* mod2 = ",\"transformerid\":";
    //TEMPTID
    const char* mod3 = ",\"time\":";
    //TEMPTIME
    const char* mod4 = ",\"ref_id\":";
    //TEMPRID
    const char* mod5 = ",\"HphV0_phsC\":1.1234,\"HphV1_phsC\":16.0000,\"HphV2_phsC\":1.050,\"HphV3_phsC\":13.000,\"HphV4_phsC\":14.012,\"HphV5_phsC\":1.333,\"HphV6_phsC\":1.222,\"HphV7_phsC\":1.124,\"HphV8_phsC\":2.0000,\"HphV9_phsC\":1.050,\"HphV10_phsC\":13.000,\"HphV11_phsC\":1.1234,\"HphV12_phsC\":20.0000,\"HphV13_phsC\":1.050,\"HphV14_phsC\":13.000,\"HphV15_phsC\":14.012,\"HphV16_phsC\":12.333,\"HphV17_phsC\":15.222,\"HphV18_phsC\":16,\"HphV19_phsC\":20.1000,\"HphV20_phsC\":1.050,\"HphV21_phsC\":13.000,\"HphV22_phsC\":1.1234,\"HphV23_phsC\":220.0000,\"HphV24_phsC\":1.050,\"HphV25_phsC\":13.000,\"HphV26_phsC\":14.012,\"HphV27_phsC\":12.333,\"HphV28_phsC\":15.222,\"HphV29_phsC\":0.8,\"HphV30_phsC\":2.0000,\"HphV31_phsC\":1.050,\"THDUc\":0.5}";

    strncpy_s(temp, cnum, mod1, strlen(mod1));
    strncat_s(temp, cnum, sid, strlen(sid));
    strncat_s(temp, cnum, mod2, strlen(mod2));
    strncat_s(temp, cnum, tid, strlen(tid));
    strncat_s(temp, cnum, mod3, strlen(mod3));
    strncat_s(temp, cnum, tm, strlen(tm));
    strncat_s(temp, cnum, mod4, strlen(mod4));
    strncat_s(temp, cnum, rid, strlen(rid));
    strncat_s(temp, cnum, mod5, strlen(mod5));
}

void makeJsonWaveBack(double newWave[], int wavenum, char** cptr)
{
    Allocator* A = NewAllocator();
    Value* v = NewValue(A);
    bool ret = Parse(v, *cptr);
    if (ret == true)
    {


        char* keys[] = {
            "HphV0_phsC", "HphV1_phsC", "HphV2_phsC","HphV3_phsC","HphV4_phsC","HphV5_phsC","HphV6_phsC","HphV7_phsC",
            "HphV8_phsC","HphV9_phsC","HphV10_phsC","HphV11_phsC","HphV12_phsC","HphV13_phsC","HphV14_phsC","HphV15_phsC",
            "HphV16_phsC","HphV17_phsC","HphV18_phsC","HphV19_phsC","HphV20_phsC","HphV21_phsC","HphV22_phsC","HphV23_phsC",
            "HphV24_phsC","HphV25_phsC","HphV26_phsC","HphV27_phsC","HphV28_phsC","HphV29_phsC","HphV30_phsC","HphV31_phsC",
            "THDUc"
        };


        for (int i = 0; i < wavenum; i++)
        {
            setHelper(v, keys[i], newWave[i]);
        }
    }
    const char* str = Stringify(v);

    strcpy_s(*cptr, JSONWAVELENGTH, str);

    ReleaseAllocator(A);
}



double ParseWaveHelper(Value* v, int n)
{
    Value* temp = ArrayGet(v, n);
    if (temp == 0)
    {
        return -1;
    }
    const double* t = GetDouble(temp);
    return *t;
}


const char* parse1Helper_str(Value* v, char* key)
{
    Value* osid = ObjGet(v, key);
    if (osid == 0)
    {
        return "-1";
    }
    return Stringify(osid);
}