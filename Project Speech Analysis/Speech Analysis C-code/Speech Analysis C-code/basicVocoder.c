//
//  basicVocoder.c
//  Speech Analysis C-code
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include "basicVocoder.h"
#include "SignalProcessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



void findPitchAndVoice(float* y_pitch,float* pitchProperties,int Fs){
    
    int N = floor(0.02*Fs)-floor(0.002*Fs);
    float ry[N];
    autocorr(y_pitch, ry);
    int i,j,minima = 0;
    float pitchRange[N],pitchFrame[N];
    for (i = floor(0.002*Fs); i<floor(0.02*Fs); i++) {
        pitchRange[i] = i;
        pitchFrame[i] = ry[i];
    }
    int foundMinima = 0;
    for (j = 0; j<N; j++) {
        if (pitchFrame[j] <= 0) {
            minima= j;
            foundMinima = 1;
            break;
        }
    }
    if(foundMinima == 0){
        minima = N;
    }
    
    int k,pitchPos = 0;
    int max = 0;
    for (k = minima; k<N; k++) {
        if (pitchPos< pitchFrame[k]) {
            pitchPos = k;
            max = pitchFrame[k];
        }
    }
    int pitchPeriod = pitchPos + minima + pitchRange[0] - 3;
    float pitchRatio = ry[pitchPeriod+1]/ry[0];
    pitchProperties[0] = pitchPeriod;
    pitchProperties[1] = pitchRatio;
}


void basicVocoder(float* data,float* output, int P){
    int Fs = 16000;
    int Fc = 4000;
    printf("Esel %i",Fc);
    int N = 8;
    int step = 0.02*Fs;
    int halfStep = step*0.5;
    int speechLength = 0.03*Fs;
    float alpha = 0.5,beta = 0.1;
    int pitchLength;
    const int length_data = sizeof(data);
    float pitch[length_data],noise[length_data],vocoderInput[length_data],synthezised[length_data];
    float windowSpeech[speechLength],windowPitch[pitchLength];
    hammingWindow(0.03*Fs,windowSpeech);
    hammingWindow(0.05*Fs,windowPitch);
    memset(&pitch,0,sizeof(pitch));
    memset(&noise, 0, sizeof(noise));
    memset(&vocoderInput, 0, sizeof(vocoderInput));
    memset(&synthezised, 0, sizeof(synthezised));
    int last = 1;
    int lastPulse = 1;
    int i,j,k,l,m;
    for (i = Fs*0.03; i<length_data-0.025*Fs; i= i+step) {
        int lastSpeech = i+1-0.015*Fs;
        int nextSpeech = i+0.015*Fs;
        int lastPitch = i+1-0.025*Fs;
        int nextPitch = i+0.025*Fs;
        float y_filt[nextSpeech-lastSpeech],y_pitch[nextPitch-lastPitch];
        
        for (j = 0; j<nextSpeech-lastSpeech; j++) {
            y_filt[j] = windowSpeech[j]*data[j];
        }
        for (k = 0; k<nextPitch-lastPitch; k++) {
            y_pitch[k] = windowPitch[k]*data[k];
        }
        float r_y[nextSpeech-lastSpeech];
        autocorr(r_y,y_filt);
        float A[P];
        LevinsonDurbin(r_y,A,P);
        float pitchProperties[2];
        findPitchAndVoice(y_filt,pitchProperties,Fs);
        
        if (pitchProperties[1] >= alpha){
            last = i+0.01*Fs-pitchProperties[0]-1;
            for (m = lastPulse; m<i+0.01*Fs; m = m+pitchProperties[0]){
                pitch[m] = 1;
                if (m > last){
                    lastPulse = m;
                }
            }
        }else{
            lastPulse = i+0.01*Fs;
            float randNoise[step];
            rand_gauss(randNoise,step);
            
            for (l = 0; l<step; l++) {
                noise[i-halfStep+l] = beta*randNoise[l];
            }
            
        }
        for (j = 0; j<length_data; j++) {
            vocoderInput[j] = pitch[j] + noise[j];
            if(vocoderInput[j] >= 1){
                vocoderInput[j] = 1;
            }
        }
        float temp[step],tempSynthesized[step];
        for (k = 0; k<0.02*Fs; k++) {
            temp[k] = vocoderInput[i-halfStep+k];
        }
        float B[1] = {1};
        filtrate(tempSynthesized,B,1,A,P,temp);
        for (l = 0; l<step; l++) {
            synthezised[i-halfStep+l] = tempSynthesized[l];
        }
    }
    
    //Lowpassfiltrate synthezised
    
    //output = synthesized
    
    for (i = 0; i<N; i++) {
        output[i]= synthezised[i];
    }
}
