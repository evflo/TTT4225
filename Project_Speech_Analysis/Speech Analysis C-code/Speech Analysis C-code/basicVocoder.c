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



void findPitchAndVoice(float* y_pitch,int pitchLength,float* pitchProperties,int Fs){
    printf("Input findPitch: %f %f %f\n", y_pitch[0], y_pitch[1], y_pitch[2]); 
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
    printf("%f %f %f\n", pitchFrame[0], pitchFrame[1], pitchFrame[2]);
    printf("%f %f %f\n", ry[0], ry[1], ry[2]);
    int foundMinima = 0;
    for (j = 0; j<N; j++) {
        if (pitchFrame[j] <= 0) {
            minima= j;
            foundMinima = 1;
	    printf("Found minima, %d\n", minima);
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


void basicVocoder(float* data,int length_data, int P){
	int Fs = 16000;
	int Fc = 4000;
	int N = 8;
	int step = 0.02*Fs;
	int halfStep = step*0.5;
	int speechLength = 0.03*Fs;
	float alpha = 0.5,beta = 0.1;
	int pitchLength = 0.05*Fs;

	float pitch[length_data],noise[length_data],vocoderInput[length_data],synthezised[length_data], pitchProperties[2],randNoise[step];
	float B[1] = {1};
	float windowSpeech[speechLength],windowPitch[pitchLength],temp[step];
	float y_filt[speechLength],y_pitch[pitchLength],r_y[speechLength],A[P];
	hammingWindow(speechLength,windowSpeech);

	hammingWindow(pitchLength,windowPitch);


	memset(&pitch,0,sizeof(pitch));
	memset(&noise, 0, sizeof(noise));
	memset(&vocoderInput, 0, sizeof(vocoderInput));
	memset(&synthezised, 0, sizeof(synthezised));

	int last = 1;
	int lastPulse = 1;

	int i,j,k,l,m;
	int test = length_data-0.025*Fs;
	int lastSpeech, nextSpeech, lastPitch, nextPitch;
	printf("%d, %d\n", length_data, speechLength);
	for (i = Fs*0.03; i<length_data-0.025*Fs; i= i+step) {

		lastSpeech = i+1-0.015*Fs;
		nextSpeech = i+0.015*Fs;
		lastPitch = i+1-0.025*Fs;
		nextPitch = i+0.025*Fs;
		printf("1\n");

		for (j = 0; j<nextSpeech-lastSpeech; j++) {
		    y_filt[j] = windowSpeech[j]*data[lastSpeech+j];
		}
		printf("2\n");
		for (k = 0; k<nextPitch-lastPitch; k++) {
		    y_pitch[k] = windowPitch[k]*data[lastPitch+k];
		}


		printf("3\n");
		autocorr(y_filt,speechLength,r_y);

		printf("4\n");
		LevinsonDurbin(r_y,A,P);

		findPitchAndVoice(y_pitch,pitchLength,pitchProperties,Fs);
		printf("5, voiced: %f\n", pitchProperties[1]);
		if (pitchProperties[1] >= alpha){
		    printf("5if\n");
		    last = i+0.01*Fs-pitchProperties[0]-1;
		    printf("5iflast\n");
		    printf("%d, %f\n", lastPulse, pitchProperties[0]);
		    for (m = lastPulse; m<i+0.01*Fs; m = m+pitchProperties[0]){

			pitch[m] = 1;
			//if (m > last){
			    lastPulse = m;
			//}
		    }
		    printf("5iffor\n");
		}else{
		    printf("5else\n");
		    lastPulse = i+0.01*Fs;

		    rand_gauss(randNoise,step);
		    
		    for (l = 0; l<step; l++) {
			noise[i-halfStep+l] = beta*randNoise[l];
		    }
		    
		}
		printf("6\n");
		for (j = 0; j<length_data; j++) {
		    vocoderInput[j] = pitch[j] + noise[j];
		    if(vocoderInput[j] >= 1){
			vocoderInput[j] = 1;
		    }
		}

		for (k = 0; k<0.02*Fs; k++) {
		    temp[k] = vocoderInput[i-halfStep+k];
		}

		filtrate(temp,B,1,A,P);
		printf("7\n");
		for (l = 0; l<step; l++) {
		    synthezised[i-halfStep+l] = temp[l];
		}

	}

	//Lowpassfiltrate synthezised

	//output = synthesized

	for (i = 0; i<N; i++) {
		data[i]= synthezised[i];
	}
}
