//***************************************************************************
//* FileName:       WavDecoder.c
//*
//* Description:    Implementation of the wav writer.
//*
//***************************************************************************

#include "WavDecoder.h"
#include "GeneralFlags.h"
#include <stdio.h>

//***************************************************************************
//* Struct Name:    WavFile
//*
//* Purpose:        Struct which represents a wav header file information.
//*
//***************************************************************************
struct WavFile
{
    uint32_t ChunkId;            // Should always contain "RIFF" (for little endian)
    uint32_t ChunkSize;          // The size of the entire file in bytes minus 8 bytes for the two fields not included in this count: ChunkI and ChunkSize.
    uint32_t Format;             // should be "WAVE"
    uint32_t Subchunk1Id;        // Contains the letters "fmt "
    uint32_t Subchunk1Size;      // 16 for PCM.  This is the size of the rest of the Subchunk which follows this
    uint16_t AudioFormat;        // PCM = 1 (i.e. Linear quantization) Values other than 1 indicate some form of compression.
    uint16_t NumChannels;        // Mono = 1, Stereo = 2, etc. 
    uint32_t SampleRate;         // 8000, 44100, etc.
    uint32_t ByteRate;           // == SampleRate * NumChannels * BitsPerSample / 8
    uint16_t BlockAlign;         // == NumChannels * BitsPerSample / 8 e number of bytes for one sample including all channels.
    uint16_t BitsPerSample;      // 8 bits, 16 bits, etc.
    uint32_t Subchunk2Id;        // Contains the letters "data"
    uint32_t Subchunk2Size;      // == NumSamples * NumChannels * BitsPerSample / 8, This is the number of bytes in the data.
};

//***************************************************************************
//* Function Name:  FormatUint32ForWrite
//*
//* Purpose:        Formats a unsigned int of 32 bits (big endian) to be write as a 
//*                 little endian value.
//*
//* Parameters:     Value   - IN - A unsigned int of 32 bits.
//*
//* Returns:        A little endian unsigned int of 32 bits.
//*
//***************************************************************************
uint32_t FormatUint32ForWrite(uint32_t Value)
{
    uint32_t block = 0;
    uint32_t result = 0;

    //First Byte (D)
    block = Value << 24;
    result += block;

    // Second Byte (C)
    block = Value << 8;
    block &= 0x00FF0000;
    result += block;

    // Third Byte (B)
    block = Value >> 8;
    block &= 0x0000FF00;
    result += block;

    // Four Byte(A)
    block = Value >> 24;
    result += block;

    return result;
}

//***************************************************************************
//* Function Name:  FormatUint16ForWrite
//*
//* Purpose:        Formats a unsigned int of 16 bits (big endian) to be write as a 
//*                 little endian value.
//*
//* Parameters:     Value   - IN - A unsigned int of 16bits.
//*
//* Returns:        A little endian unsigned int of 16 bits.
//*
//***************************************************************************
uint16_t FormatUint16ForWrite(uint16_t Value) 
{
    uint16_t result = Value << 8;
    result += Value >> 8;
    return result;
}

//***************************************************************************
//* Function Name:  WavWriter
//*
//* Purpose:        Fills the wav header struct with information like ChunkId, Format,
//*                 Audio format, Frequency, ByteRate, Channels, SampleRate, etc.
//*
//* Parameters:     Header          - IN - The wav header struct to fill.
//*                 NumOfSamples    - IN - The number of samples that the wav file will contain.
//*
//* Returns:        None.
//*
//***************************************************************************
void FillWavHeader(struct WavFile* Header, uint32_t NumOfSamples)
{
    // Constants
    const uint32_t c_iHeaderSizeInBytes = 44 - 8;   // Ignoring the first two chunks ChunkId and ChunkSize
    const uint32_t c_iChunkId = 0x52494646;         // "RIFF"  
    const uint32_t c_iFormat = 0x57415645;          // "WAVE"  
    const uint32_t c_iSubchunk1Id = 0x666d7420;     // "fmt "
    const uint32_t c_iSubchunk1Size = 16;           // 16 for PCM
    const uint16_t c_iAudioFormat = 1;              // PCM = 1 (no compression)
    const uint16_t c_iNumChannels = 1;              // Stereo = 2
    const uint32_t c_iSampleRate = 8000;            // Frequency = 8kHz
    const uint16_t c_iBitsPerSample = 16;           // 16 bits
    const uint32_t c_iSubchunk2Id = 0x64617461;     // "data"

    // Calculated variables
    const uint32_t c_iByteRate = c_iSampleRate * (uint32_t)c_iNumChannels * ((uint32_t)c_iBitsPerSample / 8);
    const uint16_t c_iBlockAlign = c_iNumChannels * (c_iBitsPerSample / 8);
    const uint32_t c_iSubChunk2Size = NumOfSamples * (uint32_t)c_iNumChannels * ((uint32_t)c_iBitsPerSample / 8);
    const uint32_t c_iChunkSize = c_iHeaderSizeInBytes + (NumOfSamples * 2);   // Number of bytes
    
    // Assign values
    Header->ChunkId         = FormatUint32ForWrite(c_iChunkId);
    Header->ChunkSize       = c_iChunkSize;
    Header->Format          = FormatUint32ForWrite(c_iFormat);
    Header->Subchunk1Id     = FormatUint32ForWrite(c_iSubchunk1Id);
    Header->Subchunk1Size   = c_iSubchunk1Size;
    Header->AudioFormat     = c_iAudioFormat;
    Header->NumChannels     = c_iNumChannels;
    Header->SampleRate      = c_iSampleRate;
    Header->ByteRate        = c_iByteRate;
    Header->BlockAlign      = c_iBlockAlign;
    Header->BitsPerSample   = c_iBitsPerSample;
    Header->Subchunk2Id     = FormatUint32ForWrite(c_iSubchunk2Id);
    Header->Subchunk2Size   = c_iSubChunk2Size;
}

//***************************************************************************
//* Function Name:  WavWriter
//*
//* Purpose:        Creates a Wav file for a given number of samples. Using PCM,
//*                 16 bits per sample, Stereo, and at frequency of 8kHz.
//*
//* Parameters:     FileName        - IN - A pointer to the array of real coefficients.
//*                 Samples	        - IN - A pointer to the array of samples.
//*                 NumOfSamples    - IN - Total number of samples located in the array.
//*
//* Returns:        SUCCESS if the wav file was generated successfully.
//*                 FAILURE in case of error.
//*
//***************************************************************************
int WavWriter(const char* FileName, int16_t* Samples, uint32_t NumOfSamples)
{
    int Status = SUCCESS;
    FILE* pFile = NULL;
    struct WavFile WavHeaderFile;
    FillWavHeader(&WavHeaderFile, NumOfSamples);
    pFile = fopen(FileName, "wb");
    if (NULL == pFile) 
    {
        Status = FAILURE;
    }
    if (SUCCESS == Status) 
    {
        fwrite(&WavHeaderFile, sizeof(struct WavFile), 1, pFile);
        fwrite(Samples, sizeof(int16_t), NumOfSamples, pFile);
        fclose(pFile);
    }
    return Status;
}
