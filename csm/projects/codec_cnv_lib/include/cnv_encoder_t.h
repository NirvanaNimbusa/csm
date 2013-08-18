//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: cnv_encoder_t.h
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
#ifndef _CNV_ENCODER_T_
#define _CNV_ENCODER_T_

#include "frame_t.h"
#include "codeword_t.h"
#include "common.h"

#include <stdio.h>

#define SELF cnv_encoder_t self
typedef struct cnv_encoder cnv_encoder_base_t;
typedef cnv_encoder_base_t* cnv_encoder_t;

struct cnv_encoder {
	int* regs;
	int regs_q;
	int codeword_length;
	FILE* log;

	int encoded_frames_cnt;
	int generated_codewords_cnt;
	int transmitted_bits_cnt;

	int (*start) (SELF);
	int (*stop) (SELF);
	codeword_t (*encode) (SELF, frame_t frame);
};

void cnv_encoder_init (SELF);
void cnv_encoder_deinit (SELF);

cnv_encoder_t cnv_encoder_create (FILE* log, int regs_q, int codeword_length);
void cnv_encoder_destroy (SELF);

int cnv_encoder_start (SELF);
int cnv_encoder_stop (SELF);

codeword_t cnv_encoder_encode (SELF, frame_t frame);

#undef SELF

#endif