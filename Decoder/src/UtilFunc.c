#include <stdio.h>
#include <stdlib.h>
#include "UtilFuncs.h"

// Defines
#define COEFF_COUNT 64

// Global variables
uint16_t N;
uint8_t* Header; //of size N, populated in ExtractDescriptor. It holds the amount of bits used to store each coefficient

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
//* Returns:		void
//*
//***************************************************************************
void ExtractDescriptor(char* File)
{
	uint16_t headerIdx = 0;
	N = File[0];
	N <<= 8;
	N |= File[1];
	int limit = N + 2;
	uint8_t byte;
	
	Header = (uint8_t*) malloc(COEFF_COUNT * N * sizeof(uint8_t));
	
	for(int i = 2; i < limit; i++)
	{
		byte = File[i];
		Header[headerIdx]   = (byte & 0xC0) >> 6;
		Header[headerIdx+1] = (byte & 0x30) >> 4;
		Header[headerIdx+2] = (byte & 0xC) >> 2;
		Header[headerIdx+3] = (byte & 0x3);
		headerIdx += 4;
	}
}


