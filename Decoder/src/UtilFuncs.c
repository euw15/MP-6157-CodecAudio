#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "UtilFuncs.h"

// Defines
//32 real + 32 imaginary coeffs
#define COEFF_COUNT 64
#define ACTUAL_COEFFS 32
#define BYTES_PER_HEADER 16

// Global variables
uint16_t N = 0;
unsigned int DataSize = 0;
uint8_t** Header; //of size N, populated in ExtractDescriptor. It holds the header with amount of bits used to store each coefficient, for each audio block
short bitmask[4] = {0x1, 0x3, 0xF, 0xFF}; //mask for requested amount of bits (1, 2, 4, 8)


//***************************************************************************
//* Function Name:	ReadFileInBinaryMode
//*
//* Purpose:		Reads a file and stores its data in a bytes buffer. If everything
//*					is ok, the method returns a pointer to the buffer and the BufferSize
//*					out parameter is populated.
//*
//* Parameters:		FileName	- IN - A pointer to the name of the file to be read.
//*					BufferSize	- IN - A pointer to a variable to store the buffer size in bytes.
//*
//* Returns:		A pointer to the bytes buffer which contains the file's data.
//*					In case of error a NULL pointer is returned. Those cases are:
//*						- FileName pointer is null.
//*						- BufferSize pointer is null.
//*						- If it's not possible to open the file.
//*						- If it's not possible to allocate memory for the buffer to store
//*						  the file's data.
//*
//***************************************************************************
char* ReadFileInBinaryMode(char* FileName, long* BufferSize)
{
	FILE* FilePtr = NULL;
	char* BufferPtr = NULL;
	if (NULL == FileName || BufferSize == NULL)
	{
		return NULL;
	}
	FilePtr = fopen(FileName, "rb");
	if (NULL == FilePtr)
	{
		*BufferSize = 0;
		return NULL;
	}
	fseek(FilePtr, 0, SEEK_END);
	*BufferSize = ftell(FilePtr);
	fseek(FilePtr, 0, SEEK_SET);
	BufferPtr = (char*)malloc(*BufferSize);
	if (NULL == BufferPtr)
	{
		fclose(FilePtr);
		*BufferSize = 0;
		return NULL;
	}
	fread(BufferPtr, *BufferSize, 1, FilePtr);
	fclose(FilePtr);
	return BufferPtr;
}

//***************************************************************************
//* Function Name:	ExtractDescriptor
//*
//* Purpose:		Reads the byte data buffer and extracts the header descriptor
//*
//* Parameters:		File - IN - A data file pointer extracted with ReadFileInBinaryMode.
//*
//* Returns:		A void of everlasting darkness my old friend
//*
//***************************************************************************
void ExtractDescriptor(char* File)
{
	int i = 0, j = 0;
	short limit = 0;
	uint16_t headerIdx = 0;
	uint8_t byte;
	N = 0;
	N = File[0];
	N <<= 8;
	N |= File[1];
	limit = N + 2;
	
	Header = (uint8_t**) malloc(N * sizeof(uint8_t*));
	
	for(i = 2, j = 0, DataSize = 0; i < limit; i++, j++)
	{
		Header[j] = (uint8_t*) malloc(COEFF_COUNT * sizeof(uint8_t));
		byte = File[i];
		for(headerIdx = 0; headerIdx < BYTES_PER_HEADER; headerIdx+=4)
		{
			DataSize += (Header[j][headerIdx]   = ((byte & 0xC0) >> 6) );
			DataSize += (Header[j][headerIdx+1] = ((byte & 0x30) >> 4) );
			DataSize += (Header[j][headerIdx+2] = ((byte & 0xC) >> 2) );
			DataSize += (Header[j][headerIdx+3] = (byte & 0x3) );
		}
	}
}

//***************************************************************************
//* Function Name:	ExtractCoeffs
//*
//* Purpose:		Reads the byte data buffer and extracts the compressed data,
//*						re-applies the original 128 offset, and scales it back
//*						by multiplying each value by 2^13
//*					Remember that Header[][] contains the amount of bits to read per coeff
//*
//* Parameters:		File - IN - A data file pointer extracted with ReadFileInBinaryMode.
//*
//* Returns:		FFT Coefficients read into a 3 dimensional array of the form
//*					
//*
//***************************************************************************
int*** ExtractCoeffs(char* File)
{   //header, block idx, real coeff idx, imaginary coeff idx, coeff number, data idx
	int h = 0, bi = 0, ci = 0, cii = 0, coeff = 0, di = 0;
	short trailing = 0, bitIdx = 0, bitSwch = 0, reqBits = 0;//vars for word processing
	int limit = COEFF_COUNT * N;
	char* Data = File + (N*BYTES_PER_HEADER + 2);//move pointer to start of compressed data, 2 bytes for N, and N * 16 to skip over headers

	//Data section size in bits was extracted on the header parser, divide by 8 to get the index count for Data
	const unsigned int dataIdx = DataSize >> 3;
	
	// 2 dimensions array for storing complex coeffs, succesive elements starting with idx 0 are paired (real numbers and img numbers)
	int*** Coeffs = (int***) malloc(N * sizeof(int**));
	
	for(di = dataIdx, bi = 0; bi < N; bi++)//loop through each audio block (8ms)
	{
		Coeffs[bi] = (int**) malloc(2 * sizeof(int*)); //2: real and img
		Coeffs[bi][0] = (int*) malloc(COEFF_COUNT * sizeof(int));

		bitIdx = 8;
		//Header[N][64]
		for(h = 0; h < COEFF_COUNT; h++)//loop through this block's header
		{
			reqBits = Header[bi][h];
			bitSwch = bitIdx - reqBits;

			if(bitSwch == 0)//last fetch from current Data byte
			{
				//pull entire byte, requested bits are obtained with the mask applied after these else-ifs
				coeff = Data[di];
				++di;
				bitIdx = 8;
			}
			else if(bitSwch < 0)//trailing bits, supports any anount of trailing between 1-8
			{
				//pull remaining bits, leave space for remaining bits stored in next Data byte
				coeff = (Data[di] << (reqBits - bitIdx));
				bitIdx = 8 - (reqBits - bitIdx);
				++di;
				coeff |= Data[di] >> bitIdx;
			}
			else//fetch normally
			{
				coeff = Data[di] >> bitSwch;
				bitIdx -= reqBits;
			}

			coeff &= bitmask[reqBits];

			if(h < ACTUAL_COEFFS)//real
			{
				Coeffs[bi][0][h] = coeff;
			}
			else//img
			{
				Coeffs[bi][1][h] = coeff;
			}
		}
	}
}
