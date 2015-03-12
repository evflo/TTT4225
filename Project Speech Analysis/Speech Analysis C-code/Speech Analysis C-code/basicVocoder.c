//
//  basicVocoder.c
//  Speech Analysis C-code
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include "basicVocoder.h"
#include "SignalProcessing.h"
#include "filtProg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    int max = 0;
    for (k = minima; k<N; k++) {
        if (max< pitchFrame[k]) {
            pitchPos = k;
            max = pitchFrame[k];
        }
    }
    int pitchPeriod = pitchPos + minima + 0.002*Fs - 3;
    float pitchRatio = ry[pitchPeriod+1]/ry[0];
    pitchProperties[0] = pitchPeriod;
    pitchProperties[1] = pitchRatio;
}

void basicVocoder(float* data,float* output,int length_data, int P){
	//Defining constant variables
	int Fs = 16000;
	int Fc = 4000;
	int N = 8;
	int step = 0.02*Fs;
	int halfStep = step*0.5;
	int speechLength = 0.03*Fs;
	float alpha = 0.5,beta = 0.1;
	int pitchLength = 0.05*Fs;
	int end = length_data-0.025*Fs;
	//Defining changing variables
	int last = 1;
	int lastPulse = 1;
	int lastSpeech, nextSpeech, lastPitch, nextPitch;
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
	float* yFiltrated = (float*) calloc(speechLength,sizeof(float));
	float* yPitch = (float*) calloc(pitchLength,sizeof(float));
	float* ry = (float*) calloc(speechLength,sizeof(float));
	float* A = (float*) calloc(P,sizeof(float));
	float B[1] = {1};
	//Making the Hamming Windows for speech and pitch
	hammingWindow(speechLength,windowSpeech);
	hammingWindow(pitchLength,windowPitch);
	

	
	for (i = Fs*0.03; i<end; i= i+step) {

		lastSpeech = i+1-0.015*Fs;
		nextSpeech = i+0.015*Fs;
		lastPitch = i+1-0.025*Fs;
		nextPitch = i+0.025*Fs;


		for (j = 0; j<nextSpeech-lastSpeech; j++) {
		    yFiltrated[j] = windowSpeech[j]*data[lastSpeech+j];
		}
			
		for (k = 0; k<nextPitch-lastPitch; k++) {
		    yPitch[k] = windowPitch[k]*data[lastPitch+k];
		}
		
		autocorr(yFiltrated,speechLength,ry);


		LevinsonDurbin(ry,A,P);
		
		findPitchAndVoice(yPitch,pitchLength,pitchProperties,Fs);

		if (pitchProperties[1] >= alpha){

		    last = i+0.01*Fs-pitchProperties[0]-1;


		    for (m = lastPulse; m<i+0.01*Fs; m = m+pitchProperties[0]){

				pitch[m] = 1;
				lastPulse = m;
		    }

		}else{

		    lastPulse = i+0.01*Fs;

		    rand_gauss(randNoise,step);
		    
		    for (l = 0; l<step; l++) {
			noise[i-halfStep+l] = beta*randNoise[l];
			vocoderInput[i-halfStep+l] = pitch[i-halfStep+l] + noise[i-halfStep+l];
			if (vocoderInput[i-halfStep+l] > 1){
		    	vocoderInput[i-halfStep+l] = 1;
		    }
		    vocoderInputSample[l] = vocoderInput[i-halfStep+l];
		    }
		    
		}
		filtrate(vocoderInputSample,B,1,A,P);

		for (l = 0; l<step; l++) {
		    synthezised[i-halfStep+l] = vocoderInputSample[l];
		}

	}


	for (i = 0; i<length_data; i++) {
		data[i]= synthezised[i];
	}

	//filtprog
	firFilter(coeff,N,data,output, length_data);

	free(yPitch), free(yFiltrated), free(synthezised), free(vocoderInputSample), free(vocoderInput);
	free(pitch), free(pitchProperties), free(windowPitch), free(windowSpeech),free(ry),free(randNoise),free(A);
}