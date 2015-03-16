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
#include <string.h>
#include "basicVocoder.h"
#include "RELP.h"
#include "SignalProcessing.h"


int main(int argc, const char * argv[]) {
	//short* soundData;
	//short* outputData;

	typedef unsigned char BYTE;
	typedef unsigned int DWORD;
	char* file;
	char* outFile;
	if (argc == 1){
		printf("Mangler argument\n");
		return 0;
	}
	if (strcmp(argv[1], "sanntid") == 0){
		file = "/home/evenflo/Documents/TTT4240/Project Speech Analysis/anvsb1.wav";
		outFile = "/home/evenflo/Documents/TTT4240/Project Speech Analysis/basic.wav";
	}else if (strcmp(argv[1], "evenMac") == 0){
		file = "/Users/even_florenes/GitHub/TTT4240/Project Speech Analysis/anvsb1.wav";
		outFile = "/Users/even_florenes/GitHub/TTT4240/Project Speech Analysis/basic.wav";
	}else if (strcmp(argv[1], "evenUbuntu") == 0){
		file = "/home/parallels/Documents/TTT4240/Project Speech Analysis/anvsb1.wav";
		outFile = "/home/parallels/Documents/TTT4240/Project Speech Analysis/basic.wav";
	}else if (strcmp(argv[1], "sanntidBenjamin") == 0){
		file = "/home/benjamsf/Documents/TTT4240/Project Speech Analysis/anvsb1.wav";
		outFile = "/home/benjamsf/Documents/TTT4240/Project Speech Analysis/basic.wav";
	}else{
		printf("Feil argument\n");
		printf("Husk argument\n");
		return 0;
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
	
	int i;
	float gainDown = 1.0/32760.0;
	for (i=1;i<wav_length;i++){
	
		y[i] = (float) soundData[i]*gainDown;
		
	}
	printf("Input ints: %d, %d, %d\n", soundData[1000], soundData[5000], soundData[10000]);
	printf("convert to float: %g, %g, %g\n", y[1000], y[5000], y[10000]);

	
	float output[wav_length];
	basicVocoder(y,output,dataSize/2,14);
	//RELPcoder(y,output,dataSize/2,14,0);
	
	float gainUp = 32760.0;
	float tmp;
	for(i=0;i<wav_length;i++){
		tmp = output[i]*gainUp;
		soundData[i] = (short) tmp; //should be output instead of y
	}
	printf("Output after gain: %d, %d, %d\n", soundData[1000], soundData[5000], soundData[10000]);
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
