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



int rand_gauss (float *x, int N){
    /* Create Gaussian N(0,1) distributed numbers from uniformly distributed numbers using */
    /* the Box-Muller transform as described in                                            */
    /* D. E. Knuth: The Art of Computer Programming, Vol II, p 104                         */
    float v1,v2,s;
    int i, j, M;
    
    M=N/2;
    
    // Initialize uniform number generator
    
    
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


void hammingWindow(float* hamming,int L){
    int i;
    float x;
    int N = L-1;
    float alpha = 0.54;
    float beta = 0.46;
    for (i = 0; i<=L-1; i++) {
        x = 2.0*3.14*(float)i;
        x = x/(float)N;
        hamming[i] = alpha - beta*(float)cos((double)x);
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
    float *C = (float *) calloc(lengthx, sizeof(float));
    for (j=0; j<lengthx; j++) {
        for (k=0; k<lengthx; k++) {
            C[j] +=  y[k]*y[k+j];
        }
    }
    for (l =0 ; l<lengthx; l++) {
        rx[l] = C[l]/C[0];
    }
    free(C);
}


/*

void altLevDeb(float* r, float* A, int P){
    A[0] = 1.0;
    float K = -r[1]/r[0];
    A[1] = K;
    float Alpha = r[0]* (1-K*K);
    int i, j;
    float S;
    float* An = (float*) calloc(P+1, sizeof(float));
    for (i = 2; i <= P; i++){
        S = r[i];
        for (j = 1; j <= i-1; j++){
            S += r[j] * A[i-j];
        }
        K = -S / Alpha;
        for (j = 1; j <= i-1; j++){
            An[j] = A[j] + K * A[i-j];
        }
        An[i] = K;
        Alpha = Alpha * (1-K*K);
        for (j = 1; j <=i; j++){
            A[j] = An[j];
        }
    }
    free(An);
}


double** forward_substitution(double **coefficient_matrix, int size){
    int max, i, j, k;
    double t;
    for (i = 0; i < size; i++) {
        max = i;
        for (j = i + 1; j < size; j++)
            if (coefficient_matrix[j][i] > coefficient_matrix[max][i])
                max = j;
        for (j = 0; j < size + 1; j++) {
            t = coefficient_matrix[max][j];
            coefficient_matrix[max][j] = coefficient_matrix[i][j];
            coefficient_matrix[i][j] = t;
        }
        for (j = size; j >= i; j--)
            for (k = i + 1; k < size; k++)
                coefficient_matrix[k][j] -= coefficient_matrix[k][i] / coefficient_matrix[i][i] * coefficient_matrix[i][j];
    }
        return coefficient_matrix;
}

double** reverse_elimination(double **coefficient_matrix, int size){
    int i, j;
    for (i = size - 1; i >= 0; i--) {
        coefficient_matrix[i][size] = coefficient_matrix[i][size] / coefficient_matrix[i][i];
        coefficient_matrix[i][i] = 1;
        for (j = i - 1; j >= 0; j--) {
            coefficient_matrix[j][size] -= coefficient_matrix[j][i] * coefficient_matrix[i][size];
            coefficient_matrix[j][i] = 0;
        }
    }
    return coefficient_matrix;
}

double** gauss(double **coefficient_matrix, int size) {
    coefficient_matrix = forward_substitution(coefficient_matrix, size);
    return reverse_elimination(coefficient_matrix, size);
}

float* calculate_lpc_coefficients(double *correlation_values, int size){
    double **coefficient_matrix = (double **)malloc(size * sizeof(double*));
    int i, j;
    for (i = 0; i < size; i++) coefficient_matrix[i] = (double*)malloc((size + 1) * sizeof(double));
    
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j)
            coefficient_matrix[i][j] = correlation_values[abs(i - j)];
    
    for (i = 0; i < size; i++)
        coefficient_matrix[i][size] = correlation_values[i + 1];
    
    coefficient_matrix = gauss(coefficient_matrix, size);
    double* features = (double*)malloc(sizeof(double)*size);
    float* featuresFloat = (float*)malloc(sizeof(float)*size);
    for (i = 0; i < size; i++){
        features[i] = coefficient_matrix[i][size];
        featuresFloat[i] = (float) features[i];
    }
    return featuresFloat;
}
*/
void lpc(float* r,float* A,int P);
void LevinsonDurbin(float* r,float* A,int P){
    //altLevDeb(r,A,P);
    
    float* b;
    float* k;
    b = (float*) calloc(P+1,sizeof(float));
    k = (float*) calloc(P+1,sizeof(float));
    float E = r[0];
    int i,j,l;
    memset(A,0,(P+1)*sizeof(float));
    A[0] = 1;
    b[0] = 1;
    for (i = 1; i<=P; i++) {
        //k[i] = 0.0;

    
        for (j = 1; j<i; j++) {
            k[i] += A[j]*r[i-j];

        }
        
        k[i] = (r[i]-k[i])/E;
        A[i] = k[i];
        for (j=1; j<i; j++) {
            b[j] = A[j] - k[i]*A[i-j];
        }
        for (l = 1; l<i; l++) {
            A[l] = b[l];
                 
        }
        E = (1- k[i]*k[i])*E;

    }
    for(i = 1; i<= P;i++){
        A[i]= -A[i];
    }
    free(b);
    free(k);
}


int filtrate(float* x,int lengthx,float* B,int sizeB,float* A,int sizeA,float* y){
	int i,j,k;
    sizeA++;
    memset(y,0,lengthx*sizeof(float));
    if (sizeA > 1){
    	for (i = 0; i<lengthx; i++) {
    		for (j = 1; j<sizeA; j++) {
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
    }else{
        return -1;
    }
    return 0;
}
/*
void filtrate(float* x, int np, float *b, int ordB, float *a, int ordA, float* y){
    int i,j;
    y[0]=b[0]*x[0];
    for (i=1;i<ord+1;i++)
    {
        y[i]=0.0;
        for (j=0;j<i+1;j++)
            y[i]=y[i]+b[j]*x[i-j];
        for (j=0;j<i;j++)
            y[i]=y[i]-a[j+1]*y[i-j-1];
    }


for (i=ord+1;i<np+1;i++)
{
    y[i]=0.0;
        for (j=0;j<ord+1;j++)
        y[i]=y[i]+b[j]*x[i-j];
        for (j=0;j<ord;j++)
        y[i]=y[i]-a[j+1]*y[i-j-1];
}
} 



*/

//Takes a signal x of length N, downsamples the signal with a factor D,
//and puts the output in xDec of length N/D.
void decimate(float *x, float *xDec, int N, int D){
	int i;
	int c = 0;
	for (i = 0; i < N; i += D){
		xDec[c] = x[i];
		c++;
	}
}

//Takes a signal xDec of length N/D, upsamples the signal with a factor D,
//and puts the output in x of length N.
void upsample(float *xDec, float *x, int N, int D){
	int i;
	int c = 0;
	for (i = 0; i < N; i++){
		if (i%D){
			x[i] = 0;
		}else{
			x[i] = xDec[c];
			c++;
		}
	}
}


//Takes in a set of Ncoeffs filter coefficents and filters the input x
//Output xFiltred is the filtered x

void firFilter (float *coeff, int Ncoeffs,
               float *x, float *xFiltred, int n)
{
  int i, j, k;
  float tmp;

  for (k = 0; k < n; k++)  //  position in output
  {
    tmp = 0;

    for (i = 0; i < Ncoeffs; i++)  //  position in coefficients array
    {
      j = k - i;  //  position in input

      if (j >= 0)  //  bounds check for input buffer
      {
        tmp += coeff[i] * x[j];
      }
    }

    xFiltred[k] = tmp;
  }
}

void findPitchAndVoice(float* y_pitch,int pitchLength,float* pitchProperties,int Fs){
    //printf("Input findPitch: %f %f %f\n", y_pitch[0], y_pitch[1], y_pitch[2]); 
    int N = floor(0.02*Fs)-floor(0.002*Fs);
    float ry[pitchLength];
    autocorr(y_pitch,pitchLength, ry);
    int i,j,minima = 0;
    float pitchFrame[N];
    int m = 0;
    for (i = floor(0.002*Fs); i<floor(0.02*Fs); i++) {
        pitchFrame[m] = ry[i];
        m++;
    }
    
    int foundMinima = 0;
    for (j = 0; j<N; j++) {
        if (pitchFrame[j] <= 0) {
            //printf("%g, %g\n", pitchFrame[j], ry[0]);
            minima= j;
            foundMinima = 1;
       // printf("Found minima, %d\n", minima);
            break;
        }
    }
    if(foundMinima == 0){
        minima = N;
    }
    int k,pitchPos = 0;
    float max = 0.0;
    for (k = minima; k<N; k++) {
        if (max< pitchFrame[k]) {
            pitchPos = k;
            max = pitchFrame[k];
        }
        if ((max > ry[0]/2) && (pitchFrame[k] < 0)){ //Might need some fine-tuning, or might be better to remove 
            break; //Stops after first peak/bang
        }
    }
    //printf("%g\n", max);
    int pitchPeriod = pitchPos + minima + 0.002*Fs - 3;
    float pitchRatio = ry[pitchPeriod+1]/ry[0];
    pitchProperties[0] = pitchPeriod;
    pitchProperties[1] = pitchRatio;
    /*if ((pitchPeriod == 63)||(pitchPeriod == 179)){
        printf("ry[0]: %g\n", ry[0]);
        for (i = 0; i < N; i++){
            printf("%d : %6.4g\t|", i, pitchFrame[i]);
            if (i%4 == 0){
                printf("\n");
            }
        }
        printf("\n");
    }*/
}