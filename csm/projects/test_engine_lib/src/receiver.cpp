//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: receiver.cpp
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
#include "receiver_t.h"

#include <stdlib.h>

receiver_t receiver_create(FILE* log, int frame_size, int data_size)
{
	receiver_t self = (receiver_t)malloc(sizeof(receiver_base_t));
	if(!self) return NULL;

	self->log = log;
	self->data_size = data_size;
	self->frame_size = frame_size;
	self->data = (int*)malloc(sizeof(int) * data_size);

	receiver_init(self);
	return self;
}

void receiver_init (receiver_t self)
{
	log2(self->log, "receiver initialization started\n");
	self->start = receiver_start;
	self->stop = receiver_stop;
	self->receive_frame = receiver_receive_frame;
}

void receiver_destroy (receiver_t self)
{
	if(!self) return;
	receiver_deinit (self);
}

void receiver_deinit (receiver_t self)
{
	free(self->data);
	free(self);
}

static int received_bits_cnt = 0;

int receiver_start(receiver_t self)
{
	log2(self->log, "receiver started\n");
	self->received_frames_cnt = 0;
	self->received_frames_valid_cnt = 0;
	self->received_frames_invalid_cnt = 0;
	self->received_bits_cnt = 0;
	self->received_bits_valid_cnt = 0;
	self->received_bits_invalid_cnt = 0;
	received_bits_cnt = 0;
	return 0;
}

int receiver_stop(receiver_t self)
{
	log(self->log, "\n");
	log(self->log, "... receiver stopped\n");

	log(self->log, "... receiver settings ...\n");
	log(self->log, "data size: %d\n", self->data_size);
	log(self->log, "frame size: %d\n", self->frame_size);

	log(self->log, "... receiver statistics ...\n");
	log(self->log, "received frames: %d\n", self->received_frames_cnt);
	log(self->log, "valid frames: %d (%.2f%%)\n", self->received_frames_valid_cnt,
		(double)self->received_frames_valid_cnt/(double)self->received_frames_cnt*100);
	log(self->log, "invalid frames: %d (%.2f%%)\n", self->received_frames_invalid_cnt,
		(double)self->received_frames_invalid_cnt/(double)self->received_frames_cnt*100);
	log(self->log, "received bits: %d\n", self->received_bits_cnt);
	log(self->log, "valid bits: %d (%.2f%%)\n", self->received_bits_valid_cnt,
		(double)self->received_bits_valid_cnt/(double)self->received_bits_cnt*100);
	log(self->log, "invalid bits: %d (%.2f%%)\n", self->received_bits_invalid_cnt,
		(double)self->received_bits_invalid_cnt/(double)self->received_bits_cnt*100);
	double ber = (double)self->received_bits_invalid_cnt/(double)self->received_bits_cnt;
	log(self->log, "RBER: %f\n", ber);
	double fer = (double)self->received_frames_invalid_cnt/(double)self->received_frames_cnt;
	log(self->log, "RFER: %f\n", fer);
	return 0;
}

int receiver_receive_frame (receiver_t self, frame_t const frame_out, frame_t const frame_in)
{
	self->received_frames_cnt++;
	self->received_bits_cnt += self->frame_size;

	if (frame_cmp(frame_in, frame_out))
	{
		self->received_frames_valid_cnt++;
		self->received_bits_valid_cnt += frame_find_valid_bits(frame_in, frame_out);
	}
	else
	{
		self->received_frames_invalid_cnt++;

		/* find valid and invalid both */
		self->received_bits_valid_cnt += frame_find_valid_bits(frame_in, frame_out);
		self->received_bits_invalid_cnt += frame_find_invalid_bits(frame_in, frame_out);
	}

	int i = 0;
	
	if(self->data_size - received_bits_cnt >= self->frame_size)
	{
		for (i = 0; i < self->frame_size; i++)
			self->data[received_bits_cnt++] = frame_out->xframe[i];
	}
	else
	{
		int bits_q = self->data_size - received_bits_cnt;
		for (i = 0; i <	bits_q; i++)
			self->data[received_bits_cnt++] = frame_out->xframe[i];
	}

	frame_destroy(frame_out);
	frame_destroy(frame_in);
	return 0;
}

int* receiver_get_received_data(receiver_t self)
{
	return self->data;
}

