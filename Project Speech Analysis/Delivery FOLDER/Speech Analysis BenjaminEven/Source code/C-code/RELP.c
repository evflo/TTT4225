//
//  RELP.c
//  Speech Analysis C-code
//

#include "RELP.h"
//Including the SignalProcessing.h to use the function defined in SignalProcessing.c
#include "SignalProcessing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



void RELPcoder(float* data, float* output,int length_data,int P, int choice){

	//Define the parameter values to be used in the processing
    int filterOrden = 8;
    int Fs = 16000;
    int D = 4;
    int low_P = 4;
    int step = 0.02*Fs;
    int halfStep = step/2;
    int speechLength = 0.03*Fs;
    int start = 0.005*Fs;
    float B[1] = {1};

	//Allocating space for all the arrays that is needed in the computation
	//Using calloc sets the value of every array position to zero
    float* syntheticError = (float*) calloc(length_data,sizeof(float));
    float* syntheticSpeechUpsampled = (float*) calloc(length_data,sizeof(float));
    float* syntheticSpeechHF = (float*) calloc(length_data,sizeof(float));
    float* temp1 = (float*) calloc(length_data,sizeof(float));
    float* windowSpeech = (float*) calloc(speechLength,sizeof(float));
    float* dataDecimated = (float*) calloc(speechLength/D,sizeof(float));
    float* speechFilt = (float*) calloc(speechLength,sizeof(float));
    float* speechEst = (float*) calloc(speechLength,sizeof(float));
    float* speechError = (float*) calloc(speechLength,sizeof(float));
    float* speechErrorFilt = (float*) calloc(speechLength,sizeof(float));
    float* ryLF = (float*) calloc(speechLength,sizeof(float));
    float* ryHF = (float*) calloc(speechLength,sizeof(float));
    float* ry = (float*) calloc(speechLength,sizeof(float));
    float* A = (float*) calloc(P+1,sizeof(float));
    float* A_low = (float*) calloc(low_P+1,sizeof(float));
    float* HFexcitation = (float*) calloc(speechLength,sizeof(float));
    float* HFexcitationFilt = (float*) calloc(speechLength,sizeof(float));
    float* HFexcitationLP = (float*) calloc(speechLength,sizeof(float));
    float* LFexcitation = (float*) calloc(speechLength,sizeof(float));
    float* syntheticFullbandResidual = (float*) calloc(speechLength,sizeof(float));
    float* speechUpsampling = (float*) calloc(speechLength,sizeof(float));
    float* speechAlgorithm = (float*) calloc(speechLength,sizeof(float));
	
    //Defining the filter coefficients for both the lowpass- and highpassfilter
	//The coefficients has been computed in MATLAB
    float coeffLow[9] = {0, -0.0277, 0, 0.274,0.4974, 0.274, 0, -0.0227, 0};
    float coeffHigh[9] = {0, -0.0161, -0.086, -0.1948, 0.7501, -0.1948, -0.0860, -0.0161, 0};
	
	//Defining variables that will be changed in the processing
    float gainLF, gainHF, gain;
    int i,j,filtering;
    float tmp;
	
	//Making the Hamming window using the function in SignalProcessing.h
    hammingWindow(windowSpeech,speechLength);

    for (i=0.03*Fs; i<length_data-0.025*Fs; i+= step) {
        int lastSpeech = i+1-0.015*Fs;
        int nextSpeech = i+0.015*Fs;
       
		// Select a frame from the signal using Hamming window 
        for (j = 0; j<nextSpeech-lastSpeech; j++) {
            tmp = windowSpeech[j]*data[lastSpeech+j];
            speechFilt[j]= tmp;
        }
        
        // Finding filter coefficients using LPC
        autocorr(speechFilt, speechLength,ry);
        LevinsonDurbin(ry, A, P);
        
        filtering = filtrate(speechFilt,speechLength ,B, 1, A, P, speechEst);
        if(filtering == -1){
            printf("Filtrate function failed: Size of A smaller than 1\n");
            return;
        }
		// Find prediction error
        for (j=0; j< speechLength; j++) {
            speechError[j] = speechFilt[j]-speechEst[j];
        }
        for(j= 0; j< step;j++){
            syntheticError[i+j-halfStep] = speechError[j+start];
        }

        // Filtrate speechError
        firFilter(coeffLow, filterOrden,speechError,speechErrorFilt,speechLength);
		
        // Decimate speechError
        decimate(speechErrorFilt, dataDecimated, speechLength, D); 
		
        // Upsample speechError
        upsample(dataDecimated, speechError, speechLength, D);
       
		// Low-pass filter the upsampled signal
        firFilter(coeffLow,filterOrden,speechError,LFexcitation,speechLength);
        
        for (j = 0; j < speechLength; j++){
            HFexcitation[j] = LFexcitation[j];
            if (HFexcitation[j] < 0){
                HFexcitation[j] = 0;
            }
        }
        autocorr(HFexcitation,speechLength,ryHF);
        autocorr(LFexcitation,speechLength,ryLF);
		
		// Filter the high frequency components using filter coefficients from a low order LPC
        LevinsonDurbin(ry,A_low,low_P);
        filtering = filtrate(HFexcitation,speechLength,B,1,A_low,low_P,HFexcitationLP);
        if(filtering == -1){
            printf("Filtrate function failed: Size of A smaller than 1\n");
            return;
        }
		
		// High-pass filter the HF signal 
        firFilter(coeffHigh,filterOrden,HFexcitationLP,HFexcitationFilt,speechLength);
		
		// Adjust gain before adding the LF and HF components
        gainHF = 0;
        gainLF = 0;
        for (j = 0; j < speechLength; j++){
            if(ryHF[j]> gainHF){
                gainHF = ryHF[j];
            }
            if (ryLF[j] > gainLF){
                gainLF = ryLF[j];
            }
        }
        gain = gainLF/gainHF;

		// Add LF and HF components
        for (j = 0; j < speechLength; j++){
            HFexcitationFilt[j] = gain*HFexcitationFilt[j];
            syntheticFullbandResidual[j] = LFexcitation[j] + HFexcitationFilt[j];
        }
		
		// Filter the upsampled signal using the LPC coefficients to find RELP with upsampling
        filtering = filtrate(speechError,speechLength,B,1,A,P,speechUpsampling);
        if(filtering == -1){
            printf("Filtrate function failed: Size of A smaller than 1\n");
            return;
        }
		
		// Filter the signal obtained through HF regeneration using the LPC coefficients from the first input
        filtering = filtrate(syntheticFullbandResidual,speechLength,B,1,A,P,speechAlgorithm);
        if(filtering == -1){
            printf("Filtrate function failed: Size of A smaller than 1\n");
            return;
        }
		
        for (j = 0; j< speechLength; j++){
            syntheticSpeechUpsampled[i+j-halfStep] = speechUpsampling[j+start];
            syntheticSpeechHF[i+j-halfStep] = speechAlgorithm[j+start];
        }
    }
	
	// choice == 0: Use RELP with upsampling
	// choice == 1: Use RELP with HF regeneration
    switch (choice){
        case 0:
            for (i = 0; i < length_data; i++){ output[i] = syntheticSpeechUpsampled[i];}
            break;
        case 1:
            for (i= 0; i<length_data; i++){ output[i] = syntheticSpeechHF[i];}
            break;
    }
	
	// Attempt at making the speech intelligible, amplify everything so average amplitude becomes max amplitude
	// Clip everything else.
    float maxVal = 0;
    float sum = 0;
    for (i = 0; i < length_data; i++){
        sum += fabsf(output[i])/(float)length_data;
        if (fabsf(output[i]) > maxVal){
            maxVal = fabsf(output[i]);

        }
    }
    for (i = 0; i < length_data; i++){
        if(fabsf(output[i]) >= sum){
            output[i] = output[i] / fabsf(output[i]);// maxVal;
        }else{
            output[i] = output[i]/sum;
        }
    }
<<<<<<< Updated upstream
   
<<<<<<< Updated upstream
	// Free dynamically allocated arrays
=======
=======
    //SNR
    /*
    firFilter(coeffLow,filterOrden,output, temp1, length_data);

    for (i = 0; i < length_data; i++){
        output[i] = temp1[i];
    }
    */
>>>>>>> Stashed changes
    printf("Max value and average: %g %g\n", maxVal,sum);
    printf("Error values: %g %g %g\n",syntheticError[1000],syntheticError[5000],syntheticError[10000]);
    printf("Output(HFregeneration) before gain: %g %g %g\n",syntheticSpeechHF[1000],syntheticSpeechHF[5000],syntheticSpeechHF[10000]);
    printf("Output(upsample) before gain: %g %g %g\n",output[1000],output[5000],output[10000]);
>>>>>>> Stashed changes
    free(syntheticError),free(syntheticSpeechUpsampled),free(syntheticSpeechHF),free(windowSpeech);
    free(dataDecimated), free(speechFilt), free(speechEst),free(speechError),free(speechErrorFilt);
    free(ryLF),free(ryHF),free(A),free(A_low),free(HFexcitation),free(HFexcitationFilt);
    free(HFexcitationLP),free(LFexcitation),free(syntheticFullbandResidual),free(speechUpsampling);
    free(speechAlgorithm);
}