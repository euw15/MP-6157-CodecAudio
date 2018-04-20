//***************************************************************************
//* FileName:       UtilFuncs.h
//*
//* Description:    Header file which contains utility functions to read
//*                 codified audio files to obtain its coefficients.
//*
//***************************************************************************

#ifndef UTILFUNCS_H_
#define UTILFUNCS_H_

#include <stdint.h>

unsigned char* ReadFileInBinaryMode(const char* FileName, long* BufferSize);

uint8_t** ExtractDescriptor(unsigned char* File);

int*** ExtractCoeffs(unsigned char* File);

int*** RetrieveIFFTCoeffs(int*** coeffs);

#endif
