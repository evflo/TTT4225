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
#include <libsndfile.h>

#define BUFSIZE 1024
int data[BUFSIZE];




void readFile(float* count, const char* file){
    
    SNDFILE *infile;
    
    SFINFO sfinfo;
    
    memset(&sfinfo,0,sizeof(sfinfo));
    
    if(!(infile = sf_open(file,SFM_READ,&sfinfo))){
        
    }
    
    calloc(sfinfo.frames*sfinfo.channels,sizeof(double));
    count = sf_read_double(infile, data,sfinfo.frames*sfinfo.channels);
    
}
