//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: receiver_t.h
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
#ifndef _RECEIVER_T_
#define _RECEIVER_T_

#include "frame_t.h"
#include "common_u.h"
#include <stdio.h>

#define SELF receiver_t self
typedef struct receiver receiver_base_t;
typedef receiver_base_t* receiver_t;

struct receiver {
	int frame_size;
	int data_size;
	int received_frames_cnt;
	int received_frames_valid_cnt;
	int received_frames_invalid_cnt;
	int received_bits_cnt;
	int received_bits_valid_cnt;
	int received_bits_invalid_cnt;
	FILE* log;
	int* data;
	int (*start) (SELF);
	int (*stop) (SELF);
	int (*receive_frame) (SELF, frame_t const frame_out, frame_t const frame_in);
};

void receiver_init (SELF);
void receiver_deinit (SELF);

receiver_t receiver_create (FILE* log, int frame_size, int data_size);
void receiver_destroy (SELF);

int receiver_start (SELF);
int receiver_stop (SELF);

int receiver_receive_frame (SELF, frame_t const frame_out, frame_t const frame_in);
int* receiver_get_received_data(SELF);

#undef SELF

#endif