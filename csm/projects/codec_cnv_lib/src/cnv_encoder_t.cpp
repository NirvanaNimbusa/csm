//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: cnv_encoder_t.cpp
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
#include "cnv_encoder_t.h"

#include <stdlib.h>
#include <stdio.h>
#include <iso646.h>

cnv_encoder_t cnv_encoder_create (FILE* log, int regs_q, int codeword_length)
{
	cnv_encoder_t self = (cnv_encoder_t)malloc(sizeof(cnv_encoder_base_t));
	if(!self) return NULL;
	self->log = log;
	self->regs_q = regs_q;
	self->regs = (int*)calloc(self->regs_q, sizeof(int));
	self->codeword_length = codeword_length;
	if(codeword_length % 2) xexit("cnv encoder: wrong codeword length\n");
	cnv_encoder_init(self);
	return self;
}

void cnv_encoder_init (cnv_encoder_t self)
{
	log2(self->log, "cnv encoder initialization started\n");
	self->start = cnv_encoder_start;
	self->stop = cnv_encoder_stop;
	self->encode = cnv_encoder_encode;
}

void cnv_encoder_deinit (cnv_encoder_t self)
{
	free(self->regs);
	free(self);
}

void cnv_encoder_destroy (cnv_encoder_t self)
{
	if(!self) return;
	cnv_encoder_deinit(self);
}

int cnv_encoder_start (cnv_encoder_t self)
{
	log2(self->log, "cnv encoder started\n");
	self->regs[0] = 0;
	self->regs[1] = 0;
	self->encoded_frames_cnt = 0;
	self->generated_codewords_cnt = 0;
	self->transmitted_bits_cnt = 0;
	return 0;
}

int cnv_encoder_stop (cnv_encoder_t self)
{
	log(self->log, "\n");
	log(self->log, "... cnv encoder stopped\n");

	log(self->log, "... cnv encoder settings ...\n");
	log(self->log, "quantity of registers: %d\n", self->regs_q);
	log(self->log, "codeword length: %d\n", self->codeword_length);
	
	log(self->log, "... cnv encoder statistics ...\n");
	log(self->log, "encoded frames: %d\n", self->encoded_frames_cnt);
	log(self->log, "generated codewords: %d\n", self->generated_codewords_cnt);
	log(self->log, "transmitted bits: %d\n", self->transmitted_bits_cnt);

	self->regs[0] = 0;
	self->regs[1] = 0;

	return 0;
}

codeword_t cnv_encoder_encode (cnv_encoder_t self, frame_t frame)
{
	int codeword_length = self->codeword_length;
	codeword_t codeword = codeword_create(codeword_length);

	int i, j, input_bit, output_bit_1, output_bit_2;

	for (i = 0, j = 0; i < frame->xsize; i++)
	{
		// get input bit
		input_bit = frame->xframe[i];

		// find output bits
		output_bit_1 = input_bit xor self->regs[0] xor self->regs[1];
		output_bit_2 = input_bit xor self->regs[1];

		// shift regs
		self->regs[1] = self->regs[0];
		self->regs[0] = input_bit;

		// make codeword
		codeword->xcodeword[j++] = output_bit_1;
		codeword->xcodeword[j++] = output_bit_2;
	}

	frame_destroy(frame);

	self->encoded_frames_cnt++;
	self->generated_codewords_cnt++;
	self->transmitted_bits_cnt += self->codeword_length;

	return codeword;
}