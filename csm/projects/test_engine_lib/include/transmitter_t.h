//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: transmitter_t.h
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
#ifndef _TRANSMITTER_T_
#define _TRANSMITTER_T_

#include "frame_t.h"
#include "common_u.h"
#include <stdio.h>

#define SELF transmitter_t self
typedef struct transmitter transmitter_base_t;
typedef transmitter_base_t* transmitter_t;

struct transmitter {
	int frame_size;
	int data_size;
	int transmitted_frames_cnt;
	int transmit_bits_cnt;
	FILE* log;
	int* data;
	int (*start) (SELF);
	int (*stop) (SELF);
	frame_t (*transmit_frame) (SELF);
};

void transmitter_init (SELF);
void transmitter_deinit (SELF);

transmitter_t transmitter_create (FILE* log, int frame_size, int* data, int data_size);
void transmitter_destroy (SELF);

int transmitter_start (SELF);
int transmitter_stop (SELF);

frame_t transmitter_transmit_frame (SELF);

#undef SELF

#endif