//***************************************************************************
//* FileName:       fft.h
//*
//* Description:    Header file which contains declarations of FFT functions.
//*
//***************************************************************************

#ifndef FFT_H_
#define FFT_H_

void fft(double* samples, double* realCoeficient, double* imCoeficient, int totalsamples);
void ifft(double* realCoeficient, double* imCoeficient, double* SamplesIFFT, int totalsamples);

#endif