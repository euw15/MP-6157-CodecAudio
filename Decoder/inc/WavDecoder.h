//***************************************************************************
//* FileName:       WavDecoder.h
//*
//* Description:    Header file which contains the declarations of a wav 
//*                 writer.
//*
//***************************************************************************

#ifndef WAVDECODER_H_
#define WAVDECODER_H_

#include <stdint.h> // for int16_t and int32_t

int WavWriter(const char* FileName, int16_t* Samples, uint32_t NumOfSamples);

#endif