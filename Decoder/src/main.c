//***************************************************************************
//* FileName:       main.c
//*
//* Description:    Decoders main file which exercises all the code.
//*
//***************************************************************************

#include <stdio.h>
#include "UtilFuncs.h"

#define SUCCESS 0
#define FAILURE -1
#define TRACING 1

const char* c_sFileName = "C:\\TestFile1.bin";

int main() 
{
    unsigned char* FileBuffer = NULL;
    long FileSize = 0;

#ifdef TRACING
    printf("Starting reading file...\n");
#endif
    FileBuffer = ReadFileInBinaryMode(c_sFileName, &FileSize);
#ifdef TRACING
    printf("File reading completed...\n");
#endif

    if (FileBuffer == NULL || FileSize <= 0) 
    {
#ifdef TRACING
        printf("Error found during reading file...\n");
#endif
        return FAILURE;
    }

    return SUCCESS;
}