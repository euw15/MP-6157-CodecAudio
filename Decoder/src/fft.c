//***************************************************************************
//* FileName:       fft.c
//*
//* Description:    Implementations of FFT functions.
//*
//***************************************************************************

#include "fft.h"
#include "GeneralFlags.h"
#include "CosAndSinPreCalc.h"

#if MSVC_COMPILER
#define _USE_MATH_DEFINES
#endif

#include <math.h>

#if C55X
#define M_PI 3.14159265358979323846
#endif

#if !C55X
typedef int64_t LongReg;
#else
typedef long long LongReg;
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

#define QA1 23
#define QB1 4

#define QA2 23
#define QB2 16

#define QA3 23
#define QB3 30

void ifft_fixedpoint(int32_t* realCoeficient, int32_t* imCoeficient, int16_t* SamplesIFFT, int32_t totalsamples)
{
    int32_t arrayaddr = 0;
    int32_t val = 0;
    int32_t k = 0, n = 0;
    int32_t shift = 0;
    int32_t m1_shift = 0;
    int32_t m2_shift = 0;
    LongReg m1 = 0;
    LongReg m2 = 0;

#if C55X
    shift = QB2 - (31 - QA2) + 4;
#else
    shift = QB3 - (31 - QA3) + 4;
#endif

    for (k = 0; k < totalsamples; k++)
    {
        arrayaddr = totalsamples * k;
        val = 0;
        for (n = 0; n < totalsamples; n++, arrayaddr++)
        {
            m1 = (LongReg)(realCoeficient[n]) * (LongReg)(CosArray[arrayaddr]);
            m2 = (LongReg)(imCoeficient[n]) * (LongReg)(SinArray[arrayaddr]);
            m1 >>= shift;
            m2 >>= shift;
            m1_shift = m1;
            m2_shift = m2;
            val += m1_shift;
            val -= m2_shift;
        }
        val = val / totalsamples;
        val >>= QB1;
        SamplesIFFT[k] = val;
    }
}
