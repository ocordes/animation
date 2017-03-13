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

/* random.c

   written by: Oliver Cordes 2017-03-12
   changed by: Oliver Cordes 2017-03-14

   $Id$

*/

#include <stdlib.h>

#include "config.h"
#include "project.h"

#include "output.h"
#include "parsetree.h"
#include "type_point.h"
#include "variable.h"


/* module functions */

void random_init( void )
{
  srand48( 1234567890 );
}


constant *execute_cmd_random( void )
{
  constant *con;

  con = new_constant();
  con->type = constant_double;

  con->d = drand48();

  return con;
}


constant *execute_cmd_random_point( void )
{
  constant  *con;
  double     x,y ;

  x = drand48();
  y = drand48();

  con = add_constant_point( x, y );

  return con;
}


void random_seed( parsenode *seed )
{
  int i;

  i = get_int_from_node( seed );

  srand48( i );
}
