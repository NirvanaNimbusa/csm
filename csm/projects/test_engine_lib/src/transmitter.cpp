//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: transmitter.cpp
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
#include "transmitter_t.h"

#include <stdlib.h>

transmitter_t transmitter_create (FILE* log, int frame_size, int* data, int data_size)
{
	transmitter_t self = (transmitter_t)malloc(sizeof(transmitter_base_t));
	if(!self) return NULL;
	
	self->log = log;
	self->data_size = data_size;
	self->frame_size = frame_size;
	self->data = data;

	transmitter_init(self);
	return self;
}

void transmitter_init (transmitter_t const self)
{
	log2(self->log, "transmitter initialization started\n");
	self->start = transmitter_start;
	self->stop = transmitter_stop;
	self->transmit_frame = transmitter_transmit_frame;
}

void transmitter_destroy(transmitter_t self)
{
	if(!self) return;
	transmitter_deinit(self);
}

void transmitter_deinit(transmitter_t self)
{
	free (self);
}

static int bits_for_transmission = 0;
static int transmitted_bits_cnt = 0;
static int dummy_bit = 0;
static int eod_occured = 0;

int transmitter_start(transmitter_t self)
{
	log2(self->log, "transmitter started\n");
	self->transmitted_frames_cnt = 0;
	self->transmit_bits_cnt = 0;

	bits_for_transmission = self->data_size;
	transmitted_bits_cnt = 0;
	 dummy_bit = 0;
	eod_occured = 0;

	return 0;
}

int transmitter_stop(transmitter_t self)
{
	log(self->log, "\n");
	log(self->log, "... transmitter stopped\n");

	log(self->log, "... transmitter settings ...\n");
	log(self->log, "data size: %d\n", self->data_size);
	log(self->log, "frame size: %d\n", self->frame_size);

	log(self->log, "... transmitter statistics ...\n");
	log(self->log, "transmitted frames: %d\n", self->transmitted_frames_cnt);
	log(self->log, "transmitted bits: %d\n", self->transmit_bits_cnt);
	return 0;
}



frame_t transmitter_transmit_frame(transmitter_t self)
{
	log2(self->log, "transmit frame\n");

	int i = 0;
	
	if (eod_occured)
	{
		//log(self->log, "eod occured, transmission aborted\n");
		return NULL;
	}

	int frame_size = self->frame_size;
	frame_t frame = frame_create(frame_size);

	if(bits_for_transmission - self->frame_size > 0)
	{
		for (i = 0; i < frame_size; i++)
			frame->xframe[i] = self->data[transmitted_bits_cnt++];
		bits_for_transmission -= frame_size;
	}
	else if (bits_for_transmission - self->frame_size == 0)
	{
		for (i = 0; i < frame_size; i++)
			frame->xframe[i] = self->data[transmitted_bits_cnt++];
		eod_occured = 1;
	}
	else
	{
		for (i = 0; i < bits_for_transmission; i++)
			frame->xframe[i] = self->data[transmitted_bits_cnt++];
		for (; i < frame_size; i++)
			frame->xframe[i] = dummy_bit;
		eod_occured = 1;
	}

	self->transmitted_frames_cnt++;
	self->transmit_bits_cnt += self->frame_size;
	
	return frame;
}
