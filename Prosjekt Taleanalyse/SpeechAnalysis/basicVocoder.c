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


int* hammingWindow(int L){
    int i;
    int ham[L];
    for (i = 0; i<L-1; i++) {
        ham[i] = 0.54-0.46*cos(2*3.14*(i/(L-1)));
    }
    return ham;
}

int* autocorr(int* x){
    int N = sizeof(x);
    int y[2*N+1];
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
    int r[N];
    for (l =0 ; l<N; l++) {
        r[l] = C[l];
    }
    return r;
}

int* LevinsonDurbin(int* r,int P){
    
    int E = r[0];
    int b[P];
    int k[P];
    int a[P];
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
int* findPitchAndVoice(y_pitch,Fs){
    
    int* ry = autocorr(&y_pitch);
    
    return <#expression#>
    
}

struct defined{
    int Fs;
    int Fc;
    int N;
    int alpha;
    int beta;
};
void basicVocoder(int* data, int P){
    int Fs = 16000;
    int Fc = 4000;
    
    int N = 8;
    
    int alpha = 0.5;
    int beta= 0.1;
    int pitch[sizeof(data)],noise[sizeof(data)],vocoderInput[sizeof(data)],synthezised[sizeof(data)];
    int* windowSpeech = hammingWindow(0.03*Fs);
    int* windowPitch = hammingWindow(0.05*Fs);
    memset(&pitch,0,sizeof(data));
    memset(&noise, 0, sizeof(data));
    memset(&vocoderInput, 0, sizeof(data));
    memset(&synthezised, 0, sizeof(data));
    int last = 1;
    int lastPulse = 1;
    int i,j,k;
    for (i = Fs*0.03; i<sizeof(data)-0.025*Fs; i= i+Fs*0.02) {
        int lastSpeech = i+1-0.015*Fs;
        int nextSpeech = i+0.015*Fs;
        int lastPitch = i+1-0.025*Fs;
        int nextPitch = i+0.025*Fs;
        int y_filt[nextSpeech-lastSpeech],y_pitch[nextPitch-lastPitch];
        
        for (j = 0; j<nextSpeech-lastSpeech; j++) {
            y_filt[j] = windowSpeech[j]*data[j];
        }
        for (k = 0; k<nextPitch-lastPitch; k++) {
            y_pitch[k] = windowPitch[k]*data[k];
        }
        
        int* r_y = autocorr(y_filt);
        
        int* A = LevinsonDurbin(r_y,P);
        
        
        
    }
}
