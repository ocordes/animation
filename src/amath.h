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

/*
  amath.h

  written by: Oliver Cordes 2010-08-01
  changed by: Oliver Cordes 2012-11-16
	
  $Id: amath.h 315 2012-11-16 18:50:30Z ocordes $

 */


#ifndef __amath_h

#define __amath_h  1

#include "parsetree.h"

constant *math_evaluate_node( constant *left,
			      constant *right,
			      int mathop );
constant *math_evaluate_node_func( constant *left,
				   int mathop );

constant *math_execute_node( parsenode *node );


int      math_execute_node_int( parsenode *node );
double   math_execute_node_double( parsenode *node );
char*  math_execute_node_string( parsenode *node );

#endif
