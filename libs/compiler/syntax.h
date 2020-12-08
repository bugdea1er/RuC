/*
 *	Copyright 2020 Andrey Terekhov, Victor Y. Fadeev
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 */

#pragma once

#include "defs.h"
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Global vars definition */
typedef struct syntax
{
	// mem, pc & iniprocs - usage here only for codes printing

	int mem[MAXMEMSIZE];		/**< Memory */
	int pc;						/**< Program counter */

	int iniprocs[INIPROSIZE];	/**< Init processes */
	int procd;					/**< Process management daemon */

	int functions[FUNCSIZE];	/**< Functions table */
	int funcnum;				/**< Number of functions */

	int identab[MAXIDENTAB];	/**< Identifiers table */
	int id;						/**< Number of identifiers */

	int modetab[MAXMODETAB];	/**< Modes table */
	int md;						/**< Number of modes */
	int startmode;				/**< Start of last record in modetab */
	
	int tree[MAXTREESIZE];		/**< Tree */
	int tc;						/**< Tree counter */

	int reprtab[MAXREPRTAB];	/**< Representations table */
	int rp;						/**< Representations size */
	int repr;					/**< Representations position */

	int maxdisplg;				/**< Max displacement */
	int wasmain;				/**< Main function flag */

	int anstdispl;				/**< Stack displacement */
} syntax;


/**
 *	Create Syntax structure
 *
 *	@return	Syntax structure
 */
syntax sx_create();

/**
 *	Add new record to modetab
 *
 *	@param	sx	Syntax structure
 *	@param	record	Pointer to the new record
 *	@param	size	Size of the new record
 *
 *	@return	Index of the new record from modetab
 */
size_t mode_add(syntax *const sx, const int *const record, const size_t size);

/**
 *	Get an item from modetab by index
 *
 *	@param	sx	Syntax structure
 *	@param	index	Index of record
 *	@return	Item by index from modetab
 */
int mode_get(syntax *const sx, const size_t index);

#ifdef __cplusplus
} /* extern "C" */
#endif
