//
//  Signal Processing.c
//  Speech Analysis C-code
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include "SignalProcessing.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

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

void autocorr(float* x,int lengthx,float* rx){

    float y[2*lengthx+1];
    int i,j,k,l;
    for (i = 0; i<2*lengthx+1; i++) {
        if (i<lengthx) {
            y[i] = x[i];
        }else{
            y[i] = 0;
        }
    }
    float C[lengthx];
    for (j=0; j<lengthx; j++) {
        for (k=0; k<lengthx; k++) {
            C[j] +=  y[k]*y[k+j];
        }
    }
    for (l =0 ; l<lengthx; l++) {
        rx[l] = C[l];
    }
}

void LevinsonDurbin(float* r,float* A,int P){
    
    double* b = (double*) calloc(P,sizeof(double));
    double* k = (double*) calloc(P,sizeof(double));
    double E = r[0];
    int i,j,l;
    memset(A,0,P);
    A[0] = 1;
    b[0] = 1;
    for (i = 1; i<=P; i++) {
        k[i] = 0.0;

    
        for (j = 1; j<=i-1; j++) {
            k[i] += b[j]*r[i-j];

        }
        
        k[i] = (r[i]-k[i])/E;
        A[i] = (float)k[i];
        for (j=1; j<=i-1; j++) {
            A[j] = (float)b[j] - (float)k[i]*(float)b[i-j];
        }
        for (l = 0; l<P; l++) {
            b[l] = A[l];
            
        }
        E = (1- k[i]*k[i])*E;

    }
    for(i = 1; i<= P;i++){
        A[i]= -A[i];
    }

}
void filtrate(float* x,float* B,int sizeB,float* A,int sizeA){
	int i,j,k,l;
	int y_length = step;
	float y[y_length];

	for (i = 0; i<step; i++) {
		for (j = 0; j<sizeA; j++) {
	    		for (k= 0; k<sizeB; k++) {

	       			if (k <= i){
		   			y[i] += B[k]*x[i-k];
				}
				if (j <= i){
					y[i] -= A[j]*y[i-j];
				}
			}		
		}
		y[i] = (1/A[0])*y[i];
	}
	for (l = 0; l<step;l++){
		x[l] = y[l];
	}
}
