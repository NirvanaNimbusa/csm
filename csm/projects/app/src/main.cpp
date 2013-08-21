//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: main.cpp
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
#include "argtable2.h"
#include "kernel.h"
#include <string.h>

int app_run(int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double channel_ber,
	int random_error_quantity,
	int erase_error_quantity,
	const char* file_postfix,
	int run_under_gui,
	int cnv_only,
	int bch_only,
	int channel_cfg,
	int verbose,
	const char **infiles, 
	int ninfiles)
{
	int i;

	for (i=0; i<ninfiles; i++)
	{
		if(cnv_only && bch_only)
		{
			printf("Incompatible options for starting the kernel, specify only one mode\n");
			return 1;
		}

		printf("Simulation of transfer \'%s\' data is started...\n", infiles[i]);

		if(cnv_only)
		{
			kernel_run(
				CNV_MODE,
				USE_BER_CHANNEL,
				galois_field_degree,
				bch_code_length,
				error_correction,
				decoded_seq_buf_size_frames,
				channel_ber,
				random_error_quantity,
				erase_error_quantity,
				const_cast<char*>(file_postfix),
				const_cast<char*>(infiles[i]),
				run_under_gui);
		}
		else if(bch_only)
		{
			if(channel_cfg)
			{
				kernel_run(
					BCH_MODE,
					USE_ERRQ_ERAQ_CHANNEL,
					galois_field_degree,
					bch_code_length,
					error_correction,
					decoded_seq_buf_size_frames,
					channel_ber,
					random_error_quantity,
					erase_error_quantity,
					const_cast<char*>(file_postfix),
					const_cast<char*>(infiles[i]),
					run_under_gui);
			}
			else
			{
				kernel_run(
					BCH_MODE,
					USE_BER_CHANNEL,
					galois_field_degree,
					bch_code_length,
					error_correction,
					decoded_seq_buf_size_frames,
					channel_ber,
					random_error_quantity,
					erase_error_quantity,
					const_cast<char*>(file_postfix),
					const_cast<char*>(infiles[i]),
					run_under_gui);
			}
		}
		else
		kernel_run(
			BASE_MODE,
			USE_BER_CHANNEL,
			galois_field_degree,
			bch_code_length,
			error_correction,
			decoded_seq_buf_size_frames,
			channel_ber,
			random_error_quantity,
			erase_error_quantity,
			const_cast<char*>(file_postfix),
			const_cast<char*>(infiles[i]),
			run_under_gui);
	}

	return 0;
}

// Returns filename portion of the given path
// Returns empty string if path is directory
char *get_file_name(char *path)
{
	char *filename = strrchr(path, '\\');
	if (filename == NULL)
		filename = path;
	else
		filename++;
	return filename;
}

int main(int argc, char **argv)
{
	struct arg_int *galois_field_degree  =
		arg_int0 ("g", "galois", NULL, 		"define Evariste Galois field degree (between 2..20, default is 4)");
	struct arg_int *bch_code_length  =
		arg_int0 ("l", "length",     NULL, 	"define BCH code length (default is 15)");
	struct arg_int *error_correction  =
		arg_int0 ("e", "errors",    NULL, 	"define BCH code error correction property (default is 3)");
	struct arg_int *decoded_seq_buf_size_frames  =
		arg_int0 ("d", "dbuffer", NULL, 	"define internal buffer size of trellis node in Viterbi Decoder (default is 2 frames)");
	struct arg_dbl *channel_ber = 
		arg_dbl0 ("b", "ber", NULL,  		"define channel Bit Error Rate (default is 1e-12)");
	struct arg_int *channel_req = 
		arg_int0 ("r", "req", NULL, 		"define channel channel Random Error Quantity to be generated (use without BER)");
	struct arg_int *channel_eeq = 
		arg_int0 ("i", "eeq", NULL, 		"define channel channel Erase Errors Quantity to be generated (use without BER)");
	struct arg_str *file_postfix = 
		arg_str0 ("p", "postfix", NULL,		"define out file postfix (default is \'transferred\')");
	struct arg_lit  *cnv_only = 
		arg_lit0 ("c", "cnvonly",           "use only Convolutional encoding and Viterbi decoding");
	struct arg_lit  *bch_only = 
		arg_lit0 ("x", "bchonly",           "use only BCH-encoding/decoding");
	struct arg_lit  *channel_cfg = 
		arg_lit0 ("j", "chcfg",				"configure channel (use with -r and -i)");
	struct arg_lit  *run_under_gui =
		arg_lit0 ("u", "gui",		        "run under GUI (additional GUI-application needed)");
	struct arg_lit  *verbose = 
		arg_lit0 ("v", "verbose,debug",     "verbose messages");
	struct arg_lit  *help    = 
		arg_lit0 ("h","help",               "print this help and exit");
	struct arg_lit  *version = 
		arg_lit0 (NULL,"version",           "print version information and exit");
	struct arg_file *infiles = 
		arg_filen(NULL, NULL,NULL,1,argc+2, "define input file(s)");
	struct arg_end  *end     = arg_end(20);
	void* argtable[] = {galois_field_degree,
		bch_code_length,
		error_correction,
		decoded_seq_buf_size_frames,
		channel_ber,
		channel_req,
		channel_eeq,
		file_postfix,
		infiles,
		cnv_only,
		bch_only,
		channel_cfg,
		run_under_gui,
		verbose,
		help,
		version,
		end};
	const char* progname = get_file_name(argv[0]);
	int nerrors;
	int exitcode=0;

	/* verify the argtable[] entries were allocated sucessfully */
	if (arg_nullcheck(argtable) != 0)
	{
		/* NULL entries were detected, some allocations must have failed */
		printf("%s: insufficient memory\n",progname);
		exitcode=1;
		goto exit;
	}

	/* set any command line default values prior to parsing */
	galois_field_degree->ival[0] = 4;
	bch_code_length->ival[0] = 15;
	error_correction->ival[0] = 3;
	decoded_seq_buf_size_frames->ival[0] = 2;
	channel_ber->dval[0] = 1e-12;
	file_postfix->sval[0] = "transferred";

	/* Parse the command line as defined by argtable[] */
	nerrors = arg_parse(argc,argv,argtable);

	/* special case: '--help' takes precedence over error reporting */
	if (help->count > 0)
	{
		printf("Usage: %s", progname);
		arg_print_syntax(stdout,argtable,"\n");
		printf("Program performs simulation of data transfer in communication system. ");
		printf("The system is structured in the following manner:\n");
		printf("[Transmitter]->[BCH encoder]->[Convolutional Encoder]->[Binary Symmetric Channel]->[Viterbi Decoder]->[BCH decoder]->[Receiver]\n");
		printf("The following options are available:\n");
		arg_print_glossary(stdout,argtable,"  %-25s %s\n");
		exitcode=0;
		goto exit;
	}

	/* special case: '--version' takes precedence error reporting */
	if (version->count > 0)
	{
		printf("%s - Communication System Modeler v.1.1.\n",progname);
		printf("August 2013, Pavel Morozkin (pavelmorozkin@yandex.ru)\n");
		exitcode=0;
		goto exit;
	}

	/* If the parser returned any errors then display them and exit */
	if (nerrors > 0)
	{
		/* Display the error details contained in the arg_end struct.*/
		arg_print_errors(stdout,end,progname);
		printf("Try '%s --help' for more information.\n",progname);
		exitcode=1;
		goto exit;
	}

	/* special case: uname with no command line options induces brief help */
	if (argc==1)
	{
		printf("Try '%s --help' for more information.\n",progname);
		exitcode=0;
		goto exit;
	}

	/* normal case: take the command line options at face value */
	exitcode = app_run(
		galois_field_degree->ival[0], 
		bch_code_length->ival[0],
		error_correction->ival[0],
		decoded_seq_buf_size_frames->ival[0],
		channel_ber->dval[0],
		channel_req->ival[0],
		channel_eeq->ival[0],
		file_postfix->sval[0],
		run_under_gui->count,
		cnv_only->count,
		bch_only->count,
		channel_cfg->count,
		verbose->count,
		infiles->filename,
		infiles->count);

exit:
	/* deallocate each non-null entry in argtable[] */
	arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

	return exitcode;
}
