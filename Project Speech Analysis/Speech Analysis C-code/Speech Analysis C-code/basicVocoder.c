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
#include <time.h>


void basicVocoder(float* data,float* output,int length_data, int P){
	srand(time(NULL));
	//Defining constant variables
	int Fs = 16000;
	int N = 8;
	int step = 0.02*Fs;
	int halfStep = step*0.5;
	int speechLength = 0.03*Fs;
	float alpha = 0.5,beta = 0.1;
	int pitchLength = 0.05*Fs;
	int end = length_data-0.025*Fs;
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
	float* A = (float*) calloc(P,sizeof(float));
	float B[1] = {1};
	//Making the Hamming Windows for speech and pitch
	hammingWindow(windowSpeech,speechLength);
	hammingWindow(windowPitch,pitchLength);

	float lowCoeff[9] = {0, -0.0277, 0, 0.274,0.4974, 0.274, 0, -0.0227, 0};
	//int test = 0;
	for (i = Fs*0.03; i<end; i= i+step) {
		//printf("Test: %d\n", ++test);
		lastSpeech = i+1-0.015*Fs;
		nextSpeech = i+0.015*Fs;
		lastPitch = i+1-0.025*Fs;
		nextPitch = i+0.025*Fs;


		for (j = 0; j<nextSpeech-lastSpeech; j++) {
		    yFiltrated[j] = windowSpeech[j]*data[lastSpeech+j];
		}
		/*if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
			printf("yFiltrated, i = %d:\n", i);
			printf("%g, %g, %g\n", yFiltrated[0], yFiltrated[100], yFiltrated[300]);
		}*/
		for (k = 0; k<nextPitch-lastPitch; k++) {
		    yPitch[k] = windowPitch[k]*data[lastPitch+k];
		}
		
		autocorr(yFiltrated,speechLength,ry);
		/*if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
			printf("autocorr, i = %d:\n", i);
			printf("%g, %g, %g\n", ry[0], ry[100], ry[300]);
		}*/
		LevinsonDurbin(ry,A,P);
		/*if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
			printf("Levinson, A, i = %d:\n", i);
			for (j = 0; j < P; j++){
				printf("%f\n", A[j]);
			}
		}*/
		findPitchAndVoice(yPitch,pitchLength,pitchProperties,Fs);

		if (pitchProperties[1] >= alpha){

		    last = i+0.01*Fs-pitchProperties[0]-1;
		    //printf("Pitch period: %g\n", pitchProperties[0]);

		    for (m = lastPulse; m<i+0.01*Fs; m = m+pitchProperties[0]){

				pitch[m] = 1;
				lastPulse = m;
		    }

		}else{

		    lastPulse = i+0.01*Fs;

		    rand_gauss(randNoise,step);
		    /*if ((i == Fs*0.03+Fs*0.02*3) || (i == Fs*0.03+Fs*0.02*15) || (i == Fs*0.03+Fs*0.02*30)){
				printf("randNoise, A, i = %d:\n", i);
				printf("%g, %g, %g\n", randNoise[0], randNoise[100], randNoise[300]);	
			}*/
		    for (l = 0; l<step; l++) {
			noise[i-halfStep+l] = beta*randNoise[l];
			vocoderInput[i-halfStep+l] = pitch[i-halfStep+l] + noise[i-halfStep+l];
			if (vocoderInput[i-halfStep+l] > 1){
		    	vocoderInput[i-halfStep+l] = 1;
		    }
		    vocoderInputSample[l] = vocoderInput[i-halfStep+l];
		    }
		    
		}
		filtering = filtrate(vocoderInputSample,step,B,1,A,P, vocoderInputSampleFilt);
		if(filtering == -1){
			printf("Filtrate function failed: Size of A is equal to or smaller then 1\n");
			return;
		}
		for (l = 0; l<step; l++) {
		    synthezised[i-halfStep+l] = vocoderInputSampleFilt[l];
		}

	}

	for (i = 0; i<length_data; i++) {
		data[i]= synthezised[i]; //Should be data
	}
	printf("Before filtrate: %g %g %g\n",vocoderInput[1000],vocoderInput[5000],vocoderInput[10000]);
	//filtprog
	printf("Data before FIR and after filtrate: %g, %g, %g\n", data[1000], data[5000], data[10000]);
	firFilter(lowCoeff,N,data,output, length_data);
	printf("Output after FIR: %g, %g, %g\n", output[1000], output[5000], output[10000]);
	
	float maxVal = 0;
	for (i = 0; i < length_data; i++){
		if (abs(output[i]) > maxVal){
			maxVal = abs(output[i]);
		}
	}
	for (i = 0; i < length_data; i++){
		output[i] = output[i] / maxVal;
	}
printf("Output after FIR and first gain adjustment: %g, %g, %g\n", output[1000], output[5000], output[10000]);

	free(yPitch),free(yFiltrated),free(synthezised),free(vocoderInputSample),free(vocoderInput),free(vocoderInputSampleFilt);
	free(pitch),free(pitchProperties),free(windowPitch),free(windowSpeech),free(ry),free(randNoise),free(A);

}

