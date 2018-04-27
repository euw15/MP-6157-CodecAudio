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

typedef int32_t coefType;

#if C55X
typedef uint16_t headerType;
#else
typedef uint8_t headerType;
#endif

headerType** ExtractDescriptor(unsigned char* File, int* BlockCount);

unsigned char* ReadFileInBinaryMode(const char* FileName, long* BufferSize);
coefType*** ExtractCoeffs(unsigned char* File);
coefType*** RetrieveIFFTCoeffs(coefType*** coeffs);
#endif
