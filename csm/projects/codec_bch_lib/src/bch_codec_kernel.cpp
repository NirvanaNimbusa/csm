//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: bch_codec_kernel.cpp
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
#include "bch_codec_kernel.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**************************************************************************/
/* ВАЖНАЯ ИНФОРМАЦИЯ: Данное ядро кодека БЧХ-кода основано на реализиции  */
/* аналогичного кодека, выполненной автором Robert Morelos-Zaragoza.      */
/* Для сохранения авторских прав на реализацию алгоритмов ниже приводится */
/* оригинальный текст к исходному коду.                                   */
/**************************************************************************/

/*
 * File:    bch3.c
 * Title:   Encoder/decoder for binary BCH codes in C (Version 3.1)
 * Author:  Robert Morelos-Zaragoza
 * Date:    August 1994
 * Revised: June 13, 1997
 *
 * ===============  Encoder/Decoder for binary BCH codes in C =================
 *
 * Version 1:   Original program. The user provides the generator polynomial
 *              of the code (cumbersome!).
 * Version 2:   Computes the generator polynomial of the code.
 * Version 3:   No need to input the coefficients of a primitive polynomial of
 *              degree m, used to construct the Galois Field GF(2**m). The
 *              program now works for any binary BCH code of length such that:
 *              2**(m-1) - 1 < length <= 2**m - 1
 *
 * Note:        You may have to change the size of the arrays to make it work.
 *
 * The encoding and decoding methods used in this program are based on the
 * book "Error Control Coding: Fundamentals and Applications", by Lin and
 * Costello, Prentice Hall, 1983.
 *
 * Thanks to Patrick Boyle (pboyle@era.com) for his observation that 'bch2.c'
 * did not work for lengths other than 2**m-1 which led to this new version.
 * Portions of this program are from 'rs.c', a Reed-Solomon encoder/decoder
 * in C, written by Simon Rockliff (simon@augean.ua.oz.au) on 21/9/89. The
 * previous version of the BCH encoder/decoder in C, 'bch2.c', was written by
 * Robert Morelos-Zaragoza (robert@spectra.eng.hawaii.edu) on 5/19/92.
 *
 * NOTE:    
 *          The author is not responsible for any malfunctioning of
 *          this program, nor for any damage caused by it. Please include the
 *          original program along with these comments in any redistribution.
 *
 *  For more information, suggestions, or other ideas on implementing error
 *  correcting codes, please contact me at:
 *
 *                           Robert Morelos-Zaragoza
 *                           5120 Woodway, Suite 7036
 *                           Houston, Texas 77056
 *
 *                    email: r.morelos-zaragoza@ieee.org
 *
 * COPYRIGHT NOTICE: This computer program is free for non-commercial purposes.
 * You may implement this program for any non-commercial application. You may 
 * also implement this program for commercial purposes, provided that you
 * obtain my written permission. Any modification of this program is covered
 * by this copyright.
 *
 * == Copyright (c) 1994-7,  Robert Morelos-Zaragoza. All rights reserved.  ==
 *
 * m = order of the Galois field GF(2**m) 
 * n = 2**m - 1 = size of the multiplicative group of GF(2**m)
 * length = length of the BCH code
 * t = error correcting capability (max. no. of errors the code corrects)
 * d = 2*t + 1 = designed min. distance = no. of consecutive roots of g(x) + 1
 * k = n - deg(g(x)) = dimension (no. of information bits/codeword) of the code
 * p[] = coefficients of a primitive polynomial used to generate GF(2**m)
 * g[] = coefficients of the generator polynomial, g(x)
 * alpha_to [] = log table of GF(2**m) 
 * index_of[] = antilog table of GF(2**m)
 * data[] = information bits = coefficients of data polynomial, i(x)
 * bb[] = coefficients of redundancy polynomial x^(length-k) i(x) modulo g(x)
 * numerr = number of errors 
 * errpos[] = error positions 
 * recd[] = coefficients of the received polynomial 
 * decerror = number of decoding errors (in _message_ positions) 
 *
 */

bch_codec_kernel_vars_t bch_codec_kernel_vars_create ()
{
	return (bch_codec_kernel_vars_t)malloc(sizeof(bch_codec_kernel_vars_base_t));
}

void bch_codec_kernel_vars_destroy (bch_codec_kernel_vars_t self)
{
	if(self)
		free(self);
}

bch_codec_kernel_vars_t
check_and_set_code_params
(bch_codec_kernel_vars_t vars, int galois_field_degree, int code_length, int error_correction)
{
	int code_length_max;
	int code_length_min;

	/*  Проверка корректности длины кодового слова и степени поля Галуа. */
	if(galois_field_degree > 20)
		xexit("galois field degree threshold reached\n");

	code_length_max = (int)pow((double)2, galois_field_degree) - 1;
	code_length_min = (code_length_max + 1) / 2 - 1;

	if(!(code_length > code_length_min && code_length <= code_length_max))
		xexit("code length threshold reached\n");

	vars->code_length_max = code_length_max;
	vars->code_length = code_length;
	vars->galois_field_degree = galois_field_degree;

	vars->error_correction = error_correction;
	if(error_correction <= 0)
		xexit("code correction threshold reached\n");

	vars->code_distance = 2 * vars->error_correction + 1;

	return vars;
}

/*
 * Назначение:
 * 1. Проверка корректности длины кодового слова и степени поля Галуа;
 * 2. Вычисление неприводимого полинома над полем Галуа GF(2^galois_field_degree);
 * 3. Коприрование вычисленных значений в переменные ядра БЧХ-кодера.
 * 
 * Аргументы:
 * 1. vars - переменные ядра БЧХ-кодера;
 * 
 * Возвращаемое значение:
 * Переменные ядра БЧХ-кодера.
 */
bch_codec_kernel_vars_t generate_primitive_polynom(bch_codec_kernel_vars_t vars)
{
	int	i;
	int primitive_polynomial[21];	/* Неприводимый полином. */
	int galois_field_degree = vars->galois_field_degree;
	
	/* Установка начальных коэффициентов неприводимого полинома. */
	primitive_polynomial[0] = primitive_polynomial[galois_field_degree] = 1;
	for (i = 1; i < galois_field_degree; i++)
		primitive_polynomial[i] = 0;
	
	/* Вычисление неприводимого полинома над полем Галуа GF(2^galois_field_degree). */
	switch (galois_field_degree)
	{
	case 2:
		primitive_polynomial[1] = 1;
		break;
	case 3:
		primitive_polynomial[1] = 1;
		break;
	case 4:
		primitive_polynomial[1] = 1;
		break;
	case 5:
		primitive_polynomial[2] = 1;
		break;
	case 6:
		primitive_polynomial[1] = 1;
		break;	
	case 7:
		primitive_polynomial[1] = 1;
		break;	
	case 8:
		primitive_polynomial[4] = primitive_polynomial[5] = primitive_polynomial[6] = 1;
		break;	
	case 9:
		primitive_polynomial[4] = 1;
		break;	
	case 10:
		primitive_polynomial[3] = 1;
		break;	
	case 11:
		primitive_polynomial[2] = 1;
		break;
	case 12:
		primitive_polynomial[3] = primitive_polynomial[4] = primitive_polynomial[7] = 1;
		break;
	case 13:
		primitive_polynomial[1] = primitive_polynomial[3] = primitive_polynomial[4] = 1;
		break;
	case 14:
		primitive_polynomial[1] = primitive_polynomial[11] = primitive_polynomial[12] = 1;
		break;
	case 15:
		primitive_polynomial[1] = 1;
		break;	
	case 16:
		primitive_polynomial[2] = primitive_polynomial[3] = primitive_polynomial[5] = 1;
		break;	
	case 17:
		primitive_polynomial[3] = 1;
		break;	
	case 18:
		primitive_polynomial[7] = 1;
		break;	
	case 19:
		primitive_polynomial[1] = primitive_polynomial[5] = primitive_polynomial[6] = 1;
		break;		
	case 20:
		primitive_polynomial[3] = 1;
		break;		
	}

	printf_d("primitive polynomial: p(x) = ");
	for (i = 0; i <= galois_field_degree; i++) 
		printf_d("%1d", primitive_polynomial[i]);
	printf_d("\n");

	/* Коприрование вычисленных значений в переменные ядра БЧХ-кодера. */
	for (i = 0; i <= galois_field_degree; i++) 
		vars->primitive_polynomial[i] = primitive_polynomial[i];
	return vars;
}

/*
 * Назначение:
 * Генерация поля Галуа по примитивному полиному.
 * 
 * Генерируемые массивы:
 * Индексная форма -> полиномиальная форма: Lookup table: alpha_to[], содержит j = alpha ^ i;
 * Полиномиальная форма -> индексная форма: Lookup table: index_of[j=alpha^i] = i;
 * alpha=2 is the primitive element of GF(2**galois_field_degree) 
 * 
 * Аргументы:
 * vars - переменные ядра БЧХ-кодера;
 * 
 * Возвращаемое значение:
 * Переменные ядра БЧХ-кодера.
 */
bch_codec_kernel_vars_t generate_evariste_galois_field(bch_codec_kernel_vars_t vars)
{
	//log("Evariste Galois field generation started");
	int i, mask;

	mask = 1;
	vars->alpha_to[vars->galois_field_degree] = 0;
	for (i = 0; i < vars->galois_field_degree; i++) 
	{
		vars->alpha_to[i] = mask;
		vars->index_of[vars->alpha_to[i]] = i;
		if (vars->primitive_polynomial[i] != 0)
			vars->alpha_to[vars->galois_field_degree] ^= mask;
		mask <<= 1;
	}
	
	i = vars->alpha_to[vars->galois_field_degree];
	vars->index_of[i] = vars->galois_field_degree;
	mask >>= 1;
	
	for (i = vars->galois_field_degree + 1; i < vars->code_length_max; i++)
	{
		if (vars->alpha_to[i - 1] >= mask)
		{
			int a = vars->alpha_to[vars->galois_field_degree];
			int b = (vars->alpha_to[i - 1] ^ mask) << 1;
			vars->alpha_to[i] = a ^ b;
		}
		else
			vars->alpha_to[i] = vars->alpha_to[i - 1] << 1;
		
		int ii = vars->alpha_to[i];
		vars->index_of[ii] = i;
	}
	
	vars->index_of[0] = -1;
	//log("Evariste Galois field generation finished");
	return vars;
}

/*
 * Назначение:
 * Генерация порождающего полинома.
 * 
 * Аргументы:
 * vars - переменные ядра БЧХ-кодера;
 * 
 * Возвращаемое значение:
 * Переменные ядра БЧХ-кодера.
 */
/*
 * Compute the generator polynomial of a binary BCH encode. Fist generate the
 * cycle sets modulo 2**galois_field_degree - 1, cycle[][] =  (i, 2*i, 4*i, - , 2^l*i). Then
 * determine those cycle sets that contain integers in the set of (code_distance-1)
 * consecutive integers {1..(code_distance-1)}. The generator polynomial is calculated
 * as the product of linear factors of the form (x+alpha^i), for every i in
 * the above cycle sets.
 */
bch_codec_kernel_vars_t generate_generator_polynom(bch_codec_kernel_vars_t vars)
{
	register int	ii, jj, ll, kaux;
	register int	test, aux, nocycles, root, noterms, rdncy_symbols_q;
	static int             cycle[1024][21], size[1024], min[1024], zeros[1024];

	/* Generate cycle sets modulo code_length_max, code_length_max = 2**galois_field_degree - 1 */
	cycle[0][0] = 0;
	size[0] = 1;
	cycle[1][0] = 1;
	size[1] = 1;
	jj = 1;			/* cycle set index */

	do {
		/* Generate the jj-th cycle set */
		ii = 0;
		do 
		{
			ii++;
			cycle[jj][ii] = (cycle[jj][ii - 1] * 2) % vars->code_length_max;
			size[jj]++;
			aux = (cycle[jj][ii] * 2) % vars->code_length_max;
		} while (aux != cycle[jj][0]);
		
		/* Next cycle set representative */
		ll = 0;
		do 
		{
			ll++;
			test = 0;
			for (ii = 1; ((ii <= jj) && (!test)); ii++)
			{
			/* Examine previous cycle sets */
			  for (kaux = 0; ((kaux < size[ii]) && (!test)); kaux++)
			  {
				  if (ll == cycle[ii][kaux])
			        test = 1;
			  }
			}
		} while ((test) && (ll < (vars->code_length_max - 1)));
		if (!(test))
		{
			jj++;	/* next cycle set index */
			cycle[jj][0] = ll;
			size[jj] = 1;
		}
	} while (ll < (vars->code_length_max - 1));
	
	nocycles = jj;		/* number of cycle sets modulo code_length_max */

	/* Search for roots 1, 2, - , code_distance-1 in cycle sets */
	kaux = 0;
	rdncy_symbols_q = 0;
	for (ii = 1; ii <= nocycles; ii++) 
	{
		min[kaux] = 0;
		test = 0;
		for (jj = 0; ((jj < size[ii]) && (!test)); jj++)
		{
			for (root = 1; ((root < vars->code_distance) && (!test)); root++)
			{
				if (root == cycle[ii][jj])
				{
					test = 1;
					min[kaux] = ii;
				}
			}
		}
		
		if (min[kaux]) 
		{
			rdncy_symbols_q += size[min[kaux]];
			kaux++;
		}
	}

	noterms = kaux;
	kaux = 1;
	
	for (ii = 0; ii < noterms; ii++)
	{
		for (jj = 0; jj < size[min[ii]]; jj++) 
		{
			zeros[kaux] = cycle[min[ii]][jj];
			kaux++;
		}
	}

	vars->inf_symbols_q = vars->code_length - rdncy_symbols_q;

    if (vars->inf_symbols_q < 0)
		xexit("parameters corrupted\n");

	/* Compute the generator polynomial */
	vars->generator_polynomial[0] = vars->alpha_to[zeros[1]];
	vars->generator_polynomial[1] = 1;		/* generator_polynomial(x) = (X + zeros[1]) initially */
	
	for (ii = 2; ii <= rdncy_symbols_q; ii++) 
	{
		vars->generator_polynomial[ii] = 1;
		for (jj = ii - 1; jj > 0; jj--)
		{
			if (vars->generator_polynomial[jj] != 0)
			vars->generator_polynomial[jj] = vars->generator_polynomial[jj - 1] ^
				vars->alpha_to[(vars->index_of[vars->generator_polynomial[jj]] + 
				zeros[ii]) % vars->code_length_max];
			else
			vars->generator_polynomial[jj] = vars->generator_polynomial[jj - 1];
		}
		
		vars->generator_polynomial[0] = vars->alpha_to[(vars->index_of[vars->generator_polynomial[0]] + 
			zeros[ii]) % vars->code_length_max];
	}

	printf_d("generator polynomial: g(x) = ");
	for (ii = 0; ii <= rdncy_symbols_q; ii++) 
	{
		printf_d("%d", vars->generator_polynomial[ii]);
		if (ii && ((ii % 50) == 0))
			printf_d("\n");
	}
	printf_d("\n");
	return vars;
}

/*
 * Compute redundacy bb[], the coefficients of b(x). The redundancy
 * polynomial b(x) is the remainder after dividing x^(code_length-inf_symbols_q)*data(x)
 * by the generator polynomial generator_polynomial(x).
 */
void encode_bch(bch_codec_kernel_vars_t vars)
{
	register int    i, j;
	register int    feedback;

	for (i = 0; i < vars->code_length - vars->inf_symbols_q; i++)
		vars->bb[i] = 0;
	for (i = vars->inf_symbols_q - 1; i >= 0; i--) {
		feedback = vars->data[i] ^ vars->bb[vars->code_length - vars->inf_symbols_q - 1];
		if (feedback != 0) {
			for (j = vars->code_length - vars->inf_symbols_q - 1; j > 0; j--)
				if (vars->generator_polynomial[j] != 0)
					vars->bb[j] = vars->bb[j - 1] ^ feedback;
				else
					vars->bb[j] = vars->bb[j - 1];
			vars->bb[0] = vars->generator_polynomial[0] && feedback;
		} else {
			for (j = vars->code_length - vars->inf_symbols_q - 1; j > 0; j--)
				vars->bb[j] = vars->bb[j - 1];
			vars->bb[0] = 0;
		}
	}
}

void decode_bch(bch_codec_kernel_vars_t vars)
/*
 * implementation of Berlekamp's algorithm.
 *
 * Assume we have received bits in received_codeword[i], i=0..(code_length_max-1).
 *
 * Compute the 2*error_correction syndromes by substituting alpha^i into rec(X) and
 * evaluating, storing the syndromes in s[i], i=1..2t (leave s[0] zero) .
 * Then we use the Berlekamp algorithm to find the error location polynomial
 * elp[i].
 *
 * If the degree of the elp is >error_correction, then we cannot correct all the errors, and
 * we have detected an uncorrectable error pattern. We output the information
 * bits uncorrected.
 *
 * If the degree of elp is <=error_correction, we substitute alpha^i , i=1..code_length_max into the elp
 * to get the roots, hence the inverse roots, the error location numbers.
 * This step is usually called "Chien's search".
 *
 * If the number of errors located is not equal the degree of the elp, then
 * the decoder assumes that there are more than error_correction errors and cannot correct
 * them, only detect them. We output the information bits uncorrected.
 */
{
	register int    i, j, u, q, t2, count = 0, syn_error = 0;
	static int             elp[1026][1024], code_distance[1026], l[1026], u_lu[1026], s[1025];
	static int             root[200], loc[200], /*err[1024],*/ reg[201];

	t2 = 2 * vars->error_correction;

	/* first form the syndromes */
	printf_d("S(x) = ");
	for (i = 1; i <= t2; i++) {
		s[i] = 0;
		for (j = 0; j < vars->code_length; j++)
			if (vars->received_codeword[j] != 0)
				s[i] ^= vars->alpha_to[(i * j) % vars->code_length_max];
		if (s[i] != 0)
			syn_error = 1; /* set error flag if non-zero syndrome */
/*
 * Note:    If the encode is used only for ERROR DETECTION, then
 *          exit program here indicating the presence of errors.
 */
		/* convert syndrome from polynomial form to index form  */
		s[i] = vars->index_of[s[i]];

		printf_d("%3d ", s[i]);
	}
	printf_d("\n");

	if (syn_error) {	/* if there are errors, try to correct them */
		/*
		 * Compute the error location polynomial via the Berlekamp
		 * iterative algorithm. Following the terminology of Lin and
		 * Costello's book :   code_distance[u] is the 'mu'th discrepancy, where
		 * u='mu'+1 and 'mu' (the Greek letter!) is the step number
		 * ranging from -1 to 2*error_correction (see L&C),  l[u] is the degree of
		 * the elp at that step, and u_l[u] is the difference between
		 * the step number and the degree of the elp. 
		 */
		/* initialise table entries */
		code_distance[0] = 0;			/* index form */
		code_distance[1] = s[1];		/* index form */
		elp[0][0] = 0;		/* index form */
		elp[1][0] = 1;		/* polynomial form */
		for (i = 1; i < t2; i++) {
			elp[0][i] = -1;	/* index form */
			elp[1][i] = 0;	/* polynomial form */
		}
		l[0] = 0;
		l[1] = 0;
		u_lu[0] = -1;
		u_lu[1] = 0;
		u = 0;
 
		do {
			u++;
			if (code_distance[u] == -1) {
				l[u + 1] = l[u];
				for (i = 0; i <= l[u]; i++) {
					elp[u + 1][i] = elp[u][i];
					elp[u][i] = vars->index_of[elp[u][i]];
				}
			} else
				/*
				 * search for words with greatest u_lu[q] for
				 * which code_distance[q]!=0 
				 */
			{
				q = u - 1;
				while ((code_distance[q] == -1) && (q > 0))
					q--;
				/* have found first non-zero code_distance[q]  */
				if (q > 0) {
				  j = q;
				  do {
				    j--;
				    if ((code_distance[j] != -1) && (u_lu[q] < u_lu[j]))
				      q = j;
				  } while (j > 0);
				}
 
				/*
				 * have now found q such that code_distance[u]!=0 and
				 * u_lu[q] is maximum 
				 */
				/* store degree of new elp polynomial */
				if (l[u] > l[q] + u - q)
					l[u + 1] = l[u];
				else
					l[u + 1] = l[q] + u - q;
 
				/* form new elp(x) */
				for (i = 0; i < t2; i++)
					elp[u + 1][i] = 0;
				for (i = 0; i <= l[q]; i++)
					if (elp[q][i] != -1)
						elp[u + 1][i + u - q] = 
                                   vars->alpha_to[(code_distance[u] + vars->code_length_max - 
								   code_distance[q] + elp[q][i]) % vars->code_length_max];
				for (i = 0; i <= l[u]; i++) {
					elp[u + 1][i] ^= elp[u][i];
					elp[u][i] = vars->index_of[elp[u][i]];
				}
			}
			u_lu[u + 1] = u - l[u + 1];
 
			/* form (u+1)th discrepancy */
			if (u < t2) {	
			/* no discrepancy computed on last iteration */
			  if (s[u + 1] != -1)
			    code_distance[u + 1] = vars->alpha_to[s[u + 1]];
			  else
			    code_distance[u + 1] = 0;
			    for (i = 1; i <= l[u + 1]; i++)
			      if ((s[u + 1 - i] != -1) && (elp[u + 1][i] != 0))
			        code_distance[u + 1] ^= vars->alpha_to[(s[u + 1 - i] 
			                      + vars->index_of[elp[u + 1][i]]) % vars->code_length_max];
			  /* put code_distance[u+1] into index form */
			  code_distance[u + 1] = vars->index_of[code_distance[u + 1]];	
			}
		} while ((u < t2) && (l[u + 1] <= vars->error_correction));
 
		u++;
		if (l[u] <= vars->error_correction) {/* Can correct errors */
			/* put elp into index form */
			for (i = 0; i <= l[u]; i++)
				elp[u][i] = vars->index_of[elp[u][i]];


			printf_d("sigma(x) = ");
			for (i = 0; i <= l[u]; i++)
				printf_d("%3d ", elp[u][i]);
			printf_d("\n");
			printf_d("Roots: ");

			/* Chien search: find roots of the error location polynomial */
			for (i = 1; i <= l[u]; i++)
				reg[i] = elp[u][i];
			count = 0;
			for (i = 1; i <= vars->code_length_max; i++) {
				q = 1;
				for (j = 1; j <= l[u]; j++)
					if (reg[j] != -1) {
						reg[j] = (reg[j] + j) % vars->code_length_max;
						q ^= vars->alpha_to[reg[j]];
					}
				if (!q) {	/* store root and error
						 * location number indices */
					root[count] = i;
					loc[count] = vars->code_length_max - i;
					count++;

					printf_d("%3d ", vars->code_length_max - i);

				}
			}

			printf_d("\n");

			if (count == l[u])	
			/* no. roots = degree of elp hence <= error_correction errors */
				for (i = 0; i < l[u]; i++)
					vars->received_codeword[loc[i]] ^= 1;
			else	/* elp has degree >error_correction hence cannot solve */

				printf_d("Incomplete decoding: errors detected\n");
		}
	}
}