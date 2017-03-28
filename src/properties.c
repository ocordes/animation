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


/* properties.c

written by: Oliver Cordes 2017-03-28
changed by: Oliver Cordes 2017-03-28

$Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#include "amath.h"
#include "output.h"
#include "parsetree.h"
#include "type_array.h"
#include "variable.h"


/* parsenode definitios */

parsenode *add_node_property_element_variable( parsenode *variable, parsenode *element )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_property_variable;

  newnode->left  = variable;
  newnode->right = element;

  return newnode;
}


parsenode *add_node_property_element_definition( parsenode *definition, parsenode *element )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_property_definition;

  newnode->left  = definition;
  newnode->right = element;

  return newnode;
}



/* property get/set functions */


constant* execute_property_variable( parsenode* variable, parsenode* element)
{
  constant *con;

  con = add_constant_string( "variable" );

  return con;
}


constant* execute_property_definition( parsenode* definition, parsenode* element )
{
  constant *con;

  con = add_constant_string( "defintion" );

  return con;
}
