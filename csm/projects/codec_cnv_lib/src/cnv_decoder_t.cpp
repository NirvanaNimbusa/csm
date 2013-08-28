//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: cnv_decoder_t.cpp
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
#include "cnv_decoder_t.h"

#include <stdlib.h>
#include <stdio.h>

cnv_decoder_t cnv_decoder_create(FILE* log, int regs_q, int decoded_sequence_buffer_size)
{
	cnv_decoder_t self = (cnv_decoder_t)malloc(sizeof(cnv_decoder_base_t));
	if(!self) return NULL;

	self->log = log;
	self->decoded_sequence_buffer_size = decoded_sequence_buffer_size;
	self->regs_q = regs_q;

	cnv_decoder_init(self);
	return self;
}

void cnv_decoder_init (cnv_decoder_t self)
{
	log2(self->log, "cnv decoder initialization started\n");
	self->start = cnv_decoder_start;
	self->stop = cnv_decoder_stop;
	self->decode = cnv_decoder_decode;

	self->trellis = trellis_create(self->decoded_sequence_buffer_size);
	//trellis_print(self->trellis);
	self->skip_q = 0;
}

void cnv_decoder_deinit (cnv_decoder_t self)
{
	trellis_destroy(self->trellis);
	free(self);
}

void cnv_decoder_destroy (cnv_decoder_t self)
{
	if(!self) return;
	cnv_decoder_deinit(self);
}

int cnv_decoder_start(cnv_decoder_t self)
{
	log2(self->log, "cnv decoder started\n");
	self->received_bits_cnt = 0;
	self->received_bits_valid_cnt = 0;
	self->received_bits_corrupted_cnt = 0;
	self->received_codewords_cnt = 0;
	self->received_codewords_valid_cnt = 0;
	self->received_codewords_corrupted_cnt = 0;
	self->generated_frames_cnt = 0;
	return 0;
}

int cnv_decoder_stop(cnv_decoder_t self)
{
	log(self->log, "\n");
	log(self->log, "... cnv decoder stopped\n");
	log(self->log, "... cnv decoder settings ...\n");
	log(self->log, "decoded sequence buffer size: %d\n", self->decoded_sequence_buffer_size);

	log(self->log, "... cnv decoder statistics ...\n");
	log(self->log, "delayed frames: %d\n", self->skip_q);

	log(self->log, "received bits: %d\n", self->received_bits_cnt);
	log(self->log, "received bits (valid): %d\n", self->received_bits_valid_cnt);
	log(self->log, "received bits (corrupted): %d\n", self->received_bits_corrupted_cnt);

	log(self->log, "received codewords: %d\n", self->received_codewords_cnt);
	log(self->log, "received codewords (valid): %d\n", self->received_codewords_valid_cnt);
	log(self->log, "received codewords (corrupted): %d\n", self->received_codewords_corrupted_cnt);

	log(self->log, "generated frames: %d\n", self->generated_frames_cnt);

	trellis_reset(self->trellis);
	return 0;
}

int* get_next_2b_from_codeword(codeword_t codeword, int start_index)
{
	if (start_index >= codeword->xsize)
		return NULL;

	int* two_b = (int*)malloc(sizeof(int) * 2);
	if(two_b)
	{
		two_b[0] = codeword->xcodeword[start_index];
		two_b[1] = codeword->xcodeword[start_index+1];
		return two_b;
	}
	return NULL;
}

void print_2b(int* int_2b)
{
	for (int i = 0; i < 2; i++)
		printf_d("%d", int_2b[i]);
	printf_d("\n");
}

int cmp_2b(int* int_2b, char* char_2b)
{
	int f = char_2b[0] == '1' ? 1 : 0;
	int s = char_2b[1] == '1' ? 1 : 0;

	if (int_2b[0] == f && int_2b[1] == s)
		return 1;
	return 0;
}

parents_t compute_parents_of_node (trellis_node_t* nodes_cluster_of_previous_time,
	trellis_node_t trellis_node)
{
	parents_t parents = {0, 0};

	int* state_value = trellis_node->state_value;

	if (cmp_2b(state_value, "00") || cmp_2b(state_value, "10"))
	{
		parents.first = nodes_cluster_of_previous_time[0];
		parents.second = nodes_cluster_of_previous_time[1];
	} 
	else if (cmp_2b(state_value, "01") || cmp_2b(state_value, "11"))
	{
		parents.first = nodes_cluster_of_previous_time[2];
		parents.second = nodes_cluster_of_previous_time[3];
	} 
	else
		xexit("compute_parents_of_node: error\n");

	return parents;
}

int compute_hamming_distance (int* node_edge, int* channel_2b)
{
	int hamming_distance = 0;

	for (int i = 0; i < 2; ++i)
		hamming_distance += (node_edge[i] ^ channel_2b[i]);   // could also try != in place of ^

	return hamming_distance;
}

trellis_node_t fill_trellis_node
	(trellis_node_t trellis_node_dest, trellis_node_t trellis_node_src, int decode_symbol, int metric)
{
	trellis_node_dest->metric = metric;
	trellis_node_dest->decoded_sequence_buffer_size = trellis_node_src->decoded_sequence_buffer_size;
	trellis_node_dest->decoded_sequence_buffer_length = trellis_node_src->decoded_sequence_buffer_length;

	for (int i = 0; i < trellis_node_dest->decoded_sequence_buffer_size; i++)
		trellis_node_dest->decoded_sequence_buffer[i] = trellis_node_src->decoded_sequence_buffer[i];

	int index = trellis_node_dest->decoded_sequence_buffer_length;
	trellis_node_dest->decoded_sequence_buffer[index] = decode_symbol;
	trellis_node_dest->decoded_sequence_buffer_length++;

	return trellis_node_dest;
}

trellis_node_t* copy_nodes_cluster_data (trellis_t trellis, trellis_node_t* dest, trellis_node_t* src)
{
	for (int node_index = 0; node_index < trellis->nodes_cluster_size; node_index++)
	{
		for (int buffer_index = 0; buffer_index < dest[node_index]->decoded_sequence_buffer_size; buffer_index++)
		{
			dest[node_index]->decoded_sequence_buffer[buffer_index] = 
				src[node_index]->decoded_sequence_buffer[buffer_index];
		}
		dest[node_index]->decoded_sequence_buffer_length = src[node_index]->decoded_sequence_buffer_length;
		dest[node_index]->decoded_sequence_buffer_size = src[node_index]->decoded_sequence_buffer_size;
		dest[node_index]->metric = src[node_index]->metric;
	}
	return dest;
}

void shift_buffers (trellis_node_t* nodes_cluster)
{
	for (int i = 0; i < 4; i++)
	{
		int j = 0;
		for (j = 0; j < nodes_cluster[0]->decoded_sequence_buffer_size - 1; j++)
		{
			nodes_cluster[i]->decoded_sequence_buffer[j] = nodes_cluster[i]->decoded_sequence_buffer[j+1];
		}
		/* add last symbol */
		nodes_cluster[i]->decoded_sequence_buffer[j] = 0;
		nodes_cluster[i]->decoded_sequence_buffer_length--;
	}
}

int get_user_symbol (trellis_node_t* nodes_cluster)
{
	int user_symbol = 0;
	if (nodes_cluster[0]->decoded_sequence_buffer_length != 
		nodes_cluster[0]->decoded_sequence_buffer_size)
		return -1;

	/* buffer is full, get user symbol */
	/* find min metric */
	int node_index_min_metric = 0;
	int min_metric = 10000;

	for (int i = 0; i < 4; i++)
	{
		if(nodes_cluster[i]->metric <= min_metric)
		{
			min_metric = nodes_cluster[i]->metric;
			node_index_min_metric = i;
		}
	}

	/* get first symbol */    
	user_symbol = nodes_cluster[node_index_min_metric]->decoded_sequence_buffer[0];
	shift_buffers(nodes_cluster);
	return user_symbol;
}

int find_decode_symbol (trellis_node_t trellis_node)
{
	int* state_value = trellis_node->state_value;
	if (cmp_2b(state_value, "00") || cmp_2b(state_value, "01"))
		return 0;
	return 1;
}

frame_t cnv_decoder_decode (cnv_decoder_t self, codeword_t codeword_out, codeword_t codeword_in)
{
	const int frame_size = codeword_out->xsize / self->regs_q;
	frame_t frame = frame_create(frame_size);
	//codeword_display(codeword_in);
	trellis_t trellis = self->trellis;

	self->received_bits_cnt += codeword_out->xsize;
	self->received_codewords_cnt++;
	self->skip_q = self->decoded_sequence_buffer_size / (codeword_in->xsize / 2);

	if(codeword_cmp(codeword_out, codeword_in))
	{
		self->received_bits_valid_cnt += codeword_out->xsize;
		self->received_codewords_valid_cnt++;
	}
	else
	{
		int invalid_bits_q = codeword_find_invalid_bits(codeword_out, codeword_in);
		self->received_bits_corrupted_cnt += invalid_bits_q;
		self->received_codewords_corrupted_cnt++;
	}

	// decode codeword
	for (int index_2b = 0; index_2b < codeword_out->xsize; index_2b+=2)
	{
		int* channel_2b = get_next_2b_from_codeword(codeword_out, index_2b);
		//print_2b(channel_2b);
		for (int node_index = 0; node_index < trellis->nodes_cluster_size; node_index++)
		{
			// get node
			trellis_node_t trellis_node = trellis->nodes_cluster_of_current_time[node_index];

			// get node cluster
			trellis_node_t* nodes_cluster_of_previous_time = trellis->nodes_cluster_of_previous_time;

			// compute parents of this node
			parents_t parents = compute_parents_of_node(nodes_cluster_of_previous_time, trellis_node);

			trellis_node_t first_parent = parents.first;
			trellis_node_t second_parent = parents.second;

			//trellis_node_print(first_parent);
			//trellis_node_print(second_parent);

			// find hamming dist between 2b and value on edges of parents
			int hamming_distance1 = compute_hamming_distance(trellis_node->first_edge_value, channel_2b);
			int hamming_distance2 = compute_hamming_distance(trellis_node->second_edge_value, channel_2b);

			int first_parent_metric = parents.first->metric;
			int second_parent_metric = parents.second->metric;

			first_parent_metric += hamming_distance1;
			second_parent_metric += hamming_distance2;

			int decode_symbol = find_decode_symbol(trellis_node);

			if (first_parent_metric < second_parent_metric)
				trellis_node = fill_trellis_node(trellis_node, first_parent, decode_symbol, first_parent_metric);
			else if (first_parent_metric > second_parent_metric)
				trellis_node = fill_trellis_node(trellis_node, second_parent, decode_symbol, second_parent_metric);
			else 
				trellis_node = fill_trellis_node(trellis_node, second_parent, decode_symbol, second_parent_metric);

			//trellis_node_print(trellis_node);
		}

		trellis_node_t* nodes_cluster_of_current_time = trellis->nodes_cluster_of_current_time;
		trellis_node_t* nodes_cluster_of_previous_time = trellis->nodes_cluster_of_previous_time;

		//trellis_print(trellis);

		//printf_d("metrics:\n");
		//for (int i = 0; i < 4; i++)	printf_d("%d\n", trellis->nodes_cluster_of_current_time[i]->metric);

		// copy data from current cluster to previous cluster
		nodes_cluster_of_previous_time = 
			copy_nodes_cluster_data(trellis, nodes_cluster_of_previous_time, nodes_cluster_of_current_time);

		// compute user symbol
		int user_symbol = get_user_symbol(nodes_cluster_of_previous_time);

		if (user_symbol == -1)
			frame->xframe[index_2b/2] = 1; /* cnv_decoder unable to send user symbol now */
		else
			frame->xframe[index_2b/2] = user_symbol;

		free(channel_2b);
	}

	self->generated_frames_cnt++;

	codeword_destroy(codeword_out);
	codeword_destroy(codeword_in);

	return frame;
}

