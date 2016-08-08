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

/* macro.c 

   written by: Oliver Cordes 2014-08-31
   changed by: Oliver Cordes 2014-09-13


*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <math.h>

#include <assert.h>

#include "config.h"

#include "abort.h"
#include "helpers.h"
#include "macro.h"
#include "output.h"
#include "project.h"


macrodef *current_macro;


/* macro setup routines */

void macro_new_macro( parsenode *macroname, parsenode *varargs )
{
  parsenode *n;

  check_command_status( parser_status_default, "macro" );

  current_macro = (macrodef*) malloc( sizeof( macrodef ) );

  push_parser_status( parser_status_macro );

  if ( macroname != NULL )
    {
      current_macro->name = get_string_from_node( macroname );
      free_node( macroname );
    }
  else
    {
      current_macro->name = strdup( "foo" );
    }

  output( 1,"Creating a new macro '%s'!\n", current_macro->name );

  current_macro->vars     = new_variables();
  current_macro->commands = NULL;
  current_macro->varargs  = varargs;


  /* adding the variables to the macro */

  n = varargs;
  while ( n != NULL )
    {
      if ( n->left != NULL )
	register_node_variable( n->left );
      n = n->right;
    }
  
}


void macro_add_commands( parsenode *commands )
{
  if ( current_macro != NULL )
    current_macro->commands = commands;
}


void macro_free_macro( macrodef *macro )
{

  assert( macro != NULL );

  free_variables( macro->vars );
  free_node( macro->varargs );
  free_node( macro->commands );
  nfree( macro->name );

  nfree( macro );
}



