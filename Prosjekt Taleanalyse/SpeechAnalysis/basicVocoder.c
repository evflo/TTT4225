//
//  basicVocoder.c
//  SpeechAnalysis
//
//  Created by Even on 03/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include "basicVocoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <libsndfile.h>



int rand_gauss (float *x, int N) {
    /* Create Gaussian N(0,1) distributed numbers from uniformly distributed numbers using */
    /* the Box-Muller transform as described in                                            */
    /* D. E. Knuth: The Art of Computer Programming, Vol II, p 104                         */
    float v1,v2,s;
    int i, j, M;
    
    M=N/2;
    
    // Initialize uniform number generator
    srand(time(NULL));
    
    // Loop - each iteration generates two Gaussian numbers
    for (i=0; i<M; i++){
        j=2*i;
        
        // See Knuth or http://en.wikipedia.org/wiki/Box_Muller_transform
        // for algorithm description
        do {
            v1 = 2.0 * ((float) rand()/RAND_MAX) - 1;
            v2 = 2.0 * ((float) rand()/RAND_MAX) - 1;
            
            s = v1*v1 + v2*v2;
        } while ( s >= 1.0 );
        
        if (s == 0.0)
            i=i-1;
        else {
            x[j]=(v1*sqrt(-2.0 * log(s) / s));
            if (j+1<N)
                x[j+1]=(v2*sqrt(-2.0 * log(s) / s));
        }
    }
    return 0;
}


float* hammingWindow(int L){
    int i;
    float ham[L];
    for (i = 0; i<L-1; i++) {
        ham[i] = 0.54-0.46*cos(2*3.14*(i/(L-1)));
    }
    return ham;
}

float* autocorr(float* x){
    const int N = sizeof(x);
    float y[2*N+1];
    int i,j,k,l;
    for (i = 0; i<2*N+1; i++) {
        if (i<N) {
            y[i] = x[i];
        }else{
            y[i] = 0;
        }
    }
    int C[N];
    for (j=0; j<N; j++) {
        for (k=0; k<N; k++) {
            C[j] = C[j] + x[k]*x[k+j-1];
        }
    }
    float r[N];
    for (l =0 ; l<N; l++) {
        r[l] = C[l];
    }
    return r;
}

float* LevinsonDurbin(float* r,int P){
    
    float E = r[0];
    float b[P];
    float k[P];
    float a[P];
    b[1] = 1;
    int i,j,l;
    for (i = 0; i<P; i++) {
        k[i] = r[i];
        memset(a,0, i);
        
        a[0] = 1;
        for (j = 1; j<=i; j++) {
            k[i] = k[i]+b[j]*r[i-j+1];
        }
        
        k[i] = -k[i]/E;
        
        for (j=1; j<i; j++) {
            a[j] = b[j] + k[i]*b[i-j+1];
        }
        a[i+1] = k[i];
        E = (1- abs(k[i])^2)*E;
        for (l = 0; l<length(a); l++) {
            b[l] = a[l];
        }
    }
    return a;
    
}
float* findPitchAndVoice(y_pitch,Fs){
    
    int* ry = autocorr(&y_pitch);
    
    N = floor(0.02*Fs)-floor(0.002*Fs);
    int i,j,minima;
    float pitchRange[N],pitchFrame[N]
    for (i = floor(0.002*Fs); i<floor(0.02*Fs); i++) {
        pitchRange[i] = i;
        pitchFrame[i] = ry[i];
    }
    bool foundMinima = 0;
    for (j = 0; j<N; j++) {
        if (pitchFrame[j] <= 0) {
            minima= j;
            foundMinima = 1;
            break;
        }
    }
    if(foundMinima == 0){
        minima = length(pitchFrame);
    }
    
    int k,pitchPos;
    int max = 0;
    for (k = minima; k<N; k++) {
        if (pitchPos< pitchFrame[k]) {
            pitchPos = k;
            max = pitchFrame[k];
        }
    }
    pitchPeriod = pitchPos + minima + pitchRange[0] - 3;
    pitchRatio = r_x(pitchPeriod+1)/ry(0);
    float output[2] = {pitchPeriod pitchRatio};
    return output;
}
float* filtrate(int* B,int sizeB,int* A,int sizeA,float* x, int N){
    float y[step];
    memset(&y,0,sizeof(y));
    for (i = 0; i<step; i++) {
        for (j = 0; j<sizeA; j++) {
            for (k= 0; k<sizeB; k++) {
                for (l = 0; l<m; n++) {
                    if (l== 0){
                        y(i) += B(l)*x(i-l);
                    }else{
                        y(i) += B(l)*x(i-l)-A(l)*y(i-l);
                    }
                }
            }
        }
        y(i) = (1/A(0))*y(i);
    }
    return y;
}
float* basicVocoder(float* data, int P){
    int Fs = 16000;
    int Fc = 4000;
    
    int N = 8;
    int step = 0.02*Fs;
    int alpha = 0.5;
    int beta= 0.1;
    const int length_data = sizeof(data);
    float pitch[length_data],noise[length_data],vocoderInput[length_data],synthezised[length_data];
    float* windowSpeech = hammingWindow(0.03*Fs);
    float* windowPitch = hammingWindow(0.05*Fs);
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
        
        float* r_y = autocorr(y_filt);
        
        float* A = LevinsonDurbin(r_y,P);
        
        int* pitchProperties = findPitchAndVoice(y_filt,Fs);
        
        if (pitchProperties[1] >= alpha){
            last = i+0.01*Fs-pitchPeriod-1;
            for (m = lastPulse; m<i+0.01*Fs; m = m+pitchProperties[0]){
                pitch(m) = 1;
                if (m > last){
                    lastPulse = m;
                }
            }
        }else{
            lastPulse = i+0.01*Fs;
            float randNoise[step];
            rand_gauss(&randNoise,step);
            
            for (l = 0; l<step; l++) {
                noise[i-0.01*Fs+l] = beta*randNoise[l];
            }

        }
        for (j = 0; j<length_data; j++) {
            vocoderInput[j] = pitch[j] + noise[j];
            if(vocoderInput[j] >= 1){
                vocoderInput[j] = 1;
            }
        }
        float temp[step];
        for (k = 0; k<0.02*Fs; k++) {
            temp[k] = vocoderInput[i-0.01*Fs+k];
        }
        float tempSynthesized = filtrate(1,1,A,P,temp, step);
        for (l = 0; l<step; l++) {
            synthezised[i-0.01*Fs+l] = tempSynthesized[l];
        }
    }
    
    //Lowpassfiltrate synthezised
    return synthezised;
}
