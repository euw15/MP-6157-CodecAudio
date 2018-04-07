#include <stdio.h>
#include <stdlib.h>
#include "UtilFuncs.h"

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
