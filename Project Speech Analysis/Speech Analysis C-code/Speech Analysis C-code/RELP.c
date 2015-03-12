//
//  RELP.c
//  Speech Analysis C-code
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include "RELP.h"
#include "signalProcessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



void RELPcoder(float* data, float* output,int P, const int N){
    
    //const int N = sizeof(data);
    int filterOrden = 8;
    int Fs = 16000;
    int Fc = 2000;
    int D = 4;
    int low_P = 4;
    int step = 0.02*Fs;
    int windowLength = 0.03*Fs;
    int halfStep = step/2;
    float syntheticError[N],syntheticSpeechUpsampled[N],syntheticSpeechHF[N];
    float windowSpeech[step]; //Feil størrelse
    float dataDecimated[step/D]; //Feil størrelse
    
    
    //Make filters
    
    int i;
    for (i=0.03*Fs; i<N-0.025*Fs; i+= step) {
        int lastSpeech = i+1-0.015*Fs;
        int nextSpeech = i+0.015*Fs;
        int j;
        float speechFilt[step]; //Best å gjøre utafor for-løkka?
        for (j = lastSpeech; j<nextSpeech; j++) {
            speechFilt[j]= windowSpeech[j]*data[lastSpeech+j];
        }
        float ry[nextSpeech-lastSpeech],A[P];
        autocorr(speechFilt, ry);
        LevinsonDurbin(ry, A, P);
        float B[1] = {1};
        float speechEst[step],speechError[step];
        filtrate(speechEst, B, 1, A, P, speechFilt);
        
        for (j=0; j< step; j++) {
            speechError[j] = speechFilt[j]-speechEst[j];
            syntheticError[i+j-halfStep] = speechError[j];
        }
        
        //Filtrate speechError
        //Decimate speechError
        decimate(data, dataDecimated, s, D); //Feil input
        //Upsample speechError
        upsample(dataDecimated, data, N, D); //Feil input
        
        
    }
    
}