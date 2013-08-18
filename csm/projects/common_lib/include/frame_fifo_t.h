//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: frame_fifo_t.h
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
#ifndef _FRAME_FIFO_T_
#define _FRAME_FIFO_T_

#include "frame_t.h"

typedef struct frame_fifo frame_fifo_base_t;
typedef frame_fifo_base_t* frame_fifo_t;

struct frame_fifo
{
	frame_t* fifo;
	int frame_size;
	int fifo_size;
	int length;
};

void frame_fifo_init (frame_fifo_t const self);
frame_fifo_t frame_fifo_create(int fifo_size, int frame_size);
void frame_fifo_deinit (frame_fifo_t self);
void frame_fifo_destroy (frame_fifo_t self);

void frame_fifo_print (frame_fifo_t self);
frame_fifo_t frame_fifo_put (frame_fifo_t fifo, frame_t element);
frame_t frame_fifo_get (frame_fifo_t fifo);
void reset_frame_fifo (frame_fifo_t fifo);
int frame_fifo_is_empty(frame_fifo_t self);

#endif