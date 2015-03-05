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
    float* sound;
    
    readFile(sound, argv[1]);
    
    
}
