//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: channel_bs.cpp
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
#include "channel_bs_t.h"
#include "my_math.h"
#include "common_u.h"
#include "utilities.h"

#include <stdlib.h>
#include <time.h>

channel_bs_t channel_bs_create(FILE* log, double ber)
{
	channel_bs_t self = (channel_bs_t)malloc(sizeof(channel_bs_base_t));
	if(!self) return NULL;

	self->ber = ber;
	self->log = log;
	self->channel_created_with_q = 0;

	channel_bs_init(self);
	return self;
}

channel_bs_t channel_bs_create_q(FILE* log, int errors_quantity)
{
	channel_bs_t self = (channel_bs_t)malloc(sizeof(channel_bs_base_t));
	if(!self) return NULL;

	self->errors_quantity = errors_quantity;
	self->log = log;
	self->channel_created_with_q = 1;

	channel_bs_init(self);
	return self;
}

void channel_bs_init (channel_bs_t self)
{
	log2(self->log, "channel bs initialization started\n");
	self->start = channel_bs_start;
	self->stop = channel_bs_stop;
	self->transfer = channel_bs_transfer;

	srand((int)time(NULL));
	if(self->ber == 0)
		self->bits_to_error = (int)1e9;
	else
		self->bits_to_error = (int)(exponential_distr(self->ber));
}

void channel_bs_deinit (channel_bs_t self)
{
	free(self);
}

void channel_bs_destroy (channel_bs_t self)
{
	if(!self) return;
	channel_bs_deinit(self);
}

int channel_bs_start (channel_bs_t self)
{
	log2(self->log, "channel bs started\n");
	self->bits_transferred_cnt = 0;
	self->bits_corrupted_cnt = 0;
	self->codewords_transferred_cnt = 0;
	self->codewords_corrupted_cnt = 0;

	if(self->ber == 0)
		self->bits_to_error = (int)1e9;
	else
		self->bits_to_error = (int)(exponential_distr(self->ber));
	if(self->bits_to_error <= 0) self->bits_to_error = 1;
	return 0;
}

int channel_bs_stop (channel_bs_t self)
{
	log(self->log, "\n");
	log(self->log, "... channel bs stopped\n");

	log(self->log, "... channel bs settings ...\n");
	
	if(self->channel_created_with_q)
		log(self->log, "Number of errors generated for each codeword: %d\n", self->errors_quantity);
	else
		log(self->log, "CBER: %f\n", self->ber);

	log(self->log, "... channel bs statistics ...\n");
	log(self->log, "bits transferred: %d\n", self->bits_transferred_cnt);
	log(self->log, "bits corrupted: %d (%.2f%%)\n", self->bits_corrupted_cnt, 
		(double)self->bits_corrupted_cnt/(double)self->bits_transferred_cnt*100);
	log(self->log, "codewords transferred: %d\n", self->codewords_transferred_cnt);
	log(self->log, "codewords corrupted: %d (%.2f%%)\n", self->codewords_corrupted_cnt,
		(double)self->codewords_corrupted_cnt/(double)self->codewords_transferred_cnt*100);

	return 0;
}

codeword_t channel_bs_transfer (channel_bs_t self, codeword_t codeword)
{
	codeword_t codeword_out = codeword_create(codeword->xsize);
	int word_corrupted = 0;

	if(self->channel_created_with_q)
	{
		codeword_out = codeword_cpy(codeword_out, codeword);
		for (int i = 0; i < self->errors_quantity; i++)
			codeword_out->xcodeword[i] = codeword_out->xcodeword[i] ? 0 : 1;
		self->bits_transferred_cnt += codeword->xsize;
		word_corrupted = 1;
	}
	else
	for (int i = 0; i < codeword->xsize; i++)
	{
		codeword_out->xcodeword[i] = codeword->xcodeword[i];
		self->bits_transferred_cnt++;
		self->bits_to_error--;

		if(self->bits_to_error == 0)
		{
			codeword_out->xcodeword[i] = invert_bit(codeword_out->xcodeword[i]);
			self->bits_corrupted_cnt++;

			do
				self->bits_to_error = (int)exponential_distr(self->ber);
			while (self->bits_to_error <= 0);
			word_corrupted = 1;
		}
	}

	if (word_corrupted)
	{
		self->codewords_corrupted_cnt++;
		log2(self->log, "channel bs: codeword corrupted\n");
		log2(self->log, "channel bs: original codeword: ");
		codeword_display(codeword);
		log2(self->log, "channel bs: corruptd codeword: ");
		codeword_display_diff_color(codeword, codeword_out, LIGHTRED, WHITE);
	}

	self->codewords_transferred_cnt++;
	return codeword_out;
}