//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: codeword_t.h
// Author: Pavel Morozkin
// Date: August 18th 2013
// Revised: August 18th 2013
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
#ifndef _CODEWORD_T_
#define _CODEWORD_T_

typedef struct codeword codeword_base_t;
typedef codeword_base_t* codeword_t;

struct codeword
{
	int* xcodeword;
	int xsize;
};

void codeword_init (codeword_t const self);
codeword_t codeword_create(int size);
void codeword_deinit (codeword_t self);
void codeword_destroy (codeword_t self);

void codeword_display (codeword_t self);
void codeword_display_diff_color (codeword_t src_codeword, codeword_t new_codeword,
								  int diff_color, int normal_color);

int codeword_cmp(codeword_t f_codeword, codeword_t s_codeword);
codeword_t codeword_cpy(codeword_t d_codeword, codeword_t s_codeword);

int codeword_find_invalid_bits	(codeword_t f_codeword, codeword_t s_codeword);
int codeword_find_valid_bits (codeword_t f_codeword, codeword_t s_codeword);

#endif