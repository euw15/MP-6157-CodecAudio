#ifndef UTILFUNCS_H_
#define UTILFUNCS_H_

#include <stdint.h>

/*typedef struct CplxCoeff
{
    uint8_t real;
    uint8_t img;
} CplxCoeff;*/

char* ReadFileInBinaryMode(char* FileName, long* BufferSize);

void ExtractDescriptor(unsigned char* File);

int*** ExtractCoeffs(unsigned char* File);

#endif
