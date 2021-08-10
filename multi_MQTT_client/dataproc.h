#ifndef DATAPROC_H

#include "zzzjson.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

#define JSONWAVELENGTH 2000
#define WAVERESULT 33
#define WAVELENGTH 128  /*输入序列的大小，在本程序中仅限2的次幂*/

typedef struct {
    double real;
    double img;
}complex;

extern complex x[];
extern complex W[];
extern double result[];

const char* parse1Helper_str(Value* v, char* key);
void makeJsonWaveFront(char temp[], int cnum, char* sid, char* tid, char* tm, char* rid);
void makeJsonWaveBack(double newWave[], int wavenum, char** cptr);
void setHelper(Value* v, char* key, double item);

void fft();     /*快速傅里叶变换*/
void change(); /*变址*/
void add(complex, complex, complex*); /*复数加法*/
void mul(complex, complex, complex*); /*复数乘法*/
void sub(complex, complex, complex*); /*复数减法*/
double output();/*输出快速傅里叶变换的结果*/
void CutoffBuffer1(char* buffer);
double ParseWaveHelper(Value* v, int n);

#endif // !DATAPROC_H