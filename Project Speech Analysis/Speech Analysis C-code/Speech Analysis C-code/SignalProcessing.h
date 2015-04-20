//
//  Signal Processing.h
//  Speech Analysis C-code
//


#ifndef __Speech_Analysis_C_code__Signal_Processing__
#define __Speech_Analysis_C_code__Signal_Processing__

#include <stdio.h>

int filtrate(float* x,int lengthx, float* B,int sizeB,float* A,int sizeA,float* y);

void LevinsonDurbin(float* r,float* A,int P);

int rand_gauss (float *x, int N);

void hammingWindow(float* hamming,int L);

void autocorr(float* x,int lengthx,float* rx);

void decimate(float *x, float *xDec, int N, int D);

void upsample(float *xDec, float *x, int N, int D);

void firFilter(float* coeff,int Ncoeffs, float* x, float* xFiltred, int n);

void findPitchAndVoice(float* y_pitch,int pitchLength,float* pitchProperties,int Fs);

#endif /* defined(__Speech_Analysis_C_code__Signal_Processing__) */
