//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: kernel.h
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
#ifndef _KERNEL_
#define _KERNEL_

#include "common.h"
#include "test_engine.h"
#include "codec_bch.h"
#include "codec_cnv.h"
#include "channel_bs.h"
#include "channel_be.h"
#include "test_utilities.h"

#include <stdlib.h>
#include <limits.h>

enum kernel_mode_base;
typedef enum kernel_mode_base kernel_mode_t;

enum kernel_mode_base
{
	BASE_MODE,
	CNV_MODE,
	BCH_MODE
};

enum kernel_sub_mode_base;
typedef enum kernel_sub_mode_base kernel_sub_mode_t;

enum kernel_sub_mode_base
{
	USE_BER_CHANNEL,
	USE_ERRQ_ERAQ_CHANNEL
};

int kernel_run(
	kernel_mode_t kernel_mode,
	kernel_sub_mode_t  kernel_sub_mode,
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double ber,
	int random_error_quantity,
	int erase_error_quantity,
	char* out_file_postfix,
	char* input_file_name,
	int gui_progress);

int kernel_run_base_mode(
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double ber,
	char* out_file_postfix,
	char* input_file_name,
	int gui_progress);

int kernel_run_cnv_mode(
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double ber,
	char* out_file_postfix,
	char* input_file_name,
	int gui_progress);

int kernel_run_bch_ber_ch_mode(
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double ber,
	char* out_file_postfix,
	char* input_file_name,
	int gui_progress);

int kernel_run_bch_eraq_ch_mode(
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	int random_error_quantity,
	int erase_error_quantity,
	char* out_file_postfix,
	char* input_file_name,
	int gui_progress);

#endif