//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: trellis_node_t.h
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
#ifndef _TRELLIS_NODE_T_
#define _TRELLIS_NODE_T_

#include "frame_t.h"

typedef struct trellis_node trellis_node_base_t;
typedef trellis_node_base_t* trellis_node_t;

const int infinity_number = 999;

struct trellis_node {
	int metric;
	int* decoded_sequence_buffer;
	int decoded_sequence_buffer_size;
	int decoded_sequence_buffer_length;
	int* state_value;
	int* first_edge_value;
	int* second_edge_value;
};

void trellis_node_init (trellis_node_t const self);
trellis_node_t trellis_node_create (int decoded_sequence_buffer_size);
void trellis_node_deinit (trellis_node_t self);
void trellis_node_destroy (trellis_node_t self);

void trellis_node_print(trellis_node_t self);
void trellis_node_reset (trellis_node_t self);

#endif