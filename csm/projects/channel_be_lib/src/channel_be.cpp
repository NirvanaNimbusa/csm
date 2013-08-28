//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: channel_be.cpp
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
#include "channel_be_t.h"
#include "my_math.h"
#include "common_u.h"
#include "utilities.h"

#include <stdlib.h>
#include <time.h>

channel_be_t channel_be_create(FILE* log, double ber)
{
	channel_be_t self = (channel_be_t)malloc(sizeof(channel_be_base_t));
	if(!self) return NULL;

	self->ber = ber;
	self->log = log;
	self->channel_created_with_q = 0;

	channel_be_init(self);
	return self;
}

channel_be_t channel_be_create_q(FILE* log, int errors_quantity, int erase_errors_quantity)
{
	channel_be_t self = (channel_be_t)malloc(sizeof(channel_be_base_t));
	if(!self) return NULL;

	self->errors_quantity = errors_quantity;
	self->erase_errors_quantity = erase_errors_quantity;
	self->log = log;
	self->channel_created_with_q = 1;

	channel_be_init(self);
	return self;
}

void channel_be_init (channel_be_t self)
{
	log2(self->log, "channel bs initialization started\n");
	self->start = channel_be_start;
	self->stop = channel_be_stop;
	self->transfer = channel_be_transfer;

	srand((int)time(NULL));
	if(self->ber == 0)
		self->bits_to_error = (int)1e9;
	else
		self->bits_to_error = (int)(exponential_distr(self->ber));
}

void channel_be_deinit (channel_be_t self)
{
	free(self);
}

void channel_be_destroy (channel_be_t self)
{
	if(!self) return;
	channel_be_deinit(self);
}

int channel_be_start (channel_be_t self)
{
	log2(self->log, "channel be started\n");
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

int channel_be_stop (channel_be_t self)
{
	log(self->log, "\n");
	log(self->log, "... channel be stopped\n");

	log(self->log, "... channel be settings ...\n");
	
	if(self->channel_created_with_q)
		log(self->log, "Number of errors generated for each codeword: %d\n", self->errors_quantity);
	else
		log(self->log, "CBER: %f\n", self->ber);

	log(self->log, "... channel be statistics ...\n");
	log(self->log, "bits transferred: %d\n", self->bits_transferred_cnt);
	log(self->log, "bits corrupted: %d (%.2f%%)\n", self->bits_corrupted_cnt, 
		(double)self->bits_corrupted_cnt/(double)self->bits_transferred_cnt*100);
	log(self->log, "codewords transferred: %d\n", self->codewords_transferred_cnt);
	log(self->log, "codewords corrupted: %d (%.2f%%)\n", self->codewords_corrupted_cnt,
		(double)self->codewords_corrupted_cnt/(double)self->codewords_transferred_cnt*100);
	return 0;
}

codeword_t channel_be_transfer (channel_be_t self, codeword_t codeword)
{
	codeword_t codeword_out = codeword_create(codeword->xsize);
	int word_corrupted = 0;

	if(self->channel_created_with_q)
	{
		/* Codeword format: [Random errors | Data | Erase errors]. */
		/* Copy entire codeword firstly. */
		codeword_out = codeword_cpy(codeword_out, codeword);
		/* Generate pseudo-random errors. */
		for (int i = 0; i < self->errors_quantity; i++)
		{
			codeword_out->xcodeword[i] = codeword_out->xcodeword[i] ? 0 : 1;
			self->bits_corrupted_cnt++;
		}
		/* Generate erase errors. */
		for (int i = codeword_out->xsize - self->erase_errors_quantity; i < codeword_out->xsize; i++)
		{
			codeword_out->xcodeword[i] = 0;
			self->bits_corrupted_cnt++;
		}
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
		log2(self->log, "channel be: codeword corrupted\n");
		log2(self->log, "channel be: original codeword: ");
		codeword_display(codeword);
		log2(self->log, "channel be: corrupted codeword: ");
		codeword_display_diff_color(codeword, codeword_out, LIGHTRED, WHITE);
	}

	self->codewords_transferred_cnt++;
	return codeword_out;
}

int channel_be_get_erase_errors_quantity(channel_be_t self)
{
	return self->erase_errors_quantity;
}