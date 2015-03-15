//
//  RELP.c
//  Speech Analysis C-code
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include "RELP.h"
#include "SignalProcessing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



void RELPcoder(float* data, float* output,int length_data,int P, int choice){
    
    //const int N = sizeof(data);
    int filterOrden = 8;
    int Fs = 16000;
    int D = 4;
    int low_P = 4;
    int step = 0.02*Fs;
    int halfStep = step/2;
    int speechLength = 0.03*Fs;
    //int pitchLength = 0.05*Fs;
    int start = 0.005*Fs;
    
    float B[1] = {1};


    float* syntheticError = (float*) calloc(length_data,sizeof(float));
    float* syntheticSpeechUpsampled = (float*) calloc(length_data,sizeof(float));
    float* syntheticSpeechHF = (float*) calloc(length_data,sizeof(float));
    float* windowSpeech = (float*) calloc(speechLength,sizeof(float));
    float* dataDecimated = (float*) calloc(speechLength/D,sizeof(float));
    float* speechFilt = (float*) calloc(speechLength,sizeof(float));
    float* speechEst = (float*) calloc(speechLength,sizeof(float));
    float* speechError = (float*) calloc(speechLength,sizeof(float));
    float* speechErrorFilt = (float*) calloc(speechLength,sizeof(float));
    float* ryLF = (float*) calloc(speechLength,sizeof(float));
    float* ryHF = (float*) calloc(speechLength,sizeof(float));
    float* ry = (float*) calloc(speechLength,sizeof(float));
    float* A = (float*) calloc(P+1,sizeof(float));
    float* A_low = (float*) calloc(low_P,sizeof(float));
    float* HFexcitation = (float*) calloc(speechLength,sizeof(float));
    float* HFexcitationFilt = (float*) calloc(speechLength,sizeof(float));
    float* HFexcitationLP = (float*) calloc(speechLength,sizeof(float));
    float* LFexcitation = (float*) calloc(speechLength,sizeof(float));
    float* syntheticFullbandResidual = (float*) calloc(speechLength,sizeof(float));
    float* speechUpsampling = (float*) calloc(speechLength,sizeof(float));
    float* speechAlgorithm = (float*) calloc(speechLength,sizeof(float));
    //Make filters
    float coeffLow[9] = {0, -0.0277, 0, 0.274,0.4974, 0.274, 0, -0.0227, 0};
    float coeffHigh[9] = {0, -0.0161, -0.086, -0.1948, 0.7501, -0.1948, -0.0860, -0.0161, 0};
    float gainLF, gainHF, gain;
    int i,j,filtering;
    hammingWindow(windowSpeech,speechLength);
    float tmp;
    for (i=0.03*Fs; i<length_data-0.025*Fs; i+= step) {
        int lastSpeech = i+1-0.015*Fs;
        int nextSpeech = i+0.015*Fs;
       
       
        for (j = 0; j<nextSpeech-lastSpeech; j++) {
            tmp = windowSpeech[j]*data[lastSpeech+j];
            speechFilt[j]= tmp;
        }
        
        
        autocorr(speechFilt, speechLength,ry);
        
        LevinsonDurbin(ry, A, P);
        
        filtering = filtrate(speechFilt,speechLength ,B, 1, A, P, speechEst);
        if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
            printf("speechFilt, i = %d:\n", i);
            printf("%g, %g, %g\n", speechFilt[0],speechFilt[100], speechFilt[300]);
            printf("LevinsonDurbin: %g, %g, %g, %g, %g, %g\n", A[0], A[1], A[2], A[7], A[10], A[13]);
        }
        if(filtering == -1){
            printf("Filtrate function failed: Size of A smaller than 1\n");
            return;
        }
        for (j=0; j< speechLength; j++) {
            speechError[j] = speechFilt[j]-speechEst[j];
        }
        for(j= 0; j< step;j++){
            syntheticError[i+j-halfStep] = speechError[j+start];
        }
        if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
            printf("autocorr, i = %d:\n", i);
            printf("%g, %g, %g\n", ry[0], ry[100], ry[300]);
        }
        if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
            printf("speechError before filtering, i = %d:\n", i);
            printf("%g, %g, %g\n", speechError[0],speechError[100], speechError[300]);
        }
        if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
            printf("speechEst, i = %d:\n", i);
            printf("%g, %g, %g\n", speechEst[0],speechEst[100], speechEst[300]);
        }
        //Filtrate speechError
        firFilter(coeffLow, filterOrden,speechError,speechErrorFilt,speechLength);
        //Decimate speechError
        if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
            printf("speechErrorFilt, i = %d:\n", i);
            printf("%g, %g, %g\n", speechErrorFilt[0],speechErrorFilt[100], speechErrorFilt[300]);
        }
        decimate(speechErrorFilt, dataDecimated, speechLength, D); 
        //Upsample speechError
        upsample(dataDecimated, speechError, speechLength, D);
        if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
            printf("speechError after upsampling, i = %d:\n", i);
            printf("%g, %g, %g\n", speechError[0],speechError[100], speechError[300]);
        }
        firFilter(coeffLow,filterOrden,speechError,LFexcitation,speechLength);
        if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
            printf("LFexcitation, i = %d:\n", i);
            printf("%g, %g, %g\n", LFexcitation[0],LFexcitation[100], LFexcitation[300]);
        }
        for (j = 0; j < speechLength; j++){
            HFexcitation[j] = LFexcitation[j];
            if (HFexcitation[j] < 0){
                HFexcitation[j] = 0;
            }
        }
        autocorr(HFexcitation,speechLength,ryHF);
        autocorr(LFexcitation,speechLength,ryLF);
        LevinsonDurbin(ry,A_low,low_P);

        filtering = filtrate(HFexcitation,speechLength,B,1,A_low,low_P,HFexcitationLP);
        if(filtering == -1){
            printf("Filtrate function failed: Size of A smaller than 1\n");
            return;
        }
        firFilter(coeffHigh,filterOrden,HFexcitationLP,HFexcitationFilt,speechLength);
        gainHF = 0;
        gainLF = 0;
        for (j = 0; j < speechLength; j++){
            if(ryHF[j]> gainHF){
                gainHF = ryHF[j];
            }
            if (ryLF[j] > gainLF){
                gainLF = ryLF[j];
            }
        }
        gain = gainLF/gainHF;

        for (j = 0; j < speechLength; j++){
            HFexcitationFilt[j] = gain*HFexcitationFilt[j];
            syntheticFullbandResidual[j] = LFexcitation[j] + HFexcitationFilt[j];
        }
        filtering = filtrate(speechError,speechLength,B,1,A,P,speechUpsampling);
        if(filtering == -1){
            printf("Filtrate function failed: Size of A smaller than 1\n");
            return;
        }
        filtering = filtrate(syntheticFullbandResidual,speechLength,B,1,A,P,speechAlgorithm);
        if(filtering == -1){
            printf("Filtrate function failed: Size of A smaller than 1\n");
            return;
        }
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
            break;
    }    
    float maxVal = 0;
    for (i = 0; i < length_data; i++){
        if (fabsf(output[i]) > maxVal){
            maxVal = fabsf(output[i]);
        }
    }
    for (i = 0; i < length_data; i++){
        output[i] = output[i] / maxVal;
    }
    //SNR
    printf("Max value: %g\n", maxVal);
    printf("Error values: %g %g %g\n",syntheticError[1000],syntheticError[5000],syntheticError[10000]);
    printf("Output(HFregeneration) before gain: %g %g %g\n",syntheticSpeechHF[1000],syntheticSpeechHF[5000],syntheticSpeechHF[10000]);
    printf("Output(upsample) before gain: %g %g %g\n",output[1000],output[5000],output[10000]);
    free(syntheticError),free(syntheticSpeechUpsampled),free(syntheticSpeechHF),free(windowSpeech);
    free(dataDecimated), free(speechFilt), free(speechEst),free(speechError),free(speechErrorFilt);
    free(ryLF),free(ryHF),free(A),free(A_low),free(HFexcitation),free(HFexcitationFilt);
    free(HFexcitationLP),free(LFexcitation),free(syntheticFullbandResidual),free(speechUpsampling);
    free(speechAlgorithm);
}