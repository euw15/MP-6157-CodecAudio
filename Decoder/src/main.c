//***************************************************************************
//* FileName:       main.c
//*
//* Description:    Decoders main file which exercises all the code.
//*
//***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GeneralFlags.h"
#include "UtilFuncs.h"
#include "fft.h"
#include "WavDecoder.h"

const char* c_sFileName = "C:\\Output_i.bin";

int main()
{
    unsigned char* FileBuffer = NULL;
    headerType** Header = NULL;
    coefType*** Coeffs = NULL;
    coefType*** IFFTCoeffs = NULL;
    int16_t* Samples = NULL;
    int16_t* SampleBuffer = NULL;
    int16_t* SampleCounterAddress = NULL;
    long FileSize = 0;
    int BlockIdx = 0;
    int BlockCount = 0;
#if FLOATING_POINT
    int FpConvIdx = 0;
    double* SampleFpBuffer = NULL;
#endif

    FileBuffer = ReadFileInBinaryMode(c_sFileName, &FileSize);
    if (FileBuffer == NULL || FileSize <= 0)
    {
        return FAILURE;
    }

    Header = ExtractDescriptor(FileBuffer, &BlockCount);
    Coeffs = ExtractCoeffs(FileBuffer);

    //	Free memory
    free(FileBuffer);

    IFFTCoeffs = RetrieveIFFTCoeffs(Coeffs);

    //	Free memory
	for (BlockIdx = 0; BlockIdx < BlockCount; BlockIdx++)
	{
		free(Header[BlockIdx]);
		free(Coeffs[BlockIdx][0]);
		free(Coeffs[BlockIdx][1]);
		free(Coeffs[BlockIdx]);
	}
	free(Coeffs);
    free(Header);

    //SAMPLES AS INT16_T
    Samples = (int16_t*)malloc(BlockCount * SAMPLES_PER_BLOCK * sizeof(int16_t));
    SampleBuffer = (int16_t*)malloc(SAMPLES_PER_BLOCK * sizeof(int16_t));
    SampleCounterAddress = Samples;

#if FLOATING_POINT
    SampleFpBuffer = (double*)malloc(SAMPLES_PER_BLOCK * sizeof(double));
#endif

    for(BlockIdx = 0; BlockIdx < BlockCount; BlockIdx++, SampleCounterAddress += SAMPLES_PER_BLOCK)
    {
#if FLOATING_POINT
        ifft(IFFTCoeffs[BlockIdx][0], IFFTCoeffs[BlockIdx][1], SampleFpBuffer, SAMPLES_PER_BLOCK);
        for (FpConvIdx = 0; FpConvIdx < SAMPLES_PER_BLOCK; FpConvIdx++) 
        {
            SampleBuffer[FpConvIdx] = (int16_t)SampleFpBuffer[FpConvIdx];
        }
#else
        ifft_fixedpoint(IFFTCoeffs[BlockIdx][0], IFFTCoeffs[BlockIdx][1], SampleBuffer, SAMPLES_PER_BLOCK);
#endif
        memcpy(SampleCounterAddress, SampleBuffer, SAMPLES_PER_BLOCK * sizeof(int16_t));
    }

    //free mem
    free(SampleBuffer);

#if FLOATING_POINT
    free(SampleFpBuffer);
#endif

    for (BlockIdx = 0; BlockIdx < BlockCount; BlockIdx++)
    {
        free(IFFTCoeffs[BlockIdx][0]);
        free(IFFTCoeffs[BlockIdx][1]);
        free(IFFTCoeffs[BlockIdx]);
    }
    free(IFFTCoeffs);

    WavWriter("Ak7.wav", Samples, BlockCount * SAMPLES_PER_BLOCK);

    //free mem
    free(Samples);

    return SUCCESS;
}
