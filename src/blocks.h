/* (C) Copyright 2010 by Oliver Cordes
        - ocordes ( at ) astro ( dot ) uni-bonn ( dot ) de


    This file is part of animation.

    animation is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    animation is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with animation.  If not, see <http://www.gnu.org/licenses/>.

*/

/* blocks.h

   written by: Oliver Cordes 2013-03-29
   changed by: Oliver Cordes 2017-02-01

   $Id$

*/

#ifndef __blocks_h

#define __blocks_h 1


#include "parsetree.h"
#include "variable.h"

#define max_files_per_loop 10000

#define file_type_empty 0
#define file_type_file  1


typedef struct{
	int   type;
	char *name;
	int   dimx;
	int   dimy;
} filedef;


#define max_blocks   100


typedef struct{
  char      *name;
  parsenode *commands;
  filedef   *files;
  int        nrfiles;
  variables *vars;                /* local variables */
	int        maxfiles;
} blockdef;


extern blockdef    *current_block;

/* file creation reoutines */

int file_create_empty_files( blockdef *block, int nrfiles, int dimx, int dimy, int fps );
int file_create_dir_files( blockdef *block, char *filedescr, int fps );
int file_create_static_files( blockdef *block, char *filename, int nrfiles );


/* block setting routines */

void       block_new_block( parsenode  *name );
void       block_new_block_name( char *name );
void       block_free_block( blockdef *block );
void       block_add_commands( parsenode *commands );
parsenode* block_add_files_empty( parsenode *nrfiles, parsenode *ndimx, parsenode *ndimy, parsenode *nfps );
parsenode* block_add_files_string( parsenode *s, parsenode *nfps );
parsenode* block_add_files_static( parsenode *s, parsenode *nrfiles );


void       project_update_variables( void );

#endif
