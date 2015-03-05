//
//  signalProcessing.c
//  SpeechAnalysis
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include "signalProcessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <libsndfile.h>

#define Fs 16000
#define Fc 2000
#define D 4
#define low_P 4
#define filterOrden 8
#define step Fs*0.02

int rand_gauss (float *x, int N){
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


void hammingWindow(int L,float* ham){
    int i;
    for (i = 0; i<L-1; i++) {
        ham[i] = 0.54-0.46*cos(2*3.14*(i/(L-1)));
    }
}

void autocorr(float* x,float* rx){
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
    for (l =0 ; l<N; l++) {
        rx[l] = C[l];
    }
}

void LevinsonDurbin(float* r,float* A,int P){
    
    float E = r[0];
    float b[P];
    float k[P];
    b[1] = 1;
    int i,j,l;
    for (i = 0; i<P; i++) {
        k[i] = r[i];
        memset(A,0, i);
        
        A[0] = 1;
        for (j = 1; j<=i; j++) {
            k[i] = k[i]+b[j]*r[i-j+1];
        }
        
        k[i] = -k[i]/E;
        
        for (j=1; j<i; j++) {
            A[j] = b[j] + k[i]*b[i-j+1];
        }
        A[i+1] = k[i];
        E = (1- abs(k[i])^2)*E;
        
        for (l = 0; l<P; l++) {
            b[l] = A[l];
        }
    }
    
}

void filtrate(float* y,float* B,int sizeB,float* A,int sizeA,float* x){
    int i,j,k,l;
    
    memset(&y,0,sizeof(y));
    for (i = 0; i<step; i++) {
        for (j = 0; j<sizeA; j++) {
            for (k= 0; k<sizeB; k++) {
                for (l = 0; l<i; l++) {
                    if (l== 0){
                        y[i] += B[l]*x[i-l];
                    }else{
                        y[i] += B[l]*x[i-l]-A[l]*y[i-l];
                    }
                }
            }
        }
        y[i] = (1/A[0])*y[i];
    }
}
