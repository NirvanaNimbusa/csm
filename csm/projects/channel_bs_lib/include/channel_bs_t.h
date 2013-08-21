//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: channel_bs_t.h
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
#ifndef _CHANNEL_BS_T_
#define _CHANNEL_BS_T_

#include "codeword_t.h"

#include <stdio.h>

#define SELF channel_bs_t self
typedef struct channel_bs_bs channel_bs_base_t;
typedef channel_bs_base_t* channel_bs_t;

struct channel_bs_bs {
	int bits_to_error;
	int errors_quantity;
	double ber;
	FILE* log;
	int bits_transferred_cnt;
	int bits_corrupted_cnt;
	int codewords_transferred_cnt;
	int codewords_corrupted_cnt;
	int (*start) (SELF);
	int (*stop) (SELF);
	codeword_t (*transfer) (SELF, codeword_t codeword);

	/* Options. */
	char channel_created_with_q;
};

void channel_bs_init (SELF);
void channel_bs_deinit (SELF);

channel_bs_t channel_bs_create(FILE* log, double ber);
channel_bs_t channel_bs_create_q(FILE* log, int errors_quantity);

void channel_bs_destroy (SELF);

int channel_bs_start (SELF);
int channel_bs_stop (SELF);
codeword_t channel_bs_transfer (SELF, codeword_t codeword);

#undef SELF

#endif