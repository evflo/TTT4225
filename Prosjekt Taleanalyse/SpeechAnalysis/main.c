//
//  main.c
//  SpeechAnalysis
//
//  Created by Even on 02/03/15.
//  Copyright (c) 2015 Even. All rights reserved.
//

#include <stdio.h>
#include "basicVocoder.h"
#include "OpenFile.h"

int main(int argc, const char * argv[]) {
    int P;
    float* x = readFile(argv[1]);
    const int N = sizeof(x)
    float y[N];
    basicVocoder(x,y,P);
    
    return 0;
}
