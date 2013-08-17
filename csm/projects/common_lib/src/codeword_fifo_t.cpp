//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: codeword_fifo_t.cpp
// Author: Pavel Morozkin
// Date: August 17th 2013
// Revised: August 17th 2013
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
#include "codeword_fifo_t.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void codeword_fifo_init (codeword_fifo_t const self)
{
	self->length = 0;
	self->fifo = (codeword_t*)malloc(sizeof(codeword_t) * self->fifo_size);
	if(self->fifo != NULL)
		for (int i = 0; i < self->fifo_size; i++)
			self->fifo[i] = codeword_create(self->codeword_size);
}

codeword_fifo_t codeword_fifo_create(int fifo_size, int codeword_size)
{
	codeword_fifo_t self = (codeword_fifo_t)malloc(sizeof(codeword_fifo_base_t));
	if(!self) return NULL;

	self->fifo_size = fifo_size;
	self->codeword_size = codeword_size;

	codeword_fifo_init(self);
	return self;
}

void codeword_fifo_deinit (codeword_fifo_t self)
{
	for (int i = 0; i < self->fifo_size; i++)
		codeword_destroy(self->fifo[i]);
	free (self->fifo);
	free (self);
}

void codeword_fifo_destroy (codeword_fifo_t self)
{
	if(self)
		codeword_fifo_deinit(self);
}

void codeword_fifo_print (codeword_fifo_t self)
{
	for (int i = 0; i < self->length; i++)
		codeword_display(self->fifo[i]);
}

codeword_fifo_t codeword_fifo_put (codeword_fifo_t fifo, codeword_t element)
{
	if (fifo->length >= fifo->fifo_size) xexit("codeword fifo: fifo is full");
	int index = fifo->length;
	fifo->fifo[index] = codeword_cpy(fifo->fifo[index], element);
	fifo->length++;
	return fifo;
}

codeword_t codeword_fifo_get (codeword_fifo_t fifo)
{
	if (fifo->length == 0) return NULL; //xexit("codeword fifo: fifo length below zero");

	codeword_t element = codeword_create(fifo->codeword_size);
	element = codeword_cpy(element, fifo->fifo[0]);

	for (int i = 0; i < fifo->length - 1; i++)
		fifo->fifo[i] = codeword_cpy(fifo->fifo[i], fifo->fifo[i+1]);

	fifo->length--;

	return element;
}

void reset_codeword_fifo (codeword_fifo_t fifo)
{
	fifo->length = 0;
}
