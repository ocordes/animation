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

/* project.c

   written by: Oliver Cordes 2010-07-02
   changed by: Oliver Cordes 2017-03-05
   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <math.h>

#include <assert.h>

#include "config.h"

#include "abort.h"
#include "blocks.h"
#include "controls.h"
#include "emoji.h"
#include "filldef.h"
#include "font.h"
#include "geometry.h"
#include "helpers.h"
#include "imagedef.h"
#include "output.h"
#include "project.h"



/* main variables */

projectdef  *main_project;


/* variable stack */

#define      max_local_vars 100
int          stack_vars = 1;
variables   *local_vars[max_local_vars];


#define      max_parser_entries 10
int          parser_status[max_parser_entries];
int          parser_status_nr                   = 0;



/* parser status functions */

char *str_parser_status( int status )
{
  static char dummy[200];

  switch( status )
    {
    case parser_status_default:
      strncpy( dummy, "outer", 200 );
      break;
    case parser_status_block:
      strncpy( dummy, "block", 200 );
      break;
    case parser_status_window:
      strncpy( dummy, "window", 200 );
      break;
    case parser_status_macro:
      strncpy( dummy, "macro", 200 );
      break;
    default:
      strncpy( dummy, "unknown", 200 );
      break;
    }

  return dummy;
}

int get_parser_status( void )
{
  if ( parser_status_nr == 0 )
    return parser_status_default;
  else
    return parser_status[parser_status_nr-1];
}


int push_parser_status( int newstatus )
{
  assert( parser_status_nr < max_parser_entries );

  parser_status[parser_status_nr] = newstatus;
  parser_status_nr++;

  output( 2, "Entering '%s' level ...\n", str_parser_status( newstatus) );

  return parser_status_nr;
}


int pop_parser_status( void )
{
  int status;

  assert( parser_status_nr > 0 );

  output( 2, "Leaving '%s' level ...\n", str_parser_status( get_parser_status() ) );

  parser_status_nr--;

  if ( parser_status_nr == 0 )
    status = parser_status_default;
  else
    status = parser_status[parser_status_nr-1];

  output( 2, "Re-entering '%s' level ...\n", str_parser_status( status ) );

  return status;
}


int check_parser_status( int status, char *command )
{
  int pstatus;
  int erg;

  pstatus = get_parser_status();

  if ( ( erg = ( status == pstatus ) ) )
    {
      output( 2, "Correct parser level '%s' for command!\n", str_parser_status( status ) );
    }
  else
    {
      fprintf( stderr, "Parser level '%s' for command '%s' not reached!\n",
	       str_parser_status( status ),
	       command );
    }

  return erg;
}



/* project functions */

void project_init( void )
{
  main_project = (projectdef*) calloc( 1, sizeof( projectdef ) );

  if ( main_project == NULL )
    {
      fprintf( stderr,
	       "Can't allocate memory for project structure! Abort program!\n" );
      exit( -1 );
    }

  /* default settings */
  main_project->name            = strdup( "foo" );
  main_project->output_device   = strdup( "jpeg" );
  main_project->output_filename = NULL;
  main_project->outputdir       = NULL;
  main_project->nrblocks        = 0;
  main_project->nrmacros        = 0;
  main_project->postproc_block  = NULL;    /* no postproc block defined */
  main_project->nrcontrols      = 0;
  main_project->vars            = new_variables();
  main_project->geometry[0]     = 640;
  main_project->geometry[1]     = 480;
  main_project->background      = strdup( "black" );
  main_project->fps             = 25;    /* default value */

  main_project->blockmovies     = 0;     /* default False */
  main_project->overwrite       = 1;     /* default True */
  main_project->framenr         = 0;

  /* create some variables */
  add_variable( main_project->vars, "FRAMENR", constant_int );
  set_int_variable( main_project->vars, "FRAMENR", 0 );

  add_variable( main_project->vars, "MTIME.FRAME", constant_int );
  add_variable( main_project->vars, "MTIME.SEC", constant_int );
  add_variable( main_project->vars, "MTIME.MIN", constant_int );
  add_variable( main_project->vars, "MTIME.HOUR", constant_int );

  project_update_variables();

  add_variable( main_project->vars, "PI", constant_double );
  set_double_variable( main_project->vars, "PI", M_PI );

  add_variable( main_project->vars, "AVERSION", constant_string );
  set_string_variable( main_project->vars, "AVERSION", VERSION );

  add_variable( main_project->vars, "BACKGROUND", constant_string );
  set_string_variable( main_project->vars, "BACKGROUND", main_project->background );

  add_variable( main_project->vars, "GEOMETRY.X", constant_int );
  add_variable( main_project->vars, "GEOMETRY.Y", constant_int );
  set_int_variable( main_project->vars, "GEOMETRY.X", 640 );
  set_int_variable( main_project->vars, "GEOMETRY.Y", 480 );

  /* one can think that these should be block variables,
     but I need the info also in the postproc block */
  add_variable( main_project->vars, "IMAGE.X", constant_int );
  add_variable( main_project->vars, "IMAGE.Y", constant_int );
  set_int_variable( main_project->vars, "IMAGE.X", 0 );
  set_int_variable( main_project->vars, "IMAGE.Y", 0 );
}


void project_done( void )
{
  int i;

  assert( main_project != NULL );

  nfree( main_project->name );
  nfree( main_project->output_device );
  nfree( main_project->output_filename );
  nfree( main_project->outputdir );
  nfree( main_project->geometry_name );
  nfree( main_project->background );

  for (i=0;i<main_project->nrblocks;i++)
    block_free_block( main_project->blocks[i] );

  if ( main_project->postproc_block != NULL )
    block_free_block( main_project->postproc_block );

  for (i=0;i<main_project->nrmacros;i++)
    macro_free_macro( main_project->macros[i] );

  for (i=0;i<main_project->nrcontrols;i++)
    controls_free_control( main_project->controls[i] );

  free_variables( main_project->vars );

  nfree( main_project );
}


void project_show( void )
{
  int  i;
  char dummy[51];

  assert( main_project != NULL );

  output( 1, "\n" );
  output( 1, "project       '%s'\n", main_project->name );
  output( 1, "-------------------------------------------------------------------------------\n" );
  output( 1, "output device : %s\n", main_project->output_device );
  output( 1, "output file   : %s\n", main_project->output_filename );
  output( 1, "geometry      : %ix%i (%s)\n",
		  main_project->geometry[0],
		  main_project->geometry[1],
	      main_project->geometry_name );
  output( 1, "blockmovies   : %s\n", print_bool( main_project->blockmovies ) );
  output( 1, "overwrite     : %s\n", print_bool( main_project->overwrite ) );
  font_print_fonts();
  imagedef_print_imagedefs();
  filldef_print_defs();
  output( 1, "global variables: " );
  print_variables( main_project->vars );

  /* print macros */
  output( 1, "# of macros   : %d\n", main_project->nrmacros );
  for (i=0;i<main_project->nrmacros;i++)
    {
      output( 1, " macro #%-3i   : %s\n",
	      i+1,
	      main_project->macros[i]->name );
      print_variables( main_project->macros[i]->vars );
    }

  /* print blocks */
  output( 1, "# of blocks   : %i\n", main_project->nrblocks );
  for (i=0;i<main_project->nrblocks;i++)
  {
	  strncpy( dummy, main_project->blocks[i]->name, 50 );
	  output( 1, " block #%-3i   : %-50s (%i files)\n",
			  (i+1),
			  dummy,
			  main_project->blocks[i]->nrfiles );
	  print_variables( main_project->blocks[i]->vars );
  }
  if ( main_project->postproc_block != NULL )
    {
      output( 1, "postproc block\n" );
      print_variables( main_project->postproc_block->vars );
    }
  else
    {
      output( 1, "no postproc block defined!\n" );
    }

  if ( main_project->nrcontrols > 0 )
    {
      output( 1, "# of controls : %i\n", main_project->nrcontrols );
      for (i=0;i<main_project->nrcontrols;i++)
	output( 1, " #%-3i: %s\n", i+1, main_project->controls[i]->name );
    }
  else
    {
      output( 1, "no control rules defined!\n" );
    }

  output( 1, "\n" );
}



/* some checks of the project definitions, if available or necessary */

int project_sanity_check( void )
{
  int erg = 0;

  int i;

  assert( main_project != NULL );

  if ( main_project->name == NULL )
    {
      fprintf( stderr, "Error: no project name given!\n" );
      erg = 1;
    }
  if ( main_project->output_filename == NULL )
    {
      fprintf( stderr, "Error: no output filename given!\n" );
      erg = 1;
    }


  if ( main_project->nrcontrols > 0 )
    {
      /* we have control definitions */
      for (i=0;i<main_project->nrcontrols;i++)
	{
	  erg = controls_verify( main_project->controls[i], (blockdef**) main_project->blocks, main_project->nrblocks );
	  if ( erg != 0 )
	    break;
	}
    }


  return erg;
}



/* project setting routines */

void project_set_project_name( parsenode *newname )
{
  char *s = NULL;

  assert( main_project != NULL );

  s = get_string_from_node( newname );

  nfree( main_project->name );
  main_project->name = strdup( s );

  nfree( s );
  free_node( newname );
}


void project_set_output_device( parsenode *newdevice )
{
  char *s = NULL;

  assert( main_project != NULL );

  s = get_string_from_node( newdevice );

  nfree( main_project->output_device );
  main_project->output_device = strdup( s );

  nfree( s );
  free_node( newdevice );
}


void project_set_output_filename_string( char* newfilename )
{
  assert( main_project != NULL );

  nfree( main_project->output_filename );
  main_project->output_filename = strdup( newfilename );
}



void project_set_output_filename( parsenode *newfilename )
{

  char *s = NULL;

  assert( main_project != NULL );

  s = get_string_from_node( newfilename );

  project_set_output_filename_string( s );

  nfree( s );
  free_node( newfilename );
}


void project_set_outputdir_string( char* newdir )
{
  assert( main_project != NULL );

  nfree( main_project->outputdir );

  if ( newdir[0] == '~' )
    {
      char* homedir, *p;

      char  dummy[1000];

      homedir = getenv( "HOME" );
      p = ++newdir;
      snprintf( dummy, 1000, "%s%s", homedir, p );
      main_project->outputdir = strdup( dummy );
    }
  else
    main_project->outputdir = strdup( newdir );
}


void project_set_outputdir( parsenode *newdir )
{
  char *s = NULL;

  assert( main_project != NULL );

  s = get_string_from_node( newdir );

  project_set_outputdir_string( s );

  nfree( s );
  free_node( newdir );
}



void project_set_geometry( parsenode *newgeometry )
{
  int width;
  int height;

  char *newg = NULL;

  int erg, j;

  assert( main_project != NULL );

  newg = get_string_from_node( newgeometry );

  nfree( main_project->geometry_name );
  main_project->geometry_name = strdup( newg );

  for (j=0;j<strlen(newg);j++)
    newg[j] = toupper( newg[j] );

  erg = lookup_geometry( newg, &width, &height);

  if ( erg != 0 )
    erg = translate_geometry( newg, &width, &height);

  if ( erg != 0 )
    {
      fprintf( stderr, "Can't convert geometry! Ignoring '%s'!\n",
	       newg );
      main_project->geometry[0] = 640;
      main_project->geometry[1] = 480;
    }
  else
    {
      main_project->geometry[0] = width;
      main_project->geometry[1] = height;
    }

  set_int_variable( main_project->vars, "GEOMETRY.X", main_project->geometry[0] );
  set_int_variable( main_project->vars, "GEOMETRY.Y", main_project->geometry[1] );

  nfree( newg );
  free_node( newgeometry );
}


void project_set_fps( parsenode* newfps)
{
  assert( main_project != NULL );

  main_project->fps = get_int_from_node( newfps );

  free_node( newfps );
}


void project_set_background( parsenode* newbackground)
{
  char *newb;

  assert( main_project != NULL );

  newb = get_string_from_node( newbackground );

  nfree( main_project->background );
  main_project->background = strdup( newb );
  set_string_variable( main_project->vars, "BACKGROUND", main_project->background );

  free( newb );
  free_node( newbackground );
}


void project_set_bitrate( parsenode* newbitrate)
{
  assert( main_project != NULL );

  main_project->bitrate = get_int_from_node( newbitrate );

  free_node( newbitrate );
}


void project_set_blockmovies( parsenode *newblockmovies )
{
  assert( main_project != NULL );

  main_project->blockmovies =  get_bool_from_node( newblockmovies );

  free_node( newblockmovies );
}


void project_set_overwrite( parsenode *newoverwrite )
{
  assert( main_project != NULL );

  main_project->overwrite = get_bool_from_node( newoverwrite );

  free_node( newoverwrite );
}


void project_add_block( blockdef *newblock )
{
  int i;

  pop_parser_status();
  assert( main_project != NULL );

  if ( newblock == NULL)
    {
      fprintf( stderr, "New block definition is NULL! Ignoring new block!\n");
      return;
    }

  if ( strcmp( newblock->name, "postproc" ) == 0 )
    {
       output( 1, "Adding new postproc block.%s\n", _em[_em_okay] );
       main_project->postproc_block = newblock;
    }
  else
    {
      if ( main_project->nrblocks < max_blocks )
	{
	  output( 1, "Adding new block '%s'.%s\n", newblock->name, _em[_em_okay] );
	  main_project->blocks[main_project->nrblocks] = newblock;
	  main_project->nrblocks++;
	}
      else
	{
	  fprintf( stderr,
		   "Too much block definitions (limit=%i)! Ignoring new block '%s'!\n",
		   max_blocks, newblock->name );
	}

      output( 2, "block contains %i files:\n", newblock->nrfiles );
      for (i=0;i<newblock->nrfiles;i++)
	output( 2, "file: %s (type=%i)\n",
		newblock->files[i].name,
		newblock->files[i].type);
    }
}


void project_add_macro( macrodef *newmacro )
{
  pop_parser_status();

  assert( main_project != NULL );

  if ( newmacro == NULL)
    {
      fprintf( stderr, "New macro definition is NULL! Ignoring new macro!\n");
      return;
    }

  if ( main_project->nrmacros < max_macros )
    {
      output( 1, "Adding new macro '%s'.%s\n", newmacro->name, _em[_em_okay] );
      main_project->macros[main_project->nrmacros] = newmacro;
      main_project->nrmacros++;
    }
  else
    {
      fprintf( stderr,
	       "Too much macro definitions (limit=%i)! Ignoring new macro '%s'!\n",
	       max_macros, newmacro->name );
    }

}


macrodef *project_get_macro( parsenode *func )
{
  int found = 0;
  int i;

  for (i=0;i<main_project->nrmacros;i++)
    {
      if ( strcmp( main_project->macros[i]->name, func->con->s ) == 0 )
	{
	  found = 1;
	  break;
	}
    }
  if ( found == 1 )
    return main_project->macros[i];
  else
    {
      output( 1, "macro '%s' not found!\n", func->con->s );
      return NULL;
    }
}


/* stack local variables procedures */

void project_push_local_variables( variables *vars )
{
  int i;

  if ( stack_vars < max_local_vars )
    {
      for (i=stack_vars;i>0;i--)
	{
	  output( 2, "i=%i i-1=%i\n", i, i-1 );
	  local_vars[i] = local_vars[i-1];
	}
      local_vars[0] = vars;
      stack_vars++;
      output( 2, "Push local variables: %i entries\n", stack_vars );

      for (i=0;i<stack_vars;i++)
	output( 2, "v[%i]=%x\n", i, local_vars[i] );
    }
  else
    {
      output( 1, "Too many recursions! push_local_variables failed!\n" );
    }
}


void project_pop_local_variables( void )
{
  int i;

  if ( stack_vars <= 1 )
    output( 1, "No local variables on stack! pop_local_variables failed!\n" );
  else
    {
      stack_vars--;
      for (i=0;i<stack_vars;i++)
	{
	  output( 2, "i=%i i-1=%i\n", i, i+1 );
	  local_vars[i] = local_vars[i+1];
	}
      output( 2, "Pop local variables: %i entries\n", stack_vars );

      for (i=0;i<stack_vars;i++)
	output( 2, "v[%i]=%x\n", i, local_vars[i] );
    }
}
