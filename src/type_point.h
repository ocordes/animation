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


/* type_point.h

written by: Oliver Cordes 2017-02-25
changed by: Oliver Cordes 2017-02-25

$Id$

*/

#ifndef __type_point_h

#define __type_point_h 1

#include "config.h"

#include "variable.h"
#include "parsetree.h"

/* point node definitions */
parsenode *add_node_point( parsenode *x, parsenode *y  );

/* type conversion */
Point   get_point_from_constant( constant *con );


/* math operations */
constant *math_evaluate_point( constant *left,
			                         constant *right,
			                         int mathop );
constant *math_evaluate_point_func( constant *left,
                               			int mathop );


#endif
