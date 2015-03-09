//
//  main.c
//  Speech Analysis C-code
//
//  Created by Even on 05/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include <stdio.h>
#include "openFile.h"
#include "basicVocoder.h"
#include "RELP.h"
int main(int argc, const char * argv[]) {
    short* soundData;
    float* outputData;
    typedef unsigned char BYTE;
    typedef unsigned int DWORD;
    char* file = "/users/even_florenes/desktop/anvsb1.wav";
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
    outputData = (float*) calloc(dataSize/2, sizeof(float));
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
    
    basicVocoder((float*)soundData, outputData, 14);
    
    
    fclose(soundFile);
    //Writing the .wav file
    
}
