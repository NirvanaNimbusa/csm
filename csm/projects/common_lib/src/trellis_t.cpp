//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: trellis_t.cpp
// Author: Pavel Morozkin
// Date: August 17th 2013
// Revised: August 17th 2013
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
#include "trellis_t.h"
#include "common_u.h"

#include <stdlib.h>
#include <stdio.h>

void trellis_setup(trellis_t self)
{
	/* trellis structure (states):
	*  00 ------- 00
	*      
	*  01         01
	*
	*  10         10
	*
	*  11 ------- 11
	*/

	/* level 0 */
	self->nodes_cluster_of_previous_time[0]->state_value[0] =
		self->nodes_cluster_of_current_time[0]->state_value[0] = 0;
	
	self->nodes_cluster_of_previous_time[0]->state_value[1] =
		self->nodes_cluster_of_current_time[0]->state_value[1] = 0;

	/* level 1 */
	self->nodes_cluster_of_previous_time[1]->state_value[0] =
		self->nodes_cluster_of_current_time[1]->state_value[0] = 0;

	self->nodes_cluster_of_previous_time[1]->state_value[1] =
		self->nodes_cluster_of_current_time[1]->state_value[1] = 1;

	/* level 2 */
	self->nodes_cluster_of_previous_time[2]->state_value[0] =
		self->nodes_cluster_of_current_time[2]->state_value[0] = 1;

	self->nodes_cluster_of_previous_time[2]->state_value[1] =
		self->nodes_cluster_of_current_time[2]->state_value[1] = 0;

	/* level 3 */
	self->nodes_cluster_of_previous_time[3]->state_value[0] =
		self->nodes_cluster_of_current_time[3]->state_value[0] = 1;

	self->nodes_cluster_of_previous_time[3]->state_value[1] =
		self->nodes_cluster_of_current_time[3]->state_value[1] = 1;

	/* trellis structure (edge values) [fisrt] -> [state value] <- [second]:
	*  00 -> 00 <- 11
	*      
	*  10 -> 01 <- 01
	*
	*  11 -> 10 <- 00
	*
	*  01 -> 11 <- 10
	*/
	/* level 0 */
	self->nodes_cluster_of_previous_time[0]->first_edge_value[0] =
		self->nodes_cluster_of_current_time[0]->first_edge_value[0] = 0;

	self->nodes_cluster_of_previous_time[0]->first_edge_value[1] =
		self->nodes_cluster_of_current_time[0]->first_edge_value[1] = 0;

	self->nodes_cluster_of_previous_time[0]->second_edge_value[0] =
		self->nodes_cluster_of_current_time[0]->second_edge_value[0] = 1;

	self->nodes_cluster_of_previous_time[0]->second_edge_value[1] =
		self->nodes_cluster_of_current_time[0]->second_edge_value[1] = 1;

	/* level 1 */
	self->nodes_cluster_of_previous_time[1]->first_edge_value[0] =
		self->nodes_cluster_of_current_time[1]->first_edge_value[0] = 1;

	self->nodes_cluster_of_previous_time[1]->first_edge_value[1] =
		self->nodes_cluster_of_current_time[1]->first_edge_value[1] = 0;

	self->nodes_cluster_of_previous_time[1]->second_edge_value[0] =
		self->nodes_cluster_of_current_time[1]->second_edge_value[0] = 0;

	self->nodes_cluster_of_previous_time[1]->second_edge_value[1] =
		self->nodes_cluster_of_current_time[1]->second_edge_value[1] = 1;

	/* level 2 */
	self->nodes_cluster_of_previous_time[2]->first_edge_value[0] =
		self->nodes_cluster_of_current_time[2]->first_edge_value[0] = 1;

	self->nodes_cluster_of_previous_time[2]->first_edge_value[1] =
		self->nodes_cluster_of_current_time[2]->first_edge_value[1] = 1;

	self->nodes_cluster_of_previous_time[2]->second_edge_value[0] =
		self->nodes_cluster_of_current_time[2]->second_edge_value[0] = 0;

	self->nodes_cluster_of_previous_time[2]->second_edge_value[1] =
		self->nodes_cluster_of_current_time[2]->second_edge_value[1] = 0;

	/* level 3 */
	self->nodes_cluster_of_previous_time[3]->first_edge_value[0] =
		self->nodes_cluster_of_current_time[3]->first_edge_value[0] = 0;

	self->nodes_cluster_of_previous_time[3]->first_edge_value[1] =
		self->nodes_cluster_of_current_time[3]->first_edge_value[1] = 1;

	self->nodes_cluster_of_previous_time[3]->second_edge_value[0] =
		self->nodes_cluster_of_current_time[3]->second_edge_value[0] = 1;

	self->nodes_cluster_of_previous_time[3]->second_edge_value[1] =
		self->nodes_cluster_of_current_time[3]->second_edge_value[1] = 0;

	/* init root */
	self->nodes_cluster_of_previous_time[0]->metric = 0;
}

void trellis_init (trellis_t const self)
{
	self->nodes_cluster_size = nodes_cluster_size_max;

	/* init clusters */
	self->nodes_cluster_of_previous_time = 
		(trellis_node_t*)malloc(sizeof(trellis_node_t*) * self->nodes_cluster_size);
	self->nodes_cluster_of_current_time = 
		(trellis_node_t*)malloc(sizeof(trellis_node_t*) * self->nodes_cluster_size);
	if(self->nodes_cluster_of_previous_time && self->nodes_cluster_of_current_time)
	{
		/* init of all trellis nodes */
		for (int i = 0; i < self->nodes_cluster_size; i++)
		{
			self->nodes_cluster_of_previous_time[i] = trellis_node_create(self->decoded_sequence_buffer_size);
			self->nodes_cluster_of_current_time[i] = trellis_node_create(self->decoded_sequence_buffer_size);
		}

		trellis_setup(self);
	}
}

void trellis_reset(trellis_t self)
{
	self->nodes_cluster_size = nodes_cluster_size_max;

	/* init of all trellis nodes */
	for (int i = 0; i < self->nodes_cluster_size; i++)
	{
		trellis_node_reset(self->nodes_cluster_of_previous_time[i]);
		trellis_node_reset(self->nodes_cluster_of_current_time[i]);
	}

	trellis_setup(self);
}


trellis_t trellis_create (int decoded_sequence_buffer_size)
{
	trellis_t self = (trellis_t)malloc(sizeof(trellis_base_t));
	if (!self) return NULL;

	self->decoded_sequence_buffer_size = decoded_sequence_buffer_size;

	trellis_init(self);
	return self;
}

void trellis_deinit (trellis_t self)
{
	for (int i = 0; i < self->nodes_cluster_size; i++)
	{
		trellis_node_destroy(self->nodes_cluster_of_previous_time[i]);
		trellis_node_destroy(self->nodes_cluster_of_current_time[i]);
	}

	free(self->nodes_cluster_of_previous_time);
	free(self->nodes_cluster_of_current_time);
	free(self);
}

void trellis_destroy (trellis_t self)
{
	return self ? trellis_deinit(self) : 0;
}

void trellis_print(trellis_t self)
{
	puts("dumping of trellis is started- ");
	puts("dumping of nodes cluster of previous time\n");
	for (int i = 0; i < self->nodes_cluster_size; i++)
	{
		printf_d("node number: %d\n", i);
		trellis_node_print(self->nodes_cluster_of_previous_time[i]);
		printf_d("\n");
	}
	puts("dumping of nodes cluster of current time\n");
	for (int i = 0; i < self->nodes_cluster_size; i++)
	{
		printf_d("node number: %d\n", i);
		trellis_node_print(self->nodes_cluster_of_current_time[i]);
		printf_d("\n");
	}
	puts("dumping of trellis is finished- ");
}
