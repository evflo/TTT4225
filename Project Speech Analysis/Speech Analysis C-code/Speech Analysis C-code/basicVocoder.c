// Takes an audio-input as the array 'data' of length 'length_data', analyzes and regenerates
// the signal based on the signal properties using linear predictive coding (LPC) with 'P'
// filter coefficients and stores the regenerated signal in 'output' 

#include "basicVocoder.h"
#include "SignalProcessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


void basicVocoder(float* data,float* output,int length_data, int P){
	srand(time(NULL));					// Set seed for the random-function used to generate white-noise
	//Defining constant variables
	const int Fs = 16000;				// Sampling frequency
	const int N = 8; 					// Filter order for FIR low-pass filter
	const int step = 0.02*Fs;
	const int halfStep = step*0.5;
	const int speechLength = 0.03*Fs;
	const float alpha = 0.5;			// Threshold for voiced or unvoiced
	const float beta = 0.1;				// Gain for white-noise
	const int pitchLength = 0.05*Fs;
	const int end = length_data-0.025*Fs;
	
	//Defining changing variables
	int lastPulse = 1;
	int lastSpeech, nextSpeech, lastPitch, nextPitch,filtering;
	int i,j,k,l,m;
	//Defining array that will be processed
	float* pitch = (float*) calloc(length_data,sizeof(float));
	float* noise = (float*) calloc(length_data,sizeof(float));
	float* vocoderInput = (float*) calloc(length_data,sizeof(float));
	float* synthezised = (float*) calloc(length_data,sizeof(float));
	float* pitchProperties = (float*) calloc(2,sizeof(float));
	float* randNoise = (float*) calloc(step,sizeof(float));
	float* windowSpeech = (float*) calloc(speechLength,sizeof(float));
	float* windowPitch = (float*) calloc(pitchLength,sizeof(float));
	float* vocoderInputSample = (float*) calloc(step,sizeof(float));
	float* vocoderInputSampleFilt = (float*) calloc(step,sizeof(float));
	float* yFiltrated = (float*) calloc(speechLength,sizeof(float));
	float* yPitch = (float*) calloc(pitchLength,sizeof(float));
	float* ry = (float*) calloc(speechLength,sizeof(float));
	float* A = (float*) calloc(P+1,sizeof(float));
	float B[1] = {1};
	//Making the Hamming Windows for speech and pitch
	hammingWindow(windowSpeech,speechLength);
	hammingWindow(windowPitch,pitchLength);
<<<<<<< Updated upstream
	// Low-pass filter coefficients, calculated in MATLAB
=======
>>>>>>> Stashed changes
	float lowCoeff[9] = {0, -0.0277, 0, 0.274,0.4974, 0.274, 0, -0.0227, 0};

	// Iterating through the signal, analyzing and regenerating the signal 20ms at a time.
	//'i' marks the middle of the 20ms frame.
	for (i = Fs*0.03; i<end; i= i+step) {
		//Set boundaries for the signal segments
		lastSpeech = i+1-0.015*Fs;
		nextSpeech = i+0.015*Fs;
		lastPitch = i+1-0.025*Fs;
		nextPitch = i+0.025*Fs;

		// Shape the relevant signal segments using Hamming-windows
		for (j = 0; j<nextSpeech-lastSpeech; j++) {
		    yFiltrated[j] = windowSpeech[j]*data[lastSpeech+j];
		}
		for (k = 0; k<nextPitch-lastPitch; k++) {
		    yPitch[k] = windowPitch[k]*data[lastPitch+k];
		}
		
		// Find the auto-correlation of the signal segment.
		autocorr(yFiltrated,speechLength,ry);
		// Levinson-Durbin recursion is used on the auto-correlation of the signal to find filter coefficients
		LevinsonDurbin(ry,A,P);
		// Find the period of the pitch and a voiced constant (between 0 and 1, larger for voiced sounds)
		// and store them as element 0 and 1 respectively in the array 'pitchProperties'
		findPitchAndVoice(yPitch,pitchLength,pitchProperties,Fs);

		// Generate a signal using either white noise or pulse trains depending on the voiced properties
		// of the signal segment.
		if (pitchProperties[1] >= alpha){
<<<<<<< Updated upstream
			// When the signal is voiced, make a pulse train of "dirac"-pulses with intervals
			// defined by the previously found pitch period.
			int pp = (int) (pitchProperties[0]);
		    for (m = lastPulse; m<i+0.01*Fs; m = m+pp){
=======
			printf("Pitch: %g\n", pitchProperties[0]);
		    //last = i+0.01*Fs-pitchProperties[0]-1;
		    //printf("Pitch period: %g\n", pitchProperties[0]);
			int pp = (int) (pitchProperties[0]*0.3);
			int test = 0;
		    for (m = lastPulse; m<i+0.01*Fs; m = m+pp){
		    	printf("test:%d\t", test++);
>>>>>>> Stashed changes
				pitch[m] = 1;
				lastPulse = m;
		    }
		}else{
			// When the signal isn't voiced, white noise is used as a signal, scaled down with 'beta' gain.
		    lastPulse = i+0.01*Fs;
			// Generate white-noise
		    rand_gauss(randNoise,step);
		    for (l = 0; l<step; l++) {
				noise[i-halfStep+l] = beta*randNoise[l];
			}
		}
		
		for (l = 0; l < step; l++){
			vocoderInput[i-halfStep+l] = pitch[i-halfStep+l] + noise[i-halfStep+l];
			// Adjusts numbers that are too large, in the rare case of white noise getting very large values.
			if (vocoderInput[i-halfStep+l] > 1){
				vocoderInput[i-halfStep+l] = 1;
			}
			vocoderInputSample[l] = vocoderInput[i-halfStep+l];
		}
		
		// Filter the generated signal using the filter coefficients found by the Levinson-Durbin recursion.
		filtering = filtrate(vocoderInputSample,step,B,1,A,P, vocoderInputSampleFilt);
		if(filtering == -1){
			printf("Filtrate function failed: Size of A is equal to or smaller then 1\n");
			return;
		}
		for (l = 0; l<step; l++) {
		    synthezised[i-halfStep+l] = vocoderInputSampleFilt[l];
		}
	}
	
	// Low-pass filter the generated signal to remove spurious high frequency elements.
	firFilter(lowCoeff,N,synthezised,output, length_data);
	
	// The signal should have values from -1 to 1, the signal is adjusted to comply
	// with this constraint
	float maxVal = 0;
	for (i = 0; i < length_data; i++){
		if (fabsf(output[i]) > maxVal){
			maxVal = fabsf(output[i]);
		}
	}
	for (i = 0; i < length_data; i++){
		output[i] = output[i] / maxVal;
	}

	//Free dynamically allocated memory
	free(yPitch),free(yFiltrated),free(synthezised),free(vocoderInputSample),free(vocoderInput),free(vocoderInputSampleFilt);
	free(pitch),free(pitchProperties),free(windowPitch),free(windowSpeech),free(ry),free(randNoise),free(A);
}