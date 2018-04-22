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
#define WORDS_PER_HEADER 16
typedef int coefType;
typedef uint8_t headerType;
#endif

#ifdef C55
#define WRD_SIZE 16
#define WORDS_PER_HEADER 8
typedef long coefType;
typedef uint16_t headerType;
#endif

unsigned char* ReadFileInBinaryMode(const char* FileName, long* BufferSize);

#ifdef C55
headerType** ExtractDescriptorC55(unsigned char* File);
#else
headerType** ExtractDescriptor(unsigned char* File);
#endif

coefType*** ExtractCoeffs(unsigned char* File);

coefType*** RetrieveIFFTCoeffs(coefType*** coeffs);

#endif
