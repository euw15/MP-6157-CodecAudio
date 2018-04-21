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

#define ANY
//#define C55

#ifdef ANY
#define WRD_SIZE 8
typedef int coefType;
typedef uint8_t headerType;
#endif

#ifdef C55
#define WRD_SIZE 16
typedef long coefType;
typedef uint16_t headerType;
#endif

unsigned char* ReadFileInBinaryMode(const char* FileName, long* BufferSize);

uint8_t** ExtractDescriptor(unsigned char* File);

int*** ExtractCoeffs(unsigned char* File);

int*** RetrieveIFFTCoeffs(coefType*** coeffs);

#endif
