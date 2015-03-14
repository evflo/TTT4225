//
//  main.c
//  Speech Analysis C-code
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basicVocoder.h"
#include "RELP.h"
#include "SignalProcessing.h"


int main(int argc, const char * argv[]) {
	//short* soundData;
	//short* outputData;

	typedef unsigned char BYTE;
	typedef unsigned int DWORD;
	if (argv[1] == "sanntid"){
		char* file = "/home/evenflo/Documents/TTT4240/Project Speech Analysis/anvsb1.wav";
		char* outFile = "/home/evenflo/Documents/TTT4240/Project Speech Analysis/basic.wav";
	}else if (argv[1] == "even"){
		char* file = "mac-plass";
		char* outfile = "mac-plass ut";
	}else{
		printf("Husk argument.\n");
		return;
	}
	FILE *soundFile;
	BYTE id[4],id2[4],id3[4],data[4];
	DWORD size,formatSize,sampleRate,bytesPerSec,dataSize;
	short format_tag,channels,block_align,bitsPerSample;

	//Reading the .wav file.
	if((soundFile = fopen(file,"rb"))== NULL){
		perror("Failed to open file for reading");
	}
	fread(&id,sizeof(BYTE),4,soundFile);
	fread(&size,sizeof(DWORD),1,soundFile);
	fread(&id2,sizeof(BYTE),4,soundFile);
	fread(&id3,sizeof(BYTE),4,soundFile);
	fread(&formatSize,sizeof(DWORD),1,soundFile);
	fread(&format_tag,sizeof(short),1,soundFile);
	fread(&channels,sizeof(short),1,soundFile);
	fread(&sampleRate,sizeof(DWORD),1,soundFile);
	fread(&bytesPerSec,sizeof(DWORD),1,soundFile);
	fread(&block_align,sizeof(short),1,soundFile);
	fread(&bitsPerSample,sizeof(short),1,soundFile);
	fread(&data,sizeof(BYTE),4,soundFile);
	fread(&dataSize,sizeof(DWORD),1,soundFile);

	short* soundData =  calloc (dataSize/2,sizeof(short));
	//short* outputData =  calloc(dataSize/2, sizeof(short));
	fread(soundData,sizeof(short),dataSize/2,soundFile);
	if (fclose(soundFile) != 0){
		perror("Failed to close file\n");
	}
	//Reading the .wav file.
	
	int wav_length = dataSize/2;
	float y[wav_length];
	
	//int step = 0.020*16000;
	//float y[step],r[step],A[14];

	
	//int step = 0.020*16000;
	//float y[step],r[step],A[14];
	int i;
	float gain = 1.0/32760.0;
	for (i=1;i<wav_length;i++){
	
		y[i] = (float) soundData[i]*gain;
	}
	/*autocorr(y,step,r);
	LevinsonDurbin(r,A,14);
	for (i = 0; i < 14; ++i)
	{
	

		printf("%.9g ",A[i]);
	}
	printf("\n\n");
	*/
	float output[wav_length];
	basicVocoder(y,output,dataSize/2,14);
	/*rand_gauss(y,wav_length);
	basicVocoder(y,output,dataSize/2,14);
	hammingWindow(y,wav_length);
	int i;
	for (i = 0; i < wav_length; i++){
		printf("%f\n",y[i]);
	}*/

	//Writing the .wav file
	soundFile = fopen(outFile,"wb");
	if (soundFile == NULL){
		perror("Failed to open file to be written to");
	}
	fwrite(&id,sizeof(BYTE),4,soundFile);
	fwrite(&size,sizeof(DWORD),1,soundFile);
	fwrite(&id2,sizeof(BYTE),4,soundFile);
	fwrite(&id3,sizeof(BYTE),4,soundFile);
	fwrite(&formatSize,sizeof(DWORD),1,soundFile);
	fwrite(&format_tag,sizeof(short),1,soundFile);
	fwrite(&channels,sizeof(short),1,soundFile);
	fwrite(&sampleRate,sizeof(DWORD),1,soundFile);
	fwrite(&bytesPerSec,sizeof(DWORD),1,soundFile);
	fwrite(&block_align,sizeof(short),1,soundFile);
	fwrite(&bitsPerSample,sizeof(short),1,soundFile);
	fwrite(&data,sizeof(BYTE),4,soundFile);
	fwrite(&dataSize,sizeof(DWORD),1,soundFile);

	fwrite(output,sizeof(short),dataSize/2,soundFile);
	


	fclose(soundFile);
	//Writing the .wav file
	return 0;
}
