//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: bch_encoder_t.h
// Author: Pavel Morozkin
// Date: August 28th 2013
// Revised: August 28th 2013
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
#ifndef _BCH_ENCODER_T_
#define _BCH_ENCODER_T_

#include "common.h"
#include "frame_t.h"
#include "codeword_t.h"
#include "bch_encoder_kernel_t.h"

#include <stdio.h>

#define SELF bch_encoder_t self
typedef struct bch_encoder bch_encoder_base_t;
typedef bch_encoder_base_t* bch_encoder_t;

struct bch_encoder {
	int galois_field_degree;
	int code_length;
	int inf_symbols_q;
	int error_correction;
	FILE* log;

	int encoded_frames_cnt;
	int generated_codewords_cnt;
	int transmitted_bits_cnt;

	bch_encoder_kernel_t bch_encoder_kernel;
	int (*start) (SELF);
	int (*stop) (SELF);
	codeword_t (*encode) (SELF, frame_t frame);
};

void bch_encoder_init (SELF);
void bch_encoder_deinit (SELF);

bch_encoder_t bch_encoder_create (FILE* log, int galois_field_degree, int code_length, int error_correction);
void bch_encoder_destroy (SELF);

int bch_encoder_start (SELF);
int bch_encoder_stop (SELF);

codeword_t bch_encoder_encode (SELF, frame_t frame);

int bch_encoder_get_frame_size(SELF);
int bch_encoder_get_codeword_size(SELF);

#undef SELF

#endif