//***************************************************************************
//* FileName:       GeneralFlags.h
//*
//* Description:    Defines specific defines for specific compilers and others.
//*
//***************************************************************************

#ifndef COMPILER_H_
#define COMPILER_H_

#include <stdint.h>

#define MSVC_COMPILER 1
#define GCC_COMPILER  0

#define SUCCESS  0
#define FAILURE -1

#define TRACING 0
#define NULL 0

#define C55X 0

#define SAMPLES_PER_BLOCK       64
#define SAMPLES_PER_BLOCK_SQR   4096

#endif