//
//  Signal Processing.c
//  Speech Analysis C-code
//
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

//Creates a hamming window on the empty array hamming in the input of length L
//[Source: http://se.mathworks.com/help/signal/ref/hamming.html]
void hammingWindow(float* hamming,int L){
    //Initialize the variables to be used in the for-loop
    int i;
    float x;
    int N = L-1;
    //Constant used in the formula for Hamming window
    float alpha = 0.54;
    float beta = 0.46;
    for (i = 0; i<=L-1; i++) {
        x = 2.0*3.14*(float)i;
        x = x/(float)N;
        //Making sure make the hamming[i] a float variable by casting
        //the cos-function from double to float
        hamming[i] = alpha - beta*(float)cos((double)x);
    }
}
//Creates the autocorrelation of the input x of length lengthx
//The output correlation is created on the float variable rx
void autocorr(float* x,int lengthx,float* rx){
    //Declare the array and counting variables to be used in the for-loops
    float y[2*lengthx+1];
    int i,j,k,l;
    //Copying values in x(n) to y(n) with length 2*lengthx+1
    //Filling with zeros outside the length of x(n)
    //This is done to ensure we do not getting accessing problems because
    //of indexing outside the array of x(n)
    for (i = 0; i<2*lengthx+1; i++) {
        if (i<lengthx) {
            y[i] = x[i];
        }else{
            y[i] = 0;
        }
    }
    //The values of non-normalized autocorrelation will be stored in C
    float *C = (float *) calloc(lengthx, sizeof(float));
    for (j=0; j<lengthx; j++) {
        for (k=0; k<lengthx; k++) {
            //Definiton of autocorrelation
            C[j] +=  y[k]*y[k+j];
        }
    }
    //Normalized autocorrelation stored in rx(n) where the biggest value
    //r(0) = 1
    for (l =0 ; l<lengthx; l++) {
        rx[l] = C[l]/C[0];
    }
    free(C);
}
//Creates LP-coefficients based on the autocorrelation
//The length of A is given by prediction order P
void LevinsonDurbin(float* r,float* A,int P){
    //Indexing variables and arrays
    float* b;
    float* k;
    b = (float*) calloc(P+1,sizeof(float));
    k = (float*) calloc(P+1,sizeof(float));
    float E = r[0];
    int i,j,l;
    memset(A,0,(P+1)*sizeof(float));
    //Set the value of first coefficient to 1, and copy it on b[0]
    A[0] = 1;
    b[0] = 1;
    for (i = 1; i<=P; i++) {
        for (j = 1; j<i; j++) {
            k[i] += A[j]*r[i-j];

        }
        //Adjusting k by the prediction error power

        k[i] = (r[i]-k[i])/E;
        A[i] = k[i];
        //Computing b for found A and k

        for (j=1; j<i; j++) {
            b[j] = A[j] - k[i]*A[i-j];
        }
        //Copying the values of the temporary array b in to A
        for (l = 1; l<i; l++) {
            A[l] = b[l];
                 
        }
        //Prediction error power for i'th power
        E = (1- k[i]*k[i])*E;

    }
    //Inverting the coefficients to make it easier to used in filtrating
    for(i = 1; i<= P;i++){
        A[i]= -A[i];
    }
    free(b);
    free(k);
}

//Filtrates x based on the coefficients given in A and B, and returns the output in y
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

//Determines if a speech is voiced or unvoiced based on the autocorrelation
void findPitchAndVoice(float* y_pitch,int pitchLength,float* pitchProperties,int Fs){
    //Deciding the work area
    int N = floor(0.02*Fs)-floor(0.002*Fs);
    
    float ry[pitchLength];
    autocorr(y_pitch,pitchLength, ry);
    int i,j,minima = 0;
    float pitchFrame[N];
    int m = 0;
    //We will only use the parts of the autocorrelation which is in the work area
    //defined by N
    for (i = floor(0.002*Fs); i<floor(0.02*Fs); i++) {
        pitchFrame[m] = ry[i];
        m++;
    }
    //Finds where the frame is less then zeros or sets minima as the length of the pitchFrame
    //if no minima is found
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
    float max = 0.0;
    //Finds the pitch positions
    for (k = minima; k<N; k++) {
        if (max< pitchFrame[k]) {
            pitchPos = k;
            max = pitchFrame[k];
        }
        if ((max > ry[0]/2) && (pitchFrame[k] < 0)){ 
            break; //Stops after first peak/bang
        }
    }

    //Sets the value of output values by computed data
    int pitchPeriod = pitchPos + minima + 0.002*Fs - 3;
    float pitchRatio = ry[pitchPeriod+1]/ry[0];
    pitchProperties[0] = pitchPeriod;
    pitchProperties[1] = pitchRatio;
    
}