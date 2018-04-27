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

const char* c_sFileName = "C:\\Output_i55.bin";

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

    FileBuffer = ReadFileInBinaryMode(c_sFileName, &FileSize);
    if (FileBuffer == NULL || FileSize <= 0)
    {
        return FAILURE;
    }

#if !C55X
    Header = ExtractDescriptor(FileBuffer, &BlockCount);
#else
    Header = ExtractDescriptorC55(FileBuffer, &BlockCount);
#endif

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

    for(BlockIdx = 0; BlockIdx < BlockCount; BlockIdx++, SampleCounterAddress += SAMPLES_PER_BLOCK)
    {
        //ifft16(IFFTCoeffs[BlockIdx][0], IFFTCoeffs[BlockIdx][1], SampleBuffer, SAMPLES_PER_BLOCK);
        memcpy(SampleCounterAddress, SampleBuffer, SAMPLES_PER_BLOCK * sizeof(int16_t));
    }

    //free mem
    free(SampleBuffer);
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
