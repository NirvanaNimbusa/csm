//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: bch_encoder.cpp
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
#include "bch_encoder_t.h"

#include <stdlib.h>

bch_encoder_t bch_encoder_create (FILE* log, int galois_field_degree, int code_length, int error_correction)
{
	bch_encoder_t self = (bch_encoder_t)malloc(sizeof(bch_encoder_base_t));
	if(!self) return NULL;

	self->log = log;
	self->galois_field_degree = galois_field_degree;
	self->code_length = code_length;
	self->inf_symbols_q = 0;
	self->error_correction = error_correction;

	bch_encoder_init(self);
	return self;
}

void bch_encoder_init (bch_encoder_t self)
{
	log2(self->log, "bch encoder initialization started\n");
	self->start = bch_encoder_start;
	self->stop = bch_encoder_stop;
	self->encode = bch_encoder_encode;

	self->bch_encoder_kernel = bch_encoder_kernel_create(self->log, self->galois_field_degree,
		self->code_length, self->inf_symbols_q, self->error_correction);

	self->inf_symbols_q = self->bch_encoder_kernel->inf_symbols_q;
}

void bch_encoder_deinit (bch_encoder_t self)
{
	bch_encoder_kernel_destroy(self->bch_encoder_kernel);
	free(self);
}

void bch_encoder_destroy (bch_encoder_t self)
{
	if(!self) return;
	bch_encoder_deinit (self);
}

int bch_encoder_start (bch_encoder_t self)
{
	log2(self->log, "bch encoder started\n");
	self->encoded_frames_cnt = 0;
	self->generated_codewords_cnt = 0;
	self->transmitted_bits_cnt = 0;
	return 0;
}

int bch_encoder_stop (bch_encoder_t self)
{
	log(self->log, "\n");
	log(self->log, "... bch encoder stopped\n");

	log(self->log, "... bch encoder settings ...\n");
	log(self->log, "galois field degree: %d\n", self->galois_field_degree);
	log(self->log, "code length: %d\n", self->code_length);
	log(self->log, "information symbols quantity: %d\n", self->inf_symbols_q);
	log(self->log, "error correction property: %d\n", self->error_correction);
	
	log(self->log, "... bch encoder statistics ...\n");
	log(self->log, "encoded frames: %d\n", self->encoded_frames_cnt);
	log(self->log, "generated codewords: %d\n", self->generated_codewords_cnt);
	log(self->log, "transmitted bits: %d\n", self->transmitted_bits_cnt);

	return 0;
}

codeword_t bch_encoder_encode (bch_encoder_t self, frame_t frame)
{
	log2(self->log, "encoding started\n");
	codeword_t codeword = bch_encoder_kernel_encode(self->bch_encoder_kernel, frame);

	self->encoded_frames_cnt++;
	self->generated_codewords_cnt++;
	self->transmitted_bits_cnt += self->code_length;

	frame_destroy(frame);
	log2(self->log, "encoding finished\n");
	return codeword;
}

int bch_encoder_get_frame_size(bch_encoder_t self)
{
	return self->inf_symbols_q;
}

int bch_encoder_get_codeword_size(bch_encoder_t self)
{
	return self->code_length;
}