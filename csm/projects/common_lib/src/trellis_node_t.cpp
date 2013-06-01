//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: trellis_node_t.cpp
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
#include "trellis_node_t.h"
#include "common_u.h"

#include <stdlib.h>
#include <stdio.h>

void trellis_node_init (trellis_node_t const self)
{
	self->metric = infinity_number;
	self->decoded_sequence_buffer_length = 0;
	self->decoded_sequence_buffer = NULL;

	self->decoded_sequence_buffer = (int*)calloc(self->decoded_sequence_buffer_size, sizeof(int));
	self->state_value = (int*)calloc(2, sizeof(int));
	self->first_edge_value = (int*)calloc(2, sizeof(int));
	self->second_edge_value = (int*)calloc(2, sizeof(int));
}

void trellis_node_reset (trellis_node_t self)
{
	self->metric = infinity_number;
	self->decoded_sequence_buffer_length = 0;

	int i;
	for (i = 0; i < self->decoded_sequence_buffer_size; i++) 
		self->decoded_sequence_buffer[i] = 0;
	for (i = 0; i < 2; i++) 
	{
		self->state_value[i] = 0;
		self->first_edge_value[i] = 0;
		self->second_edge_value[i] = 0;
	}
}

trellis_node_t trellis_node_create (int decoded_sequence_buffer_size)
{
	trellis_node_t self = (trellis_node_t)malloc(sizeof(trellis_node_base_t));
	if(!self) return NULL;

	self->decoded_sequence_buffer_size = decoded_sequence_buffer_size;

	trellis_node_init(self);
	return self;	
}

void trellis_node_deinit (trellis_node_t self)
{
	free(self->state_value);
	free(self->first_edge_value);
	free(self->second_edge_value);
	free(self->decoded_sequence_buffer);
	free(self);
}

void trellis_node_destroy (trellis_node_t self)
{
	if (!self) return;
	trellis_node_deinit(self);
}

void trellis_node_print(trellis_node_t self)
{
	printf_d("dumping of trellis node is started...\n");
	printf_d("- metric: %d\n", self->metric);
	printf_d("- decoded sequence buffer size: %d\n", self->decoded_sequence_buffer_size);
	printf_d("- decoded sequence buffer length: %d\n", self->decoded_sequence_buffer_length);

	int i;
	printf_d("- decoded sequence: ");
	for (i = 0; i < self->decoded_sequence_buffer_size; i++) 
		printf_d("%d", self->decoded_sequence_buffer[i]);
	printf_d("\n");
	printf_d("- state value: ");
	for (i = 0; i < 2; i++) 
		printf_d("%d", self->state_value[i]);
	printf_d("\n");
	printf_d("- first edge value: ");
	for (i = 0; i < 2; i++) 
		printf_d("%d", self->first_edge_value[i]);
	printf_d("\n");
	printf_d("- second edge value: ");
	for (i = 0; i < 2; i++) 
		printf_d("%d", self->second_edge_value[i]);
	printf_d("\n");
	printf_d("dumping of trellis node is finished...\n");
}