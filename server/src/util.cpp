#include "util.h"
#include <stdlib.h>
#include <stdio.h>

void errif(bool bl, const char* err){
    if(bl){
        perror(err);
        exit(EXIT_FAILURE);
    }
}