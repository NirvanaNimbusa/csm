//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: frame_t.cpp
// Author: Pavel Morozkin
// Date: May 31th 2013
// Revised: May 31th 2013
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
#include "frame_t.h"
#include "common_u.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void frame_init (frame_t const self)
{
	self->xframe = (int*)calloc(self->xsize, sizeof(int));
}

frame_t frame_create (int size)
{
	frame_t self = (frame_t)malloc(sizeof(frame_base_t));
	self->xsize = size;
	if(self)
		frame_init(self);
	return self;
}

void frame_deinit (frame_t self)
{
	free (self->xframe);
	free (self);
}

void frame_destroy (frame_t self)
{
	if(self)
		frame_deinit(self);
}

void frame_display(frame_t self)
{
	int i = 0;
	while(i < self->xsize)
		printf_d("%d", self->xframe[i++]);
	printf_d("\n");
}

char* frame_to_str(frame_t self)
{
	char* str = (char*)malloc(sizeof(char) * self->xsize + 1);
	int i;

	for (i = 0; i < self->xsize; i++)
		str[i] = self->xframe[i] ? '1' : '0';
	str[i] = 0;

	return str;
}

int frame_cmp (frame_t a, frame_t b)
{
	for (int i = 0; i < a->xsize; i++)
	{
		if(a->xframe[i] != b->xframe[i])
			return 0;
	}		
	return 1;
}

frame_t frame_cpy(frame_t dest, frame_t src)
{
	for (int i = 0; i < dest->xsize; i++)
		dest->xframe[i] = src->xframe[i];
	dest->xsize = src->xsize;
	return dest;
}

int frame_find_invalid_bits(frame_t a, frame_t b)
{
	int invalid_bits = 0;
	for (int i = 0; i < a->xsize; i++)
	{
		if(a->xframe[i] != b->xframe[i])
			invalid_bits++;
	}
	return invalid_bits;
}

int frame_find_valid_bits(frame_t a, frame_t b)
{
	int valid_bits = 0;
	for (int i = 0; i < a->xsize; i++)
	{
		if(a->xframe[i] == b->xframe[i])
			valid_bits++;
	}
	return valid_bits;
}