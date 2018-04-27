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
#include "GeneralFlags.h"

#if !C55X
typedef int coefType;
typedef uint8_t headerType;
#else
typedef long coefType;
typedef uint16_t headerType;
#endif

#if C55X
headerType** ExtractDescriptorC55(unsigned char* File, int* BlockCount);
#else
headerType** ExtractDescriptor(unsigned char* File, int* BlockCount);
#endif

unsigned char* ReadFileInBinaryMode(const char* FileName, long* BufferSize);
coefType*** ExtractCoeffs(unsigned char* File);
coefType*** RetrieveIFFTCoeffs(coefType*** coeffs);
#endif
