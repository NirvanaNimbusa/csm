//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: kernel.cpp
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
#include "kernel.h"
#include <string.h>

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
	int gui_progress)
{
	switch (kernel_mode)
	{
	case BASE_MODE:
		kernel_run_base_mode(
			galois_field_degree,
			bch_code_length,
			error_correction,
			decoded_seq_buf_size_frames,
			ber,
			out_file_postfix,
			input_file_name,
			gui_progress);
		break;
	case CNV_MODE:
		kernel_run_cnv_mode(
			galois_field_degree,
			bch_code_length,
			error_correction,
			decoded_seq_buf_size_frames,
			ber,
			out_file_postfix,
			input_file_name,
			gui_progress);
		break;
	case BCH_MODE:
		switch (kernel_sub_mode)
		{
		case USE_BER_CHANNEL:
			kernel_run_bch_ber_ch_mode(
				galois_field_degree,
				bch_code_length,
				error_correction,
				decoded_seq_buf_size_frames,
				ber,
				out_file_postfix,
				input_file_name,
				gui_progress);
			break;
		case USE_ERRQ_ERAQ_CHANNEL:
			kernel_run_bch_eraq_ch_mode(
				galois_field_degree,
				bch_code_length,
				error_correction,
				decoded_seq_buf_size_frames,
				random_error_quantity,
				erase_error_quantity,
				out_file_postfix,
				input_file_name,
				gui_progress);
			break;
		default:
			break;
		}
	default:
		break;
	}
	return 0;
}