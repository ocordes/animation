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

/* parsecmd.c

   written by: Oliver Cordes 2014-08-30
   changed by: Oliver Cordes 2017-04-19


*/


#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <assert.h>

#include "output.h"
#include "parsetree.h"
#include "project.h"


parsenode *add_node_cmd_assign( parsenode *variable, parsenode *rvalue )
{
  parsenode *newnode;


  if ( variable == NULL )
    {
      output( 1, "NULL variable detected, cancel command!\n");
      return NULL;
    }


  if ( variable->type != node_property_definition )
    /* register only normal variables */
    register_node_variable( variable );

  newnode = new_node();

  newnode->type  = node_cmd_assign;
  newnode->left  = variable;
  newnode->right = rvalue;

  return newnode;
}


void add_node_cmd_assign_global( parsenode *variable, parsenode *rvalue )
{
  int nr;

  if ( variable == NULL )
    {
      output( 1, "NULL variable detected, cancel command!\n");
      return;
    }

  nr = find_variable( main_project->vars, variable->var );
  if ( nr == -1 )
    {
      output( 1, "Warning: variable `%s` is not defined as a global variable!\n", variable->var );
      return;
    }

  main_project->vars->vars[nr].con.type = rvalue->con->type;
  switch( rvalue->con->type )
    {
    case constant_bool:
    case constant_int:
      main_project->vars->vars[nr].con.i = rvalue->con->i;
      break;
    case constant_double:
      main_project->vars->vars[nr].con.d = rvalue->con->d;
      break;
    case constant_string:
      main_project->vars->vars[nr].con.s = strdup( rvalue->con->s );
      break;
    }

  free_node( variable );
  free_node( rvalue );
}


parsenode *add_node_cmd_break( void )
{
  parsenode *newnode;


  newnode = new_node();

  newnode->type = node_break;

  return newnode;
}


parsenode *add_node_cmd_crop( parsenode *varx1, parsenode *vary1, parsenode *varx2, parsenode *vary2 )
{
  output( 0, "Function is not implemented yet!" );
  return NULL;
}


parsenode *add_node_cmd_exit( void )
{
  parsenode *newnode;


  newnode = new_node();

  newnode->type = node_exit;

  return newnode;
}


parsenode *add_node_cmd_return( parsenode *value )
{
  parsenode *newnode;


  newnode = new_node();

  newnode->type = node_return;
  newnode->left = value;

  return newnode;
}



parsenode *add_node_cmd_image( parsenode *varx, parsenode *vary, parsenode *imagename )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_image;
  newnode->left = imagename;
  allocate_node_args( newnode, 2 );
  newnode->args[0] = varx;
  newnode->args[1] = vary;

  return newnode;
}


parsenode *add_node_cmd_load( parsenode *variable )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_load;
  newnode->left = variable;

  return newnode;
}


parsenode *add_node_cmd_mirrorx( void )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_mirrorx;
  newnode->left = NULL;
  newnode->right = NULL;

  return newnode;
}


parsenode *add_node_cmd_mirrory( void )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_mirrory;
  newnode->left = NULL;
  newnode->right = NULL;

  return newnode;
}


parsenode *add_node_cmd_mirrorxy( void )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_mirrorxy;
  newnode->left = NULL;
  newnode->right = NULL;

  return newnode;
}


parsenode *add_node_cmd_print( parsenode *variable )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_print;
  newnode->left = variable;

  return newnode;
}


parsenode *add_node_cmd_printf( parsenode *variable, parsenode *arguments )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_printf;
  newnode->left = variable;
  newnode->right = arguments;

  return newnode;
}


parsenode *add_node_cmd_quit( void )
{
  parsenode *newnode;


  newnode = new_node();

  newnode->type = node_quit;

  return newnode;
}


parsenode *add_node_cmd_system( parsenode *cmd )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_system;
  newnode->left = cmd;
  newnode->right = NULL;

  return newnode;
}


parsenode *add_node_cmd_text( parsenode *varx,
			      parsenode *vary,
			      parsenode *font,
			      parsenode *s,
			      parsenode *alpha )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_text;
  newnode->left = s;
  allocate_node_args( newnode, 4 );
  newnode->args[0] = varx;
  newnode->args[1] = vary;
  newnode->args[2] = font;
  newnode->args[3] = alpha;

  return newnode;
}


parsenode *add_node_cmd_textfile( parsenode *varx,
				  parsenode *vary,
				  parsenode *font,
				  parsenode *filename,
				  parsenode *alpha )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_textfile;
  allocate_node_args( newnode, 5 );
  newnode->args[0] = varx;
  newnode->args[1] = vary;
  newnode->args[2] = font;
  newnode->args[3] = filename;
  newnode->args[4] = alpha;

  return newnode;
}


parsenode *add_node_cmd_line( parsenode *start,
                              parsenode *end,
                              parsenode *pendef )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_line;
  allocate_node_args( newnode, 3);
  newnode->args[0] = start;
  newnode->args[1] = end;
  newnode->args[2] = pendef;

  return newnode;
}


parsenode *add_node_cmd_circle( parsenode *center,
                                parsenode *radius,
                                parsenode *pendef,
                                int        filldef )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->flags = filldef;
  newnode->type = node_cmd_circle;
  allocate_node_args( newnode, 3);
  newnode->args[0] = center;
  newnode->args[1] = radius;
  newnode->args[2] = pendef;

  return newnode;
}


parsenode *add_node_cmd_rectangle( parsenode *start,
                                   parsenode *size,
                                   parsenode *pendef,
                                  int         filldef)
{
  parsenode *newnode;

  newnode = new_node();

  newnode->flags = filldef;
  newnode->type = node_cmd_rectangle;
  allocate_node_args( newnode, 3);
  newnode->args[0] = start;
  newnode->args[1] = size;
  newnode->args[2] = pendef;

  return newnode;
}


parsenode *add_node_cmd_roundrectangle( parsenode *start,
                                        parsenode *size,
                                        parsenode *radius,
                                        parsenode *pendef,
                                        int        filldef )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->flags = filldef;
  newnode->type = node_cmd_roundrectangle;
  allocate_node_args( newnode, 4);
  newnode->args[0] = start;
  newnode->args[1] = size;
  newnode->args[2] = radius;
  newnode->args[3] = pendef;

  return newnode;
}


parsenode *add_node_cmd_polygon( parsenode *array, parsenode *pendef, int filldef )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->flags = filldef;
  newnode->type = node_cmd_polygon;
  allocate_node_args( newnode, 2);
  newnode->args[0] = array;
  newnode->args[1] = pendef;

  return newnode;
}


parsenode *add_node_cmd_polyline( parsenode *array, parsenode *pendef, int filldef )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->flags = filldef;
  newnode->type = node_cmd_polyline;
  allocate_node_args( newnode, 2);
  newnode->args[0] = array;
  newnode->args[1] = pendef;

  return newnode;
}


parsenode *add_node_cmd_imagefade( parsenode *args )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type = node_cmd_imagefade;
  newnode->left = args;

  return newnode;
}


parsenode *add_node_cmd_macro( parsenode *func, parsenode *args )
{
  parsenode *newnode;

  assert( func != NULL );

  newnode        = new_node();
  newnode->type  = node_cmd_macro;
  newnode->left  = func;
  newnode->right = args;

  return newnode;
}


parsenode *add_node_random( void )
{
  parsenode *newnode;

  newnode        = new_node();
  newnode->type  = node_cmd_random;

  return newnode;
}


parsenode *add_node_random_point( void )
{
  parsenode *newnode;

  newnode        = new_node();
  newnode->type  = node_cmd_random_point;

  return newnode;
}


parsenode *add_node_random_seed( parsenode *seed_val )
{
  parsenode *newnode;

  newnode        = new_node();
  newnode->type  = node_cmd_random_seed;
  newnode->left  = seed_val;

  return newnode;
}
