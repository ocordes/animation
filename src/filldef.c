/* (C) Copyright 2010-2017 by Oliver Cordes
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


/* filldef.c

  written by: Oliver Cordes 2017-03-04
  changed by: Oliver Cordes 2017-03-04

  $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "config.h"

#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif

#include "abort.h"
#include "helpers.h"
#include "filldef.h"

#include "output.h"
#include "parsetree.h"
#include "project.h"


#define          new_increment_pendefs 5
pendef_descr   **pendefs;
int              nr_pendefs;
int              max_pendefs;
pendef_descr    *current_pendef = NULL;

#define          new_increment_filldefs 5
filldef_descr  **filldefs;
int              nr_filldefs;
int              max_filldefs;
filldef_descr   *current_filldef = NULL;


/* internal functions */

/* module init/done functions */
void filldef_init( void )
{

}


void filldef_done( void )
{

}
