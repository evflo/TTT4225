//
//  openFile.c
//  Speech Analysis C-code
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include "openFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
//#include <libsndfile.h>

#define BUFSIZE 1024
int data[BUFSIZE];



/*
void readFile(float* count, const char* file){
    
    SNDFILE *infile;
    
    SFINFO sfinfo;
    
    memset(&sfinfo,0,sizeof(sfinfo));
    
    if(!(infile = sf_open(file,SFM_READ,&sfinfo))){
        
    }
    
    calloc(sfinfo.frames*sfinfo.channels,sizeof(double));
    count = sf_read_double(infile, data,sfinfo.frames*sfinfo.channels);
    
}
*/

void readFile1(short* soundData, const char* file){
    typedef unsigned char BYTE;
    typedef unsigned int DWORD;
    
    FILE *soundFile;
    BYTE id[4],id2[4],id3[4],data[4];
    DWORD size,formatSize,sampleRate,bytesPerSec,dataSize;
    short format_tag,channels,block_align,bitsPerSample;
    
    //Reading the .wav file.
    soundFile = fopen(file,"rb");
    
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
    
    soundData = (short*) calloc (dataSize/2,sizeof(short));
    
    fread(soundData,sizeof(short),dataSize/2,soundFile);
    fclose(soundFile);
    //Reading the .wav file.
    
    
    //Writing the .wav file
    soundFile = fopen("file","wb");
    
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
}
