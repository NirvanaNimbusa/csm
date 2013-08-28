//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: trellis_t.h
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
#ifndef _TRELLIS_T_
#define _TRELLIS_T_

#include "trellis_node_t.h"

typedef struct trellis trellis_base_t;
typedef trellis_base_t* trellis_t;

const int nodes_cluster_size_max = 4;

struct trellis {
	trellis_node_t* nodes_cluster_of_previous_time;
	trellis_node_t* nodes_cluster_of_current_time;
	int				nodes_cluster_size;
	int decoded_sequence_buffer_size;
};

void trellis_init (trellis_t const self);
trellis_t trellis_create (int decoded_sequence_buffer_size);
void trellis_deinit (trellis_t self);
void trellis_destroy (trellis_t self);

void trellis_print(trellis_t self);
void trellis_reset(trellis_t self);

#endif