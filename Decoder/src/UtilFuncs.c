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

#define SCALE_FCT 14

// Global variables
uint16_t N = 0;
headerType** Header; //of size N, populated in ExtractDescriptor. It holds the header with amount of bits used to store each coefficient, for each audio block [N, COEFF_COUNT]
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
//#ifndef C55
headerType** ExtractDescriptor(unsigned char* File, int* BlockCount)
{
	int i = 0, j = 0;
	short limit = 0;
	uint16_t headerIdx = 0;
    headerType byte;
	N = 0;

	N = File[1];
	N <<= 8;
	*BlockCount = (N |= File[0]);
	limit = (N * WORDS_PER_HEADER) + 2;

	Header = (headerType**) malloc(N * sizeof(headerType*));

	for(i = 2, j = 0; i < limit && j < N; j++)
	{
		Header[j] = (headerType*) malloc(COEFF_COUNT * sizeof(headerType));
		for(headerIdx = 0; headerIdx < COEFF_COUNT; headerIdx+=4, i++)
		{
		    byte = File[i];
			Header[j][headerIdx]   = ((byte & 0xC0) >> 6);
			Header[j][headerIdx+1] = ((byte & 0x30) >> 4);
			Header[j][headerIdx+2] = ((byte & 0xC) >> 2);
			Header[j][headerIdx+3] = (byte & 0x3);
		}
	}
    return Header;
}
//#endif
/*#ifdef C55
uint16_t** ExtractDescriptorC55(unsigned char* File, int* BlockCount)
{
    int i = 0, j = 0;
    short limit = 0;
    uint16_t headerIdx = 0;
    headerType byte;
    *BlockCount = N = File[0];
    limit = (N * WORDS_PER_HEADER) + 1;

    Header = (headerType**)malloc(N * sizeof(headerType*));

    for (i = 2, j = 0; i < limit && j < N; j++)
    {
        Header[j] = (headerType*)malloc(COEFF_COUNT * sizeof(headerType));
        for (headerIdx = 0; headerIdx < COEFF_COUNT; headerIdx += 8, i++)
        {
            byte = File[i];
            Header[j][headerIdx] = ((byte & 0xC000) >> 14);
            Header[j][headerIdx + 1] = ((byte & 0x3000) >> 12);
            Header[j][headerIdx + 2] = ((byte & 0xC00) >> 10);
            Header[j][headerIdx + 3] = ((byte & 0x300) >> 8);
            Header[j][headerIdx + 4] = ((byte & 0xC0) >> 6);
            Header[j][headerIdx + 5] = ((byte & 0x30) >> 4);
            Header[j][headerIdx + 6] = ((byte & 0xC) >> 2);
            Header[j][headerIdx + 7] = (byte & 0x3);
        }
    }
    return Header;
}
#endif*/

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
//*					Coeff[N : (Audio block count)] [2: (real, img)] [ACTUAL_COEFFS]
//*
//***************************************************************************
coefType*** ExtractCoeffs(unsigned char* File)
{   //header, block idx, coeff number, data idx
	int cftIdx = 0, blockIdx = 0, coeff = 0, dataIdx = 0;

    //idx for scanning each byte, bitwise shift amount, requested amount of bits
	short bitIdx = 0, bitShft = 0, reqBits = 0;//vars for byte processing

    //move pointer to start of compressed data, 2 bytes for N, and N * 16 to skip over headers
	//OR in C55									1 word  for N, and N * 8
	unsigned char* Data = File + ((N*WORDS_PER_HEADER) + (WORDS_PER_HEADER / 8));

	// 3 dimensional array for storing complex coeffs
    coefType*** Coeffs = (coefType***) malloc(N * sizeof(coefType**));

	//loop through each audio block (8ms)
	for(dataIdx = 0, blockIdx = 0; blockIdx < N; blockIdx++)
	{
		Coeffs[blockIdx] = (coefType**) malloc(2 * sizeof(coefType*));
		Coeffs[blockIdx][0] = (coefType*) malloc(ACTUAL_COEFFS * sizeof(coefType));
		Coeffs[blockIdx][1] = (coefType*) malloc(ACTUAL_COEFFS * sizeof(coefType));

        //8 or 16 is MSB
		bitIdx = WRD_SIZE;

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
				bitIdx = WRD_SIZE;
			}
			else if(bitShft < 0)//trailing bits, supports any amount of trailing between 1<->WRD_SIZE
			{
				//pull remaining bits, leave space for additional bits stored in next Data byte
				coeff = (Data[dataIdx] << (-bitShft));//i.e reqBits - bitIdx
				bitIdx = WRD_SIZE - (-bitShft);
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
                Coeffs[blockIdx][0][cftIdx] = coeff;
			}
			else//img
			{
                Coeffs[blockIdx][1][cftIdx - ACTUAL_COEFFS] = coeff;
			}
		}
        if (bitShft != 0)
        {
            dataIdx++;
        }
    }
    return Coeffs;
}

//***************************************************************************
//* Function Name:	RetrieveIFFTCoeffs
//*
//* Purpose:		Provides the mirrored coeffs to send into the IFFT
//*
//* Parameters:		coeffs  - IN - FFT Coefficients read into a 3 dimensional array of the form
//*					    Coeff[N : (Audio block count)] [2: (real, img)] [ACTUAL_COEFFS].
//*
//* Returns:		actualCoeffs - expanded coefficients of the form
//*                     Coeff[N : (Audio block count)] [2: (real, img)] [COEFF_COUNT].
//*
//***************************************************************************
coefType*** RetrieveIFFTCoeffs(coefType*** coeffs)
{
    int cftIdx = 0, blockIdx = 0, invIdx = 0;

    // 3 dimensional array for storing complex coeffs
    coefType*** actualCoeffs = (coefType***)malloc(N * sizeof(coefType**));

    //loop through each audio block (8ms)
    for (blockIdx = 0; blockIdx < N; blockIdx++)
    {
        actualCoeffs[blockIdx] = (coefType**)malloc(2 * sizeof(coefType*));
        actualCoeffs[blockIdx][0] = (coefType*)malloc(COEFF_COUNT * sizeof(coefType));
        actualCoeffs[blockIdx][1] = (coefType*)malloc(COEFF_COUNT * sizeof(coefType));

        //1st and 32nd coeffs do not have mirror, nor imaginary part
        actualCoeffs[blockIdx][0][0] = ((coeffs[blockIdx][0][0] - 128) << SCALE_FCT);//offset and scale back
        actualCoeffs[blockIdx][0][32] = ((coeffs[blockIdx][1][0] - 128) << SCALE_FCT);//32nd real coeff(pivot) stored in 1st img, since 1st img is known to be 0

        actualCoeffs[blockIdx][1][0] = 0;//first img coeff is 0
        actualCoeffs[blockIdx][1][32] = 0;

        //loop through each coeff
        for (cftIdx = 1, invIdx = COEFF_COUNT - 1; cftIdx < ACTUAL_COEFFS; cftIdx++, invIdx--)
        {
            actualCoeffs[blockIdx][0][cftIdx] = actualCoeffs[blockIdx][0][invIdx] = ((coeffs[blockIdx][0][cftIdx] - 128) << SCALE_FCT);//offset and scale back
            actualCoeffs[blockIdx][1][cftIdx] = actualCoeffs[blockIdx][1][invIdx] = ((coeffs[blockIdx][1][cftIdx] - 128) << SCALE_FCT);
        }
    }
    return actualCoeffs;
}
