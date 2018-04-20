//***************************************************************************
//* FileName:       UtilFuncs.c
//*
//* Description:    Implementation of the utility functions that read
//*                 codified audio files to obtain its coefficients.
//*
//***************************************************************************

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
uint8_t** Header; //of size N, populated in ExtractDescriptor. It holds the header with amount of bits used to store each coefficient, for each audio block [N, COEFF_COUNT]
short bitmask[4] = {0x01, 0x03, 0x0F, 0xFF}; //mask for requested amount of bits (1, 2, 4, 8)

//***************************************************************************
//* Function Name:  ReadFileInBinaryMode
//*
//* Purpose:        Reads a file and stores its data in a bytes buffer. If everything
//*                 is ok, the method returns a pointer to the buffer and the BufferSize
//*                 out parameter is populated.
//*
//* Parameters:     FileName    - IN - A pointer to the name of the file to be read.
//*                 BufferSize	- OUT - A pointer to a variable to store the buffer size in bytes.
//*
//* Returns:        A pointer to the bytes buffer which contains the file's data.
//*                 In case of error a NULL pointer is returned. Those cases are:
//*                     - FileName pointer is null.
//*                     - BufferSize pointer is null.
//*                     - If it's not possible to open the file.
//*                     - If it's not possible to allocate memory for the buffer to store
//*						  the file's data.
//*
//***************************************************************************
unsigned char* ReadFileInBinaryMode(const char* FileName, long* BufferSize)
{
    FILE* FilePtr = NULL;
    unsigned char* BufferPtr = NULL;
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
    BufferPtr = (unsigned char*)malloc(*BufferSize);
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
//* Purpose:		Reads the byte data buffer and extracts the header descriptor,
//*                     which is stored in global variable 
//*                     Header[N : Audio block count][COEFF_COUNT], describing the amount
//*                     of bits used to store each coefficient
//*
//* Parameters:		File - IN - A data file pointer extracted with ReadFileInBinaryMode.
//*
//* Returns:		A void of everlasting darkness my old friend
//*                 -.- a pointer to Header[][]
//*
//***************************************************************************
uint8_t** ExtractDescriptor(unsigned char* File)
{
	int i = 0, j = 0;
	short limit = 0;
	uint16_t headerIdx = 0;
	uint8_t byte;
	N = 0;
	N = File[1];
	N <<= 8;
	N |= File[0];
	limit = (N * BYTES_PER_HEADER) + 2;
	
	Header = (uint8_t**) malloc(N * sizeof(uint8_t*));
	
	for(i = 2, j = 0, DataSize = 0; i < limit && j < N; j++)
	{
		Header[j] = (uint8_t*) malloc(COEFF_COUNT * sizeof(uint8_t));
		for(headerIdx = 0; headerIdx < COEFF_COUNT; headerIdx+=4, i++)
		{
		    byte = File[i];
			DataSize += bitmask[(Header[j][headerIdx]   = ((byte & 0xC0) >> 6) )];
			DataSize += bitmask[(Header[j][headerIdx+1] = ((byte & 0x30) >> 4) )];
			DataSize += bitmask[(Header[j][headerIdx+2] = ((byte & 0xC) >> 2) )];
			DataSize += bitmask[(Header[j][headerIdx+3] = (byte & 0x3) )];
		}
	}
    return Header;
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
//*					Coeff[N : (Audio block count)] [2: (real, img)] [COEFF_COUNT]
//*
//***************************************************************************
int*** ExtractCoeffs(unsigned char* File)
{   //header, block idx, real coeff idx, imaginary coeff idx, coeff number, data idx
	int cftIdx = 0, blockIdx = 0, ci = 0, cii = 0, coeff = 0, dataIdx = 0;
    
    //idx for scanning each byte, bitwise shift amount, requested amount of bits
	short bitIdx = 0, bitShft = 0, reqBits = 0;//vars for byte processing
    
    //move pointer to start of compressed data, 2 bytes for N, and N * 16 to skip over headers
	unsigned char* Data = File + ((N*BYTES_PER_HEADER) + 2);

	//Data section size in bits was extracted on the header parser, divide by 8 to get the index count for Data
	const unsigned int dataLastIdx = DataSize >> 3;
	
	// 3 dimensional array for storing complex coeffs
	int*** Coeffs = (int***) malloc(N * sizeof(int**));

	//loop through each audio block (8ms)
	for(dataIdx = 0, blockIdx = 0; blockIdx < N; blockIdx++, dataIdx++)
	{
		Coeffs[blockIdx] = (int**) malloc(2 * sizeof(int*));
		Coeffs[blockIdx][0] = (int*) malloc(ACTUAL_COEFFS * sizeof(int));
		Coeffs[blockIdx][1] = (int*) malloc(ACTUAL_COEFFS * sizeof(int));

        //8 is MSB
		bitIdx = 8;

        //loop through this block's header
		for(cftIdx = 0; cftIdx < COEFF_COUNT; cftIdx++)
		{   
            // 0->1 bit, 1->2 bits, 2->4 bits, 3->8 bits
			reqBits = Header[blockIdx][cftIdx] == 0 ? 1 : ( 2 << (Header[blockIdx][cftIdx] - 1) );

            //amount of bits to shift so to place what we want where we want it
			bitShft = bitIdx - reqBits;

			if(bitShft == 0)//last fetch from current Data byte
			{
				//pull entire byte, requested bits are obtained with the mask applied after these else-ifs
				coeff = Data[dataIdx];
				++dataIdx;
				bitIdx = 8;
			}
			else if(bitShft < 0)//trailing bits, supports any amount of trailing between 1-8
			{
				//pull remaining bits, leave space for additional bits stored in next Data byte
				coeff = (Data[dataIdx] << (-bitShft));//i.e reqBits - bitIdx
				bitIdx = 8 - (-bitShft);
				++dataIdx;
				coeff |= Data[dataIdx] >> bitIdx;
			}
			else//bitShift > 0, fetch normally
			{
				coeff = Data[dataIdx] >> bitShft;
				bitIdx -= reqBits;
			}

			coeff &= bitmask[ Header[blockIdx][cftIdx] ];

			if(cftIdx < ACTUAL_COEFFS)//real
			{
				Coeffs[blockIdx][0][cftIdx] = ((coeff - 128) << 13);//offset and scale back
			}
			else//img
			{
				Coeffs[blockIdx][1][cftIdx - ACTUAL_COEFFS] = ((coeff - 128) << 13);
			}
		}
	}
    return Coeffs;
}
