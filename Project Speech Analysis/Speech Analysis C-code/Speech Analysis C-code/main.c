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
#include "openFile.h"
#include "basicVocoder.h"
#include "RELP.h"
#include "SignalProcessing.h"
int main(int argc, const char * argv[]) {
	//short* soundData;
	//short* outputData;

	typedef unsigned char BYTE;
	typedef unsigned int DWORD;
	char* file = "/home/parallels/Desktop/anvsb1.wav";
	char* outFile = "home/parallels/Desktop/basicVocoder.wav";
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
	fclose(soundFile);
	//Reading the .wav file.
	

	int wav_length = dataSize/2;
	int step = 0.020*16000;
	float y[step],r[step],A[14];
	int i;
	float gain = 1.0/32760.0;
	for (i=1;i<step;i++){
	
		y[i] = (float) soundData[i]*gain;
	}
	autocorr(y,step,r);
	LevinsonDurbin(r,A,14);
	//basicVocoder(y,dataSize/2,14,argv);
	for (i = 0; i < 14; ++i)
	{
	/* code */

		printf("%.9g ",A[i]);
	}
	printf("\n\n");
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

	fwrite(soundData,sizeof(short),dataSize/2,soundFile);
	


	fclose(soundFile);
	//Writing the .wav file
	return 0;
}
