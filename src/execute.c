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

/* execute.c

   written by: Oliver Cordes 2010-07-02
   changed by: Oliver Cordes 2017-02-06

   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#include "amath.h"
#include "emoji.h"
#include "execute.h"
#include "image.h"
#include "magick.h"
#include "mprintf.h"
#include "output.h"
#include "parsetree.h"
#include "project.h"
#include "variable.h"


filedef *current_start_image;

/* return value storage */
constant  *return_value = NULL;


/* Prototypes */

int execute_image_script( parsenode *commands );

/* end of Prototypes */



/* command procedures */

int execute_if( parsenode *cond, parsenode *then_cmds, parsenode *else_cmds )
{
  constant *con;
  int       erg;


  con = math_execute_node( cond );

  output( 2, "if cond=%s\n", con->b?"True":"False" );

  if ( con->b )
    erg = execute_image_script( then_cmds );
  else
    erg = execute_image_script( else_cmds );

  free_constant( con );

  return erg;
}


int execute_cmd_assign( parsenode *variable, parsenode *rvalue )
{
  constant *con;
  int       erg;

  con = math_execute_node( rvalue );

  /* lookup only in block scope variables */
  erg = set_constant_variable( local_vars[0], variable->var, con );

  free_constant( con );

  if ( erg != 0 )
    {
      output( 1, "Variable `%s` not found!\n", variable->var );
      return -1;
    }

  return 0;
}


int execute_cmd_window( parsenode *node )
{
  int erg;

  magick_image_window( node->args[0], node->args[1], node->args[2], node->args[3] );

  erg = execute_image_script( node->left );

  magick_image_endwindow();

  return 0;
}


int execute_cmd_load( parsenode *variable )
{
  constant *con;

  con = math_execute_node( variable );

  if ( con != NULL )
    {
      if ( con->type == constant_string )
	       magick_image_load( current_start_image );
      else
	    {
	       output( 1, "LOAD: variable doesn't contain a string (con->type=%i)! Abort LOAD!\n", con->type );
	    }
    }
  else
    output( 1, "LOAD: NULL variable! Abort LOAD!\n" );

  free_constant( con );

  return 0;
}


int execute_cmd_print( parsenode *variable )
{
  constant *con;

  con  = math_execute_node( variable );

  if ( con == NULL )
    {
      output( 1, "%s: NULL\n", _em[_em_printer] );
      return 0;
    }

  switch( con->type )
    {
    case constant_none:
      output( 1, "%s: NONE\n", _em[_em_printer] );
      break;
    case constant_int:
      output( 1, "%s: %i\n", _em[_em_printer], con->i );
      break;
    case constant_double:
      output( 1, "%s: %f\n", _em[_em_printer], con->d );
      break;
    case constant_string:
      output( 1, "%s: %s\n", _em[_em_printer], con->s );
      break;
    case constant_bool:
      output( 1, "%s: %s\n", _em[_em_printer], (con->b?"TRUE":"FALSE" ) );
      break;
    }

  free_constant( con );

  /*
    just an idea ... not really working
    mprintf( "%s", variable );
  */

  return 0;
}


int execute_cmd_text( parsenode *node )
{
  magick_text( node->args[0], node->args[1], node->left, node->args[2], node->args[3] );

  return 0;
}


int execute_cmd_textfile( parsenode *node )
{
  magick_textfile( node->args[0], node->args[1], node->args[2], node->args[3], node->args[4] );

  return 0;
}


int execute_cmd_image( parsenode *node )
{
  magick_drawimage( node->args[0], node->args[1], node->left );

  return 0;
}



int execute_cmd_macro( parsenode *node )
{
  int       erg;
  macrodef *macro;

  parsenode *arg;
  parsenode *varg;
  constant  *con;

  int        larg;
  int        lvarg;


  macro = project_get_macro( node->left );


  if ( macro != NULL )
    {
      /* evaluate all arguments */
      arg   = node->right;
      varg  = macro->varargs;

      larg  = get_node_vararglist_len( arg );
      lvarg = get_node_vararglist_len( varg );


      if ( larg != lvarg )
	{
	  output( 1, "Not equal arguments and varargs (%i vs. %i)!\n", larg, lvarg );
	  return 0;
	}

      while ( arg != NULL )
	{
	  /* break if more arguments than variable exists */
	  if ( varg == NULL ) break;

	  con = math_execute_node( arg->left );

	  erg = set_constant_variable( macro->vars, varg->left->var, con );

	  free_constant( con );

	  if ( erg != 0 )
	    {
	      output( 1, "Macro-variable '%s' not found!\n", varg->left->var );
	      break;
	    }

	  arg = arg->right;

	  varg = varg->right;
	}

      /* activate new variables */
      project_push_local_variables( macro->vars );

      /* execute macro */
      erg = execute_image_script( macro->commands );

      /* restore old variable block */
      project_pop_local_variables();

    }
  else
    {
      output( 1, "Cannot execute macro!\n" );
    }

  return 0;
}


int execute_cmd_return( parsenode *val )
{
  assert( return_value == NULL );

  return_value = math_execute_node( val );

  return return_return;
}



/* check  procedures */

int  execute_check( void )
{
  int erg = 0;

  output( 0, "Running sanity checks ...\n" );
  erg = project_sanity_check();
  output( 0, "Done. %s\n", _em[_em_okay] );

  return erg;
}


int execute_image_script( parsenode *commands )
{
  int        erg = return_ok;
  parsenode *start;

  start = commands;
  while ( start != NULL )
    {
      switch( start->type )
	{
	case node_quit:
	  erg = return_quit;
	  break;
	case node_exit:
	  erg = return_exit;
	  break;
	case node_break:
	  erg = return_break;
	  break;
	case node_return:
	  erg = execute_cmd_return( start->left );
	  break;
	case node_if:
	  erg = execute_if( start->cond, start->left, start->right );
	  break;
	case node_cmd_assign:
	  erg = execute_cmd_assign( start->left, start->right );
	  break;
	case node_cmd_window:
	  erg = execute_cmd_window( start );
	  break;
	case node_cmd_load:
	  erg = execute_cmd_load( start->left );
	  break;
	case node_cmd_print:
	  erg = execute_cmd_print( start->left );
	  break;
	case node_cmd_text:
	  erg = execute_cmd_text( start );
	  break;
	case node_cmd_textfile:
	  erg = execute_cmd_textfile( start );
	  break;
	case node_cmd_image:
	  erg = execute_cmd_image( start );
	  break;
	case node_cmd_macro:
	  /* simple macro calls ... */
	  erg = execute_cmd_macro( start );

	  /* throw away return values ... */
	  if ( return_value != NULL )
	    {
	      output( 1, "Freeing unused return value!\n" );
	      free_constant( return_value );
	      return_value = NULL;
	    }
	  break;
	}

      /* break the execution if there is any error */
      if ( erg != return_ok )
	return erg;

      start = start->next;
    }

  return erg;
}


int execute_image( int imagenr )
{
  int erg = return_ok;

  /* prepare input file for LOAD */
  current_start_image = &current_block->files[imagenr];

  /* set global variables */
  main_project->framenr++;
  set_int_variable( main_project->vars, "FRAMENR", main_project->framenr );
  project_update_variables();

  /* setup variables */
  local_vars[0] = current_block->vars;
  set_int_variable( local_vars[0], "IMAGENR", imagenr );
  set_string_variable( local_vars[0], "FILE", current_block->files[imagenr].name );

  set_int_variable( local_vars[0], "MAXFRAMES", current_block->nrfiles );

  /* execute block script */
  erg = execute_image_script( current_block->commands );

  /* resize the image */
  magick_resize( 1 );   /* keep the aspect ratio */

  /* execute postproc block script  if available */
  if ( main_project->postproc_block != NULL )
    {
      local_vars[0] = main_project->postproc_block->vars;
      erg = execute_image_script( main_project->postproc_block->commands );
    }


  /* write image */
  image_out_write();


  return erg;
}


int execute_block( int blocknr )
{
  int       erg = return_ok;
  int       i;

  current_block = main_project->blocks[blocknr];

  output( 1, "Block #%i: '%s' ...\n", blocknr+1, current_block->name );

  for (i=0;i<current_block->nrfiles;i++)
    {
      erg = execute_image( i );

      if ( erg == return_break )
	{
	  /* exit of image detected */
	  erg = return_ok;
	  continue;
	}

      if ( erg != return_ok )
	break;

      /* for debugging only execute the first image */
      /*break; */
    }

  output( 1, "Done.%s\n", _em[_em_okay] );

  return erg;
}


int execute_execute( void )
{
  int erg = return_ok;

  int block;

  image_out_init( main_project->output_device );

  output( 1, "Executing script ...\n" );

  for (block=0;block<main_project->nrblocks;block++)
    {
      erg = execute_block( block );

      if ( erg == return_exit )
	{
	  /* exit of block detected */
	  erg = return_ok;
	  continue;
	}

      if ( erg != return_ok )
	break;
    }

  output( 1, "Execution finished!\n" );

  image_out_done();

  return erg;
}
