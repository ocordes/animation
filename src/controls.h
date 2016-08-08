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

/* controls.h

   written by: Oliver Cordes 2013-03-29
   changed by: Oliver Cordes 2013-03-30

   $Id: controls.h 420 2013-03-30 17:58:04Z ocordes $

*/

#ifndef __controls_h

#define __controls_h 1

#include "blocks.h"
#include "parsetree.h"



#define max_controls 200


typedef struct{
  char      *name;
  blockdef  *block;
} controlsdef;


/* control setting routines */
void       controls_add_control( parsenode *sname );
void       controls_free_control( controlsdef *controls );

int        controls_verify( controlsdef *controls, blockdef *blocks[], int nrblocks );

#endif
