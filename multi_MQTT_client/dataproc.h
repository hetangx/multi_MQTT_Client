#ifndef DATAPROC_H

#include "zzzjson.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

#define JSONLENGTH 2000
#define WAVERESULT 33
#define WAVELENGTH 128  /*�������еĴ�С���ڱ������н���2�Ĵ���*/
#define JSON1ITEM 30

typedef struct COMPLEX{
    double real;
    double img;
}complex;

typedef struct ReturnJsonRef{
    char* sid;
    char* tid;
    char* tm;
    char* ref_id;
    int mode; /*mode: 1 -> ub, 2~7 -> wave, 8-> voiceprint*/
}makeJsonSt;

const char* parse1Helper_str(Value* v, char* key);
void makeJsonFront(char temp[], int cnum, makeJsonSt* mjst);
void makeJsonWaveBack(double newWave[], int wavenum, char** cptr);
void makeJsonUb(double newImbNgA, char** cptr);
void setHelper(Value* v, char* key, double item);

void fft(complex x[], complex W[]);    /*���ٸ���Ҷ�任*/
void change(complex x[]); /*��ַ*/
void add(complex, complex, complex*); /*�����ӷ�*/
void mul(complex, complex, complex*); /*�����˷�*/
void sub(complex, complex, complex*); /*��������*/
double output(complex x[], double result[]);/*������ٸ���Ҷ�任�Ľ��*/
void CutoffBuffer1(char* buffer);
double ParseWaveHelper(Value* v, int n);


char GetFigure(char* topicName);
void MessageSwitch(char* topicName, char* payload, char** cptr);
void CalcUnbalance(char* payload, char** cptr);
void CalcWave(char* payload, int fig, char** cptr);
void CalcVoice(char* payload, char** cptr);
#endif // !DATAPROC_H