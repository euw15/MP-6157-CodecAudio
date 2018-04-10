#ifndef UTILFUNCS_H_
#define UTILFUNCS_H_

#include <stdint.h>

/*typedef struct CplxCoeff
{
	uint8_t real;
	uint8_t img;
} CplxCoeff;*/

char* ReadFileInBinaryMode(char* FileName, long* BufferSize);

void ExtractDescriptor(char* File);

int*** ExtractCoeffs(char* File);

#endif
