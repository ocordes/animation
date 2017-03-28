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


/* type_array.h

written by: Oliver Cordes 2017-02-25
changed by: Oliver Cordes 2017-03-28

$Id$

*/

#ifndef __type_array_h

#define __type_array_h 1

#include "config.h"

#include "parsetree.h"
#include "variable.h"

void free_array_constant( constant *con );
void clone_array_constant( constant*, constant* );

/* array node definitions */
parsenode *add_node_array( parsenode * );
parsenode *add_node_array_list( parsenode *, parsenode * );
parsenode *add_node_array_element( parsenode * );
parsenode *add_node_array_elements( parsenode*, parsenode * );


/* aaray 2 string conversion */
char *array2str( constant *);

/* node to constant conversion */
constant *add_constant_array( parsenode *);

constant *math_evaluate_array( constant *, constant *, int );
constant *math_evaluate_array_func( constant *left, int mathop );

constant *math_execute_node_array( parsenode * );

constant *math_execute_array_element( parsenode *, constant * );
constant *math_execute_array_elements( parsenode *, constant * );

int      set_constant_variable_array_element( variable *, constant *, constant * );

#endif
