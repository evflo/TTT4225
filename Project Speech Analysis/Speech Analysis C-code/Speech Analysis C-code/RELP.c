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



void RELPcoder(float* data, float* output,int P, const int length_data, int choice){
    
    //const int N = sizeof(data);
    int filterOrden = 8;
    int Fs = 16000;
    int Fc = 2000;
    int D = 4;
    int low_P = 4;
    int step = 0.02*Fs;
    int windowLength = 0.03*Fs;
    int halfStep = step/2;
    int speechLength = 0.03*Fs;
    int pitchLength = 0.05*Fs;
    int start = 0.005*Fs;
    float syntheticError[length_data],syntheticSpeechUpsampled[length_data],syntheticSpeechHF[length_data];
    float windowSpeech[speechLength]; 
    float dataDecimated[speechLength/D]; 
    float speechFilt[speechLength],speechEst[speechLength],speechError[speechLength],speechErrorFilt[speechLength];
    float ryLF[speechLength],ryHF[speechLength];
    float A[P],A_low[low_P];
    float B[1] = {1};
    float HFexcitation[speechLength], HFexcitationFilt[speechLength],HFexcitationLP[speechLength],LFexcitation[speechLength];
    float syntheticFullbandResidual[speechLength], speechUpsampling[speechLength],speechAlgorithm[speechLength];
    hammingWindow(windowSpeech,speechLength);

    //Make filters
    float gainLF, gainHF, gain;
    int i,j;
    for (i=0.03*Fs; i<N-0.025*Fs; i+= step) {
        int lastSpeech = i+1-0.015*Fs;
        int nextSpeech = i+0.015*Fs;
        int j;
        for (j = 0; j<speechLength; j++) {
            speechFilt[j]= windowSpeech[j]*data[lastSpeech+j];
        }
        autocorr(speechFilt, ry);
        LevinsonDurbin(ry, A, P);
        filtrate(speechEst, B, 1, A, P, speechFilt);
        
        for (j=0; j< speechLength; j++) {
            speechError[j] = speechFilt[j]-speechEst[j];
        }
        for(j= 0; j< step;j++){
            syntheticError[i+j-halfStep] = speechError[j+start];
        }
        
        
        //Filtrate speechError
        firFilter(coeffLow, filterOrden,speechError,speechErrorFilt,speechLength);
        //Decimate speechError
        decimate(speechErrorFilt, dataDecimated, speechLength, D); 
        //Upsample speechError
        upsample(dataDecimated, speechError, speechLength, D);

        firFilter(coeffLow,filterOrden,speechError,LFexcitation,speechLength);
        
        for (j = 0; j < speechLength; j++){
            HFexcitation[j] = LFexcitation[j];
            if (HFexcitation[j] < 0){
                HFexcitation[j] = 0;
            }
        }
        autocorr(HFexcitation,speechLength,ryHF);
        autocorr(LFexcitation,speechLength,ryLF);
        LevinsonDurbin(ry,A_low,low_P)

        filtrate(HFexcitation,B,1,A_low,low_P,HFexcitationLP);

        firFilter(coeffHigh,filterOrden,HFexcitationLP,HFexcitationFilt);
        gainHF = 0;
        gainLF = 0;
        for (j = 0; j < speechLength; j++){
            if(ryHF[j]> gainHF){
                gainHF = ryHF[j];
            }
            if (ryLF[j] > gainLF){
                gainLF = ryF[j];
            }
        }
        gain = gainLF/gainHF;

        for (j = 0; j < speechLength; j++){
            HFexcitationFilt[j] = gain*HFexcitationFilt[j];
            syntheticFullbandResidual[j] = LFexcitation[j] + HFexcitationFilt[j];
        }
        filtrate(speechError,B,1,A,P,speechUpsampling);
        filtrate(syntheticFullbandResidual,B,1,A,P,speechAlgorithm);
        for (j = 0; j< speechLength; j++){
            syntheticSpeechUpsampled[i+j-halfStep] = speechUpsampling[j+start];
            syntheticSpeechHF[i+j-halfStep] = speechAlgorithm[j+start];
        }
    }
    switch (choice){
        case 0:
            for (i = 0; i < length_data; i++){ output[i] = syntheticSpeechUpsampled[i];}
            break;
        case 1:
            for (i= 0; i<length_data; i++){ output[i] = syntheticSpeechHF[i];}
    }    
    
    //SNR
}