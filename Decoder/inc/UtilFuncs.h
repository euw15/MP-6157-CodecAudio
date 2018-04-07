#ifndef UTILFUNCS_H_
#define UTILFUNCS_H_

typedef struct CplxCoeff
{
	uint8_t real;
	uint8_t img;
} CplxCoeff;

char* ReadFileInBinaryMode(char* FileName, long* BufferSize);

CplxCoeff* ExtractDescriptor(char* File);

#endif
