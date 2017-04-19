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

/* execute.h

   written by: Oliver Cordes 2010-07-02
   changed by: Oliver Cordes 2017-04-19

   $Id$

*/

#ifndef __execute_h
#define __execute_h 1

#include "parsetree.h"

#define return_ok     0
#define return_break  1
#define return_exit   2
#define return_quit   3
#define return_return 4
#define return_fail   5


extern constant  *return_value;

int execute_check( void );
int execute_execute( void );

int execute_cmd_macro( parsenode* );

void process_image_setup( char* );

#endif
