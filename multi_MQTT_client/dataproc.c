#include "dataproc.h"

//complex x[WAVELENGTH];
//complex W[WAVELENGTH];
//double result[WAVERESULT];

/*set unbalance new ImbNgA*/
void makeJsonUb(double newImbNgA, char** cptr)
{
    Allocator* A = NewAllocator();
    Value* v = NewValue(A);
    bool ret = Parse(v, *cptr);
    if (ret == true)
    {
        setHelper(v, "ImbNgA", newImbNgA);
        const char* str = Stringify(v);
        strcpy_s(*cptr, JSONLENGTH, str);
    }
    ReleaseAllocator(A);
}

/*快速傅里叶变换*/
void fft(complex x[], complex W[])
{
    int i = 0, j = 0, k = 0, l = 0;
    complex up, down, product;
    change(x);  //调用变址函数
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
void change(complex x[])
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
double output(complex x[], double result[])
{
    int i;
    double sum = 0;
    double thd;
    // printf("The result are as follows：\n");
    double temp;
    for (i = 0; i < WAVERESULT-1; i++)
    {
        int a = (int)(log(WAVELENGTH) / log(2)) - 1;
        temp = pow((x[i].real * x[i].real) / pow(2, 2 * (double)a) + (x[i].img * x[i].img) / pow(2, 2 * (double)a), 0.5);
        //printf("%.4f\n", temp); //输出幅值
        result[i] = temp;
    }
    for (i = 2; i < WAVERESULT-1; i++)
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

/*set double by key*/
void setHelper(Value* v, char* key, double item)
{
    Value* temp = ObjGet(v, key);
    if (temp == 0)
    {
        return;
    }
    SetDouble(temp, item);
}

void makeJsonFront(char temp[], int cnum, makeJsonSt* mjst)
{
    // un balance
    if (mjst->mode == 1)
    {
        const char* mod1 = "{\"substationid\":";
        //TEMPSID 
        const char* mod2 = ",\"transformerid\":";
        //TEMPTID
        const char* mod3 = ",\"time\":\"";
        //TEMPTIME
        const char* mod4 = "\",\"ImbNgA\":0.5}";
        //TEMPIMBGA

        strncpy_s(temp, cnum, mod1, strlen(mod1));
        strncat_s(temp, cnum, mjst->sid, strlen(mjst->sid));
        strncat_s(temp, cnum, mod2, strlen(mod2));
        strncat_s(temp, cnum, mjst->tid, strlen(mjst->tid));
        strncat_s(temp, cnum, mod3, strlen(mod3));
        strncat_s(temp, cnum, mjst->tm, strlen(mjst->tm));
        strncat_s(temp, cnum, mod4, strlen(mod4));
        return;
    }

    // voice print
    if (mjst->mode == 8) 
    {
        printf("voice print need to be done\n");
        return;
    }

    // mjst.mode = 2~7
    {
        const char* mod1 = "{\"substationid\":";
        //TEMPSID 
        const char* mod2 = ",\"transformerid\":";
        //TEMPTID
        const char* mod3 = ",\"time\":\"";
        //TEMPTIME
        const char* mod4 = "\",\"ref_id\":";
        //TEMPRID
        const char* mod5 = NULL;
        if (mjst->mode == 2)
        {
            mod5 = ",\"HphV0_phsA\":1.1234,\"HphV1_phsA\":16.0000,\"HphV2_phsA\":1.050,\"HphV3_phsA\":13.000,\"HphV4_phsA\":14.012,\"HphV5_phsA\":1.333,\"HphV6_phsA\":1.222,\"HphV7_phsA\":1.124,\"HphV8_phsA\":2.0000,\"HphV9_phsA\":1.050,\"HphV10_phsA\":13.000,\"HphV11_phsA\":1.1234,\"HphV12_phsA\":20.0000,\"HphV13_phsA\":1.050,\"HphV14_phsA\":13.000,\"HphV15_phsA\":14.012,\"HphV16_phsA\":12.333,\"HphV17_phsA\":15.222,\"HphV18_phsA\":16,\"HphV19_phsA\":20.1000,\"HphV20_phsA\":1.050,\"HphV21_phsA\":13.000,\"HphV22_phsA\":1.1234,\"HphV23_phsA\":220.0000,\"HphV24_phsA\":1.050,\"HphV25_phsA\":13.000,\"HphV26_phsA\":14.012,\"HphV27_phsA\":12.333,\"HphV28_phsA\":15.222,\"HphV29_phsA\":0.8,\"HphV30_phsA\":2.0000,\"HphV31_phsA\":1.050,\"THDUa\":0.5}";
        }
        if (mjst->mode == 3)
        {
            mod5 = ",\"HphV0_phsB\":1.1234,\"HphV1_phsB\":16.0000,\"HphV2_phsB\":1.050,\"HphV3_phsB\":13.000,\"HphV4_phsB\":14.012,\"HphV5_phsB\":1.333,\"HphV6_phsB\":1.222,\"HphV7_phsB\":1.124,\"HphV8_phsB\":2.0000,\"HphV9_phsB\":1.050,\"HphV10_phsB\":13.000,\"HphV11_phsB\":1.1234,\"HphV12_phsB\":20.0000,\"HphV13_phsB\":1.050,\"HphV14_phsB\":13.000,\"HphV15_phsB\":14.012,\"HphV16_phsB\":12.333,\"HphV17_phsB\":15.222,\"HphV18_phsB\":16,\"HphV19_phsB\":20.1000,\"HphV20_phsB\":1.050,\"HphV21_phsB\":13.000,\"HphV22_phsB\":1.1234,\"HphV23_phsB\":220.0000,\"HphV24_phsB\":1.050,\"HphV25_phsB\":13.000,\"HphV26_phsB\":14.012,\"HphV27_phsB\":12.333,\"HphV28_phsB\":15.222,\"HphV29_phsB\":0.8,\"HphV30_phsB\":2.0000,\"HphV31_phsB\":1.050,\"THDUb\":0.5}";
        }
        if (mjst->mode == 4)
        {
            mod5 = ",\"HphV0_phsC\":1.1234,\"HphV1_phsC\":16.0000,\"HphV2_phsC\":1.050,\"HphV3_phsC\":13.000,\"HphV4_phsC\":14.012,\"HphV5_phsC\":1.333,\"HphV6_phsC\":1.222,\"HphV7_phsC\":1.124,\"HphV8_phsC\":2.0000,\"HphV9_phsC\":1.050,\"HphV10_phsC\":13.000,\"HphV11_phsC\":1.1234,\"HphV12_phsC\":20.0000,\"HphV13_phsC\":1.050,\"HphV14_phsC\":13.000,\"HphV15_phsC\":14.012,\"HphV16_phsC\":12.333,\"HphV17_phsC\":15.222,\"HphV18_phsC\":16,\"HphV19_phsC\":20.1000,\"HphV20_phsC\":1.050,\"HphV21_phsC\":13.000,\"HphV22_phsC\":1.1234,\"HphV23_phsC\":220.0000,\"HphV24_phsC\":1.050,\"HphV25_phsC\":13.000,\"HphV26_phsC\":14.012,\"HphV27_phsC\":12.333,\"HphV28_phsC\":15.222,\"HphV29_phsC\":0.8,\"HphV30_phsC\":2.0000,\"HphV31_phsC\":1.050,\"THDUc\":0.5}";
        }
        if (mjst->mode == 5)
        {
            mod5 = ",\"HA0_phsA\":1.1234,\"HA1_phsA\":16.0000,\"HA2_phsA\":1.050,\"HA3_phsA\":13.000,\"HA4_phsA\":14.012,\"HA5_phsA\":1.333,\"HA6_phsA\":1.222,\"HA7_phsA\":1.124,\"HA8_phsA\":2.0000,\"HA9_phsA\":1.050,\"HA10_phsA\":13.000,\"HA11_phsA\":1.1234,\"HA12_phsA\":20.0000,\"HA13_phsA\":1.050,\"HA14_phsA\":13.000,\"HA15_phsA\":14.012,\"HA16_phsA\":12.333,\"HA17_phsA\":15.222,\"HA18_phsA\":16,\"HA19_phsA\":20.1000,\"HA20_phsA\":1.050,\"HA21_phsA\":13.000,\"HA22_phsA\":1.1234,\"HA23_phsA\":220.0000,\"HA24_phsA\":1.050,\"HA25_phsA\":13.000,\"HA26_phsA\":14.012,\"HA27_phsA\":12.333,\"HA28_phsA\":15.222,\"HA29_phsA\":0.8,\"HA30_phsA\":2.0000,\"HA31_phsA\":1.050,\"THDIa\":0.5}";
        }
        if (mjst->mode == 6)
        {
            mod5 = ",\"HA0_phsB\":1.1234,\"HA1_phsB\":16.0000,\"HA2_phsB\":1.050,\"HA3_phsB\":13.000,\"HA4_phsB\":14.012,\"HA5_phsB\":1.333,\"HA6_phsB\":1.222,\"HA7_phsB\":1.124,\"HA8_phsB\":2.0000,\"HA9_phsB\":1.050,\"HA10_phsB\":13.000,\"HA11_phsB\":1.1234,\"HA12_phsB\":20.0000,\"HA13_phsB\":1.050,\"HA14_phsB\":13.000,\"HA15_phsB\":14.012,\"HA16_phsB\":12.333,\"HA17_phsB\":15.222,\"HA18_phsB\":16,\"HA19_phsB\":20.1000,\"HA20_phsB\":1.050,\"HA21_phsB\":13.000,\"HA22_phsB\":1.1234,\"HA23_phsB\":220.0000,\"HA24_phsB\":1.050,\"HA25_phsB\":13.000,\"HA26_phsB\":14.012,\"HA27_phsB\":12.333,\"HA28_phsB\":15.222,\"HA29_phsB\":0.8,\"HA30_phsB\":2.0000,\"HA31_phsB\":1.050,\"THDIb\":0.5}";
        }
        if (mjst->mode == 7)
        {
            mod5 = ",\"HA0_phsC\":1.1234,\"HA1_phsC\":16.0000,\"HA2_phsC\":1.050,\"HA3_phsC\":13.000,\"HA4_phsC\":14.012,\"HA5_phsC\":1.333,\"HA6_phsC\":1.222,\"HA7_phsC\":1.124,\"HA8_phsC\":2.0000,\"HA9_phsC\":1.050,\"HA10_phsC\":13.000,\"HA11_phsC\":1.1234,\"HA12_phsC\":20.0000,\"HA13_phsC\":1.050,\"HA14_phsC\":13.000,\"HA15_phsC\":14.012,\"HA16_phsC\":12.333,\"HA17_phsC\":15.222,\"HA18_phsC\":16,\"HA19_phsC\":20.1000,\"HA20_phsC\":1.050,\"HA21_phsC\":13.000,\"HA22_phsC\":1.1234,\"HA23_phsC\":220.0000,\"HA24_phsC\":1.050,\"HA25_phsC\":13.000,\"HA26_phsC\":14.012,\"HA27_phsC\":12.333,\"HA28_phsC\":15.222,\"HA29_phsC\":0.8,\"HA30_phsC\":2.0000,\"HA31_phsC\":1.050,\"THDIc\":0.5}";
        }
        if (mod5 == NULL)
        {
            return;
        }
        strncpy_s(temp, cnum, mod1, strlen(mod1));
        strncat_s(temp, cnum, mjst->sid, strlen(mjst->sid));
        strncat_s(temp, cnum, mod2, strlen(mod2));
        strncat_s(temp, cnum, mjst->tid, strlen(mjst->tid));
        strncat_s(temp, cnum, mod3, strlen(mod3));
        strncat_s(temp, cnum, mjst->tm, strlen(mjst->tm));
        strncat_s(temp, cnum, mod4, strlen(mod4));
        strncat_s(temp, cnum, mjst->ref_id, strlen(mjst->ref_id));
        strncat_s(temp, cnum, mod5, strlen(mod5));
    }
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
    strcpy_s(*cptr, JSONLENGTH, str);
    ReleaseAllocator(A);
}

/*parse double from array by order*/
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

/*parse double item by key*/
double Parse1Helper(Value* v, char* key)
{
    Value* temp = ObjGet(v, key);
    if (temp == 0)
    {
        return;
    }
    const double* t = GetDouble(temp);
    return *t;
}

/*parse string item by key*/
const char* parse1Helper_str(Value* v, char* key)
{
    Value* osid = ObjGet(v, key);
    if (osid == 0)
    {
        return "-1";
    }
    return Stringify(osid);
}

/*get topic figure from topic name*/
char GetFigure(char* topicName)
{
    char* fig = strstr(topicName, "/up");
    char a = 0;
    if (fig)
    {
        fig--;
        a = *fig;
        // printf("a: %c\n", a);
    }
    return a;
}

/*Switch fig*/
void MessageSwitch(char* topicName, char* payload, char** cptr)
{
    // need to reconstruct
    // use function pointer array to re-construct message switch
    char fig = GetFigure(topicName);
    switch (fig)
    {
    case '1':
        // un balance
        CalcUnbalance(payload, cptr);
        break;
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
        // wave
        CalcWave(payload, (fig - '0'), cptr);
        break;
    case '8':
        //voiceprint;
        CalcVoice(payload, cptr);

        break;
    default:
        printf("get fig error, fig=%c\n", fig);
        return -1;
        break;
    }

}

/*计算三项不平衡*/
double unbalance(double out[])
{
    double avr = (out[4] + out[12] + out[20]) / 3.0;
    double max1 = abs(avr - out[4]);
    double max2 = abs(avr - out[12]);
    double max3 = abs(avr - out[20]);

    double max4 = max(max1, max2);
    double max5 = max(max4, max3);

    double unblance = max5 / avr;

    return unblance;
}

void CalcUnbalance(char* payload, char** cptr)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    char otm[30];
    sprintf_s(otm, 30, "%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    Allocator* A = NewAllocator();
    Value* v = NewValue(A);
    CutoffBuffer1(payload);
    bool ret = Parse(v, payload);
    if (ret != true)
    {
        printf("Parse unbalance payload error\n");
        return;
    }

    double out[JSON1ITEM]; 
    char* keys[JSON1ITEM] = { "substationid", "transformerid", "time",
            "Va", "Ia", "Fa", "Posa", "Nega", "VAa", "Elea", "Viba",
            "Vb", "Ib", "Fb", "Posb", "Negb", "VAb", "Eleb", "Vibb",
            "Vc", "Ic", "Fc", "Posc", "Negc", "VAc", "Elec", "Vibc",
            "Temp1", "Temp2", "Temp3" };

    for (int i = 3; i < 30; i++)
    {
        out[i] = Parse1Helper(v, keys[i]);
    }

    double ub = unbalance(out);
    char* osid = parse1Helper_str(v, "substationid");
    char* otid = parse1Helper_str(v, "transformerid");

    char front[JSONLENGTH];
    makeJsonSt mjst;
    mjst.mode = 1;
    mjst.sid = osid;
    mjst.tid = otid;
    mjst.tm = otm;
    mjst.ref_id = NULL;

    makeJsonFront(front, JSONLENGTH, &mjst);
    memset(*cptr, 0, JSONLENGTH);
    strcpy_s(*cptr, JSONLENGTH, front);
    makeJsonUb(ub, cptr);

    // free memory
    ReleaseAllocator(A);
}

void CalcWave(char* payload, int fig, char** cptr)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    char otm[30];
    sprintf_s(otm, 30, "%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    Allocator* A = NewAllocator();
    Value* v = NewValue(A);
    CutoffBuffer1(payload);
    bool ret = Parse(v, payload);
    if (ret != true)
    {
        printf("Parse wave payload error\n");
        return -1;
    }

    char* parsecontent[6] = {
        "Vwfa", "Vwfb", "Vwfc", "Cwfa", "Cwfb", "Cwfc"
    };

    Value* waveArray = ObjGet(v, parsecontent[fig-2]);
    if (waveArray == 0)
    {
        printf("parse wave json content: %s error\n", parsecontent[fig - 2]);
        return;
    }

    complex x[WAVELENGTH];
    complex W[WAVELENGTH];
    double result[WAVERESULT];

    //FFT func
    double PI = atan(1) * 4;
    
    //init x
    for (int i = 0; i < WAVELENGTH; i++)
    {
        double temp = ParseWaveHelper(waveArray, i);
        x[i].real = temp;
    }

    for (int j = 0; j < 64; j++)
    {
        if (j % 5 == 0)
        {
            continue;
        }
        x[j].real = ((x[j + 1].real - x[j].real) / 256) * (56 * ((double)j + 1));
        x[j].img = 0;
    }

    //初始化变换核, initw

    for (int i = 0; i < WAVELENGTH; i++)
    {
        W[i].real = cos(2 * PI / WAVELENGTH * i);   //用欧拉公式计算旋转因子
        W[i].img = -1 * sin(2 * PI / WAVELENGTH * i);
    }

    fft(x, W);//调用快速傅里叶变换
    result[32] = output(x, result);//调用输出傅里叶变换结果函数

    char* osid = (char*)parse1Helper_str(v, "substationid");
    char* otid = (char*)parse1Helper_str(v, "transformerid");
    char* orid = (char*)parse1Helper_str(v, "ref_id");

    char front[JSONLENGTH];
    makeJsonSt mjst;
    mjst.mode = fig;
    mjst.sid = osid;
    mjst.tid = otid;
    mjst.tm = otm;
    mjst.ref_id = orid;

    makeJsonFront(front, JSONLENGTH, &mjst);
    memset(*cptr, 0, JSONLENGTH);
    strcpy_s(*cptr, JSONLENGTH, front);

    makeJsonWaveBack(result, WAVERESULT, cptr);

    // free memory
    ReleaseAllocator(A);
    memset(result, 0, 33);
    memset(x, 0, sizeof(complex) * WAVELENGTH);
    memset(W, 0, sizeof(complex) * WAVELENGTH);
}

void CalcVoice(char* payload, char** cptr)
{
    printf("voiceprint need to be done\n");
}