//
//  signalProcessing.h
//  SpeechAnalysis
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#ifndef __SpeechAnalysis__signalProcessing__
#define __SpeechAnalysis__signalProcessing__

#include <stdio.h>

void filtrate(float* y,float* B,int sizeB,float* A,int sizeA,float* x);

void LevinsonDurbin(float* r,float* A,int P);

int rand_gauss (float *x, int N);

void hammingWindow(int L,float* ham);

void autocorr(float* x,float* rx);

#endif /* defined(__SpeechAnalysis__signalProcessing__) */
