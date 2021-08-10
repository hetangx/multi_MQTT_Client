#ifndef DATAPROC_H

#include "zzzjson.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

#define JSONWAVELENGTH 2000
#define WAVERESULT 33
#define WAVELENGTH 128  /*�������еĴ�С���ڱ������н���2�Ĵ���*/

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

void fft();     /*���ٸ���Ҷ�任*/
void change(); /*��ַ*/
void add(complex, complex, complex*); /*�����ӷ�*/
void mul(complex, complex, complex*); /*�����˷�*/
void sub(complex, complex, complex*); /*��������*/
double output();/*������ٸ���Ҷ�任�Ľ��*/
void CutoffBuffer1(char* buffer);
double ParseWaveHelper(Value* v, int n);

#endif // !DATAPROC_H