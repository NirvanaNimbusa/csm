//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: codeword_t.cpp
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
#include "codeword_t.h"
#include "common_u.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void codeword_init (codeword_t const self)
{
	self->xcodeword = (int*)calloc(self->xsize, sizeof(int));
}

codeword_t codeword_create(int size)
{
	codeword_t self = (codeword_t)malloc(sizeof(codeword_base_t));
	if(self) 
	{
		self->xsize = size;
		codeword_init(self);
		return self;
	}
	return NULL;
}

void codeword_deinit (codeword_t self)
{
	free (self->xcodeword);
	free (self);
}

void codeword_destroy (codeword_t self)
{
	if(self)
		codeword_deinit(self);
}

void codeword_display (codeword_t self)
{
	int i = 0;
	while(i < self->xsize)
		printf_d("%d", self->xcodeword[i++]);
	printf_d("\n");
}

void codeword_display_diff_color (codeword_t src_codeword, codeword_t new_codeword,
								  int diff_color, int normal_color)
{
	for (int i = 0; i < src_codeword->xsize; i++)
	{
		if (src_codeword->xcodeword[i] != new_codeword->xcodeword[i])
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), diff_color);
			printf_d("%d", new_codeword->xcodeword[i]);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), normal_color);
		}
		else
			printf_d("%d", new_codeword->xcodeword[i]);
	}
	printf_d("\n");
}

codeword_t codeword_cpy(codeword_t d_codeword, codeword_t s_codeword)
{
	d_codeword->xsize = s_codeword->xsize;
	for (int i = 0; i < d_codeword->xsize; i++)
		d_codeword->xcodeword[i] = s_codeword->xcodeword[i];
	return d_codeword;
}

int codeword_cmp(codeword_t f_codeword, codeword_t s_codeword)
{
	for (int i = 0; i < f_codeword->xsize; i++)
	{
		if(f_codeword->xcodeword[i] != s_codeword->xcodeword[i])
			return 0;
	}		
	return 1;
}

int codeword_find_invalid_bits	(codeword_t f_codeword, codeword_t s_codeword)
{
	int invalid_bits = 0;
	for (int i = 0; i < f_codeword->xsize; i++)
	{
		if(f_codeword->xcodeword[i] != s_codeword->xcodeword[i])
			invalid_bits++;
	}
	return invalid_bits;
}

int codeword_find_valid_bits (codeword_t f_codeword, codeword_t s_codeword)
{
	int valid_bits = 0;
	for (int i = 0; i < f_codeword->xsize; i++)
	{
		if(f_codeword->xcodeword[i] == s_codeword->xcodeword[i])
			valid_bits++;
	}
	return valid_bits;
}

