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

/* macro.h

   written by: Oliver Cordes 2014-08-31
   changed by: Oliver Cordes 2016-08-24


*/


#ifndef __macro_h


#define __macro_h 1


#include "parsetree.h"


#define max_macros 1000

typedef struct{
  char      *name;
  parsenode *commands;
  parsenode *varargs;
  variables *vars;
} macrodef;


extern macrodef *current_macro;


/* macro setup reoutines */

void macro_new_macro( parsenode *macroname, parsenode *varargs );
void macro_add_commands( parsenode *commands );
void macro_free_macro( macrodef *macro );

#endif
