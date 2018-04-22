//***************************************************************************
//* FileName:       main.c
//*
//* Description:    Decoders main file which exercises all the code.
//*
//***************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "UtilFuncs.h"
#include "WavDecoder.h"
#include "fft.h"

#define SUCCESS 0
#define FAILURE -1
#define TRACING 1
#define SAMPLES_PER_BLOCK   64
#define NUM_OF_BLOCKS       1000

const char* c_sFileName = "C:\\Luigi.bin";

int main() 
{
    unsigned char* FileBuffer = NULL;
    int16_t* Int16SampleBuffer = NULL;
    uint8_t** Header = NULL;
    int*** Coeffs = NULL;
    int*** IFFTCoeffs = NULL;
    double* Samples = NULL;
    double* SampleBuffer = NULL;
    double* SampleCounterAddress = NULL;
    uint32_t NumberOfSamples = 0;
    uint32_t SampleIdx = 0;
    long FileSize = 0;
    int BlockIdx = 0;

#ifdef TRACING
    printf("Starting reading file...\n");
#endif
    FileBuffer = ReadFileInBinaryMode(c_sFileName, &FileSize);
    if (FileBuffer == NULL || FileSize <= 0) 
    {
#ifdef TRACING
        printf("Error found during reading file...\n");
#endif
        return FAILURE;
    }

    Header = ExtractDescriptor(FileBuffer);
    Coeffs = ExtractCoeffs(FileBuffer);
    IFFTCoeffs = RetrieveIFFTCoeffs(Coeffs);

    Samples = (double*)malloc(NUM_OF_BLOCKS * SAMPLES_PER_BLOCK * sizeof(double));
    SampleBuffer = (double*)malloc(SAMPLES_PER_BLOCK * sizeof(double));
    SampleCounterAddress = Samples;
    
    for (BlockIdx = 0; BlockIdx < NUM_OF_BLOCKS; BlockIdx++, SampleCounterAddress += SAMPLES_PER_BLOCK)
    {
        ifft(IFFTCoeffs[BlockIdx][0], IFFTCoeffs[BlockIdx][1], SampleBuffer, SAMPLES_PER_BLOCK);
        memcpy(SampleCounterAddress, SampleBuffer, SAMPLES_PER_BLOCK * sizeof(double));
    }
    
    NumberOfSamples = SAMPLES_PER_BLOCK * NUM_OF_BLOCKS;
    Int16SampleBuffer = (int16_t*)malloc(NUM_OF_BLOCKS * SAMPLES_PER_BLOCK * sizeof(int16_t));
    for (SampleIdx = 0; SampleIdx < NumberOfSamples; SampleIdx++)
    {
        Int16SampleBuffer[SampleIdx] = (int16_t)Samples[SampleIdx];
    }

    WavWriter("Ak7.wav", Int16SampleBuffer, NumberOfSamples);

    return SUCCESS;
}