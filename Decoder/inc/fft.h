//***************************************************************************
//* FileName:       fft.h
//*
//* Description:    Header file which contains declarations of FFT functions.
//*
//***************************************************************************
#include <stdint.h>

#ifndef FFT_H_
#define FFT_H_

void fft(double* samples, double* realCoeficient, double* imCoeficient, int totalsamples);
void ifft(int* realCoeficient, int* imCoeficient, double* SamplesIFFT, int totalsamples);

void ifft_fixedpoint(int32_t* realCoeficient, int32_t* imCoeficient, int16_t* SamplesIFFT, int32_t totalsamples);

#endif
