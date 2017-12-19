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

/* blocks.c

   written by: Oliver Cordes 2013-03-29
   changed by: Oliver Cordes 2017-04-14

   $Id$

*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <math.h>

#include <assert.h>

#include "config.h"

#include "abort.h"
#include "blocks.h"
#include "helpers.h"
#include "output.h"
#include "project.h"


blockdef    *current_block;

/* file structure functions */

int file_create_empty_files( blockdef *block,
                            int nrfiles,
                            int dimx,
                            int dimy,
                            int fps  )
{
  filedef *files;
  int      nroff;
  int      i;
  char     dummy[100];

  assert( block != NULL );

  nroff = block->nrfiles;

  files = (filedef*) malloc( sizeof( blockdef ) * ( nrfiles + nroff ) );

  if ( files == NULL )
    {
      fprintf( stderr, "Can't allocate memory for file definitions!\n" );
      return 1;
    }


  if ( nroff == 0 )
    block->files = files;
  else
    {
      for (i=0;i<nroff;i++)
	files[i] = block->files[i];
      free( block->files );
      block->files = files;
    }

  for (i=0;i<nrfiles;i++)
    {
      snprintf( dummy, 100, "empty file %i", nroff+i+1 );
      files[nroff+i].type = file_type_empty;
      files[nroff+i].name = strdup( dummy );
      files[nroff+i].dimx = dimx;
      files[nroff+i].dimy = dimy;
    }

  block->nrfiles += nrfiles;

  return 0;
}


int file_create_dir_files( blockdef *block, char *filedescr, int fps, int maxfiles )
{
  filedef *files;
  filedef *temp;
  int      nrfiles = 0;

  FILE    *pipe;
  int      len;
  char    *cmd;
  char    *line;

  int      nroff;
  int      i;


  assert( block != NULL );

  /* prepare pipe command */
  len = strlen( filedescr ) + 100;
  cmd = (char*) malloc( len );
  line = (char*) malloc( len );
  sprintf( cmd, "ls %s | sort -n", filedescr );
  output( 2, "file_create_dir_files: cmd=%s\n", cmd );
  output( 2, "file_create_dir_files: maxfiles=%i\n", maxfiles );

  /* copy all files into a temporary array */
  temp = (filedef*) malloc( sizeof( filedef) * max_files_per_loop );

  if ( temp == NULL ) aabort( abort_msg_malloc, "temporary file description" );

  pipe = popen( cmd, "r" );
  if ( pipe == NULL )
    output( 1, "ls command failed! errno=%s\n", strerror( errno ) );

  while ( fgets( line, len, pipe) != NULL )
  {
    output( 3, line );
    if ( ( maxfiles != -1 ) && ( nrfiles >= maxfiles ) )
      continue;
    /* strip filenames */
    line[strlen(line)-1] = '\0';
    temp[nrfiles].type = file_type_file;
    temp[nrfiles].name = strdup( line );

    nrfiles++;
    output( 3, "file_create_dir_files: nr=%i name=%s\n", nrfiles, line );
  }
  pclose( pipe );
  output( 2, "file_create_dir_files: nrfiles=%i\n", nrfiles );

  free( cmd );
  free( line );


  /* copy the descriptions into the final array */

  nroff = block->nrfiles;

  files = (filedef*) malloc( sizeof( blockdef ) * ( nrfiles + nroff ) );

  if ( files == NULL ) aabort( abort_msg_malloc, "file definitions" );


  if ( nroff == 0 )
    block->files = files;
  else
  {
    for (i=0;i<nroff;++i)
	     files[i] = block->files[i];
    free( block->files );
    block->files = files;
  }

  for (i=0;i<nrfiles;++i)
  {
    files[nroff+i].type = file_type_file;
    files[nroff+i].name = temp[i].name;
    files[nroff+i].dimx = -1;
    files[nroff+i].dimy = -1;
  }

  free( temp );

  block->nrfiles += nrfiles;

  return 0;
}


int file_create_static_files( blockdef *block, char *filename, int nrfiles )
{
  filedef *files;
  int      nroff;
  int      i;

  assert( block != NULL );

  nroff = block->nrfiles;

  files = (filedef*) malloc( sizeof( blockdef ) * ( nrfiles + nroff ) );

  if ( files == NULL )
    {
      fprintf( stderr, "Can't allocate memory for file definitions!\n" );
      return 1;
    }


  if ( nroff == 0 )
    block->files = files;
  else
    {
      for (i=0;i<nroff;i++)
	files[i] = block->files[i];
      free( block->files );
      block->files = files;
    }

  for (i=0;i<nrfiles;i++)
    {
      files[nroff+i].type = file_type_file;
      files[nroff+i].name = strdup( filename );
      files[nroff+i].dimx = -1;
      files[nroff+i].dimy = -1;
    }

  block->nrfiles += nrfiles;

  return 0;
}



/* block setting routines */

void block_new_block( parsenode *name )
{
  char *s;

  if ( name != NULL )
    {
      s = get_string_from_node( name );
      free_node( name );
    }
  else
    {
      s = strdup( "foo" );
    }

  block_new_block_name( s );
  free( s );
}


void block_new_block_name( char *name )
{
  check_command_status( parser_status_default, "block" );

  current_block = (blockdef*) malloc( sizeof( blockdef ) );

  if ( current_block == NULL ) aabort( abort_msg_malloc, "block structure" );

  current_block->name = strdup( name );
  output( 1,"Creating a new block '%s' ...\n", current_block->name );

  push_parser_status( parser_status_block );

  current_block->nrfiles  = 0;
  current_block->files    = NULL;
  current_block->vars     = new_variables();
  current_block->commands = NULL;

  if ( strcmp( name, "postproc" ) == 0 )
    {
    }
  else
    {
      add_variable( current_block->vars, "FILE", constant_string );
      add_variable( current_block->vars, "IMAGENR", constant_int );
      add_variable( current_block->vars, "MAXFRAMES", constant_int );
    }
}


void block_free_block( blockdef *block )
{
  int i;

  assert( block != NULL );


  /* release memory */
  for (i=0;i<block->nrfiles;i++)
    nfree( block->files[i].name );
  nfree( block->files );

  free_variables( block->vars );
  free_node( block->commands );
  nfree( block->name );

  nfree( block );
}


void project_update_variables( void )
{
  int seconds;
  int minutes;
  int hours;
  int frames;

  seconds = main_project->framenr / main_project->fps;
   frames = main_project->framenr % main_project->fps;

  minutes = seconds / 60;
  seconds = seconds % 60;

  hours = minutes / 60;
  minutes = minutes % 60;

  set_int_variable( main_project->vars, "MTIME.FRAME", frames );
  set_int_variable( main_project->vars, "MTIME.SEC", seconds );
  set_int_variable( main_project->vars, "MTIME.MIN", minutes );
  set_int_variable( main_project->vars, "MTIME.HOUR", hours );
}


void block_add_commands( parsenode *commands )
{
  if ( current_block != NULL )
	  current_block->commands = commands;
}


parsenode*  block_add_files_empty( parsenode *nrfiles,
                                  parsenode *ndimx,
                                  parsenode *ndimy,
                                  parsenode *nfps )
{
  int nr;
  int dimx;
  int dimy;
  int fps;

  assert( current_block != NULL );
  assert( nrfiles != NULL );

  nr = get_int_from_node( nrfiles );
  free_node( nrfiles );

  if ( ndimx != NULL )
    {
      dimx = get_int_from_node( ndimx );
      free_node( ndimx );
    }
  else
    {
      dimx = main_project->geometry[0];
    }

  if ( ndimy != NULL )
    {
      dimy = get_int_from_node( ndimy );
      free_node( ndimy );
    }
  else
    {
      dimy = main_project->geometry[1];
    }

  if ( nfps != NULL )
    {
      fps = get_int_from_node( nfps );
      free_node( nfps );
    }
  else
    {
      fps = main_project->fps;
    }

  file_create_empty_files( current_block, nr, dimx, dimy, fps );

  return NULL;
}


parsenode *block_add_files_string( parsenode *s,
                                   parsenode *nmaxfiles,
                                   parsenode *nfps )
{
  char *str;
  int   fps;
  int   maxfiles = -1;

  assert( current_block != NULL );
  assert( s != NULL );

  str = get_string_from_node( s );
  free_node( s );

  if ( nmaxfiles != NULL )
  {
    maxfiles = get_int_from_node( nmaxfiles );
    free_node( nmaxfiles );
  }

  if ( nfps != NULL )
  {
    fps = get_int_from_node( nfps );
    free_node( nfps );
  }
  else
  {
    fps = main_project->fps;
  }

  file_create_dir_files( current_block, str, fps, maxfiles );

  nfree( str );

  return NULL;
}


parsenode* block_add_files_static( parsenode *s, parsenode *nrfiles )
{
  char *str;
  int   nr;

  assert( current_block != NULL );
  assert( s != NULL );

  str = get_string_from_node( s );
  nr = get_int_from_node( nrfiles );

  file_create_static_files( current_block, str, nr );

  free_node( s );
  nfree( str );

  return NULL;
}
