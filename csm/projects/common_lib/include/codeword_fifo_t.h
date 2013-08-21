//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: codeword_fifo_t.h
// Author: Pavel Morozkin
// Date: August 21th 2013
// Revised: August 21th 2013
//*******************************************************************************
// NOTE:
// The author is not responsible for any malfunctioning of this program, nor for
// any damage caused by it. Please include the original program along with these
// comments in any redistribution.
//
// For more information and suggestions, please email me: pavelmorozkin@yandex.ru
//
// COPYRIGHT NOTICE: This computer program is free for non-commercial purposes.
// You may implement this program for any non-commercial application. You may
// also implement this program for commercial purposes, provided that you
// obtain my written permission. Any modification of this program is covered 
// by this copyright.
//
// Copyright (c) 2013, Pavel Morozkin. All rights reserved.
//*******************************************************************************
#ifndef _CODEWORD_FIFO_T_
#define _CODEWORD_FIFO_T_

#include "codeword_t.h"

typedef struct codeword_fifo codeword_fifo_base_t;
typedef codeword_fifo_base_t* codeword_fifo_t;

struct codeword_fifo
{
	codeword_t* fifo;
	int codeword_size;
	int fifo_size;
	int length;
};

void codeword_fifo_init (codeword_fifo_t const self);
codeword_fifo_t codeword_fifo_create(int fifo_size, int codeword_size);
void codeword_fifo_deinit (codeword_fifo_t self);
void codeword_fifo_destroy (codeword_fifo_t self);

void codeword_fifo_print (codeword_fifo_t self);
codeword_fifo_t codeword_fifo_put (codeword_fifo_t fifo, codeword_t element);
codeword_t codeword_fifo_get (codeword_fifo_t fifo);
void reset_codeword_fifo (codeword_fifo_t fifo);

#endif