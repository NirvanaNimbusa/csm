//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: frame_fifo_t.cpp
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
#include "frame_fifo_t.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void frame_fifo_init (frame_fifo_t const self)
{
	self->length = 0;
	self->fifo = (frame_t*)malloc(sizeof(frame_t) * self->fifo_size);
	if(self->fifo)
		for (int i = 0; i < self->fifo_size; i++)
			self->fifo[i] = frame_create(self->frame_size);
}

frame_fifo_t frame_fifo_create(int fifo_size, int frame_size)
{
	frame_fifo_t self = (frame_fifo_t)malloc(sizeof(frame_fifo_base_t));
	if(!self) return NULL;
	
	self->fifo_size = fifo_size;
	self->frame_size = frame_size;

	frame_fifo_init(self);
	return self;
}

void frame_fifo_deinit (frame_fifo_t self)
{
	for (int i = 0; i < self->fifo_size; i++)
		frame_destroy(self->fifo[i]);
	free (self->fifo);
	free (self);
}

void frame_fifo_destroy (frame_fifo_t self)
{
	if(self)
		frame_fifo_deinit(self);
}

void frame_fifo_print (frame_fifo_t self)
{
	for (int i = 0; i < self->length; i++)
		frame_display(self->fifo[i]);
}

frame_fifo_t frame_fifo_put (frame_fifo_t fifo, frame_t element)
{
	if (fifo->length >= fifo->fifo_size) xexit("frame fifo: fifo is full");
	int index = fifo->length;
	fifo->fifo[index] = frame_cpy(fifo->fifo[index], element);
	fifo->length++;
	return fifo;
}

frame_t frame_fifo_get (frame_fifo_t fifo)
{
	if (fifo->length == 0) return NULL; //xexit("frame fifo: fifo length below zero");

	frame_t element = frame_create(fifo->frame_size);
	element = frame_cpy(element, fifo->fifo[0]);
	
	for (int i = 0; i < fifo->length - 1; i++)
		fifo->fifo[i] = frame_cpy(fifo->fifo[i], fifo->fifo[i+1]);
	
	fifo->length--;

	return element;
}

void reset_frame_fifo (frame_fifo_t fifo)
{
	fifo->length = 0;
}

int frame_fifo_is_empty(frame_fifo_t self)
{
	return self->length ? 0 : 1;
}
