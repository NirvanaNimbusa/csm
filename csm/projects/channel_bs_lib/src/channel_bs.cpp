//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: channel_bs.cpp
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
	log(self->log, "CBER: %f\n", self->ber);

	log(self->log, "... channel bs statistics ...\n");
	log(self->log, "bits transferred: %d\n", self->bits_transferred_cnt);
	log(self->log, "bits corrupted: %d (%.2f%%)\n", self->bits_corrupted_cnt, 
		(double)self->bits_corrupted_cnt/(double)self->bits_transferred_cnt*100);
	return 0;
}

int invert_bit(int bit)
{
	return bit ? 0 : 1;
}

codeword_t channel_bs_transfer (channel_bs_t self, codeword_t codeword)
{
	codeword_t codeword_out = codeword_create(codeword->xsize);
	int word_corrupted = 0;

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
		log2(self->log, "channel bs: codeword corrupted\n");
		log2(self->log, "channel bs: original codeword: ");
		codeword_display(codeword);
		log2(self->log, "channel bs: corruptd codeword: ");
		codeword_display_diff_color(codeword, codeword_out, LIGHTRED, WHITE);
	}
	return codeword_out;
}
























#ifdef NUSE

int* find_indexes_of_corrupted_bits(int len, int t)
{
#define randrange(N) rand() / (RAND_MAX/(N) + 1)

	int max = len - 1; /* Values will be in the range (1 .. MAX) */

	int* indexes_vektor = (int*)malloc(sizeof(int) * t);
	int* candidates = (int*)malloc(sizeof(int) * max);

	int i;
	srand((int)time(NULL));   /* Seed the random number generator. */

	for (i=0; i<max; i++)
		candidates[i] = i;

	for (i = 0; i < max-1; i++) {
		int c = randrange(max-i);
		int t = candidates[i];
		candidates[i] = candidates[i+c];
		candidates[i+c] = t;
	}

	for (i=0; i<t; i++)
		indexes_vektor[i] = candidates[i] + 1;

	//for (i=0; i<t; i++)	printf_d("%i\n", indexes_vektor[i]);

	free(candidates);
	return indexes_vektor;
}

codeword_t corrupt_codeword_base (codeword_t codeword, int* indexes_of_corrupted_bits, int t)
{
	for (int i = 0; i < t; i++)
	{
		int index = indexes_of_corrupted_bits[i];
		codeword->xcodeword[index] = invert_bit(codeword->xcodeword[index]);
	}
	return codeword;
}

codeword_t corrupt_codeword (codeword_t codeword, double ber, int t)
{
	log("corrupt code word start");
#ifdef USE_PRINTF
	printf_d("current settings: ber: %f, t: %d\n", ber, t);
	printf_d("original codeword:\n");
#endif
	//codeword_display(codeword);

	int len = codeword->xsize;
	int* indexes_of_corrupted_bits = find_indexes_of_corrupted_bits(len, t);
	codeword_t corrupt_codeword = codeword_create(32);

	corrupt_codeword = codeword_cpy(corrupt_codeword, codeword);
	corrupt_codeword = corrupt_codeword_base(corrupt_codeword, indexes_of_corrupted_bits, t);

	free(indexes_of_corrupted_bits);
#ifdef USE_PRINTF
	printf_d("corrupted codeword:\n");
#endif
	//codeword_display_diff_color(codeword, corrupt_codeword, LIGHTRED, LIGHTGREY);
	codeword_destroy(codeword);

	log("corrupt code word finish");
	return corrupt_codeword;
}



#endif