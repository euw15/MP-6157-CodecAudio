//***************************************************************************
//* FileName:       fft.c
//*
//* Description:    Implementations of FFT functions.
//*
//***************************************************************************

#include "fft.h"
#include "GeneralFlags.h"

#if MSVC_COMPILER
#define _USE_MATH_DEFINES
#endif

#include <math.h>

#if C55X
#define M_PI 3.14159265358979323846
#endif

//***************************************************************************
//* Function Name:  fft
//*
//* Purpose:        Calculates the fft for a given set of samples.
//*
//* Parameters:     samples         - IN - A pointer to the array of real coefficients.
//*                 realCoeficient	- OUT - A pointer to the array where the real coefficients will be stored.
//*                 imCoeficient    - OUT - A pointer to the array where the imaginary coefficients will be stored.
//*                 totalsamples    - IN - Total number of samples.
//*
//* Returns:        None.
//*
//***************************************************************************
void fft(double* samples, double* realCoeficient, double* imCoeficient, int totalsamples)
{
    double OUT_re = 0;
    double OUT_im = 0;
    int k = 0, n = 0;

    for (k = 0; k < totalsamples; k++) 
    {
        OUT_re = 0;
        OUT_im = 0;
        for (n = 0; n < totalsamples; n++)
        {
            OUT_re += samples[n] * cos(-2.0*M_PI*k*n / totalsamples);
            OUT_im += samples[n] * sin(-2.0*M_PI*k*n / totalsamples);
        }
        realCoeficient[k] = OUT_re;
        imCoeficient[k] = OUT_im;
    }
}

//***************************************************************************
//* Function Name:  ifft
//*
//* Purpose:        Calculates the ifft for a given set of coefficients in pairs of real and
//*                 imaginary values.
//*
//* Parameters:     realCoeficient  - IN - A pointer to the array of real coefficients.
//*                 imCoeficient	- IN - A pointer to the array of imaginary coefficients.
//*                 SamplesIFFT     - OUT - The samples calculated by the ifft for the given coefficients.
//*                 totalsamples    - IN - Total number of coefficients.
//*
//* Returns:        None.
//*
//***************************************************************************
void ifft(int* realCoeficient, int* imCoeficient, double* SamplesIFFT, int totalsamples)
{
    double val = 0;
    int k = 0, n = 0;

    for (k = 0; k < totalsamples; k++) {
        val = 0;
        for (n = 0; n < totalsamples; n++)
        {
            val += realCoeficient[n] * cos(2.0*M_PI*k*n / totalsamples) - imCoeficient[n] * sin(2.0*M_PI*k*n / totalsamples);
        }
        SamplesIFFT[k] = val / totalsamples;
    }
}

void ifft16(int* realCoeficient, int* imCoeficient, int16_t* SamplesIFFT, int totalsamples)
{
    double val = 0;
    int k = 0, n = 0;

    for (k = 0; k < totalsamples; k++) {
        val = 0;
        for (n = 0; n < totalsamples; n++)
        {
            val += realCoeficient[n] * cos(2.0*M_PI*k*n / totalsamples) - imCoeficient[n] * sin(2.0*M_PI*k*n / totalsamples);
        }
        SamplesIFFT[k] = (int16_t)(val / totalsamples);
    }
}