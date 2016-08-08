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

/* controls.c

   written by: Oliver Cordes 2013-03-29
   changed by: Oliver Cordes 2013-03-30

   $Id: controls.c 420 2013-03-30 17:58:04Z ocordes $

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <math.h>

#include <assert.h>

#include "config.h"

#include "controls.h"
#include "helpers.h"
#include "output.h"
#include "parsetree.h"
#include "project.h"


/* control setting routines */
void controls_add_control( parsenode *sname )
{
  char       *name;

  controlsdef *newcontrols;

  assert( main_project != NULL );
  assert( sname != NULL );

  name = get_string_from_node( sname );


  newcontrols = malloc( sizeof( controlsdef ) );
  newcontrols->name = strdup( name );

  if ( main_project->nrcontrols < max_controls )
    {
      output( 1, "Adding new control `%s`\n", newcontrols->name );
      main_project->controls[main_project->nrcontrols] = newcontrols;
      main_project->nrcontrols++;
    }
  else
    {
      fprintf( stderr, 
	       "Too much control definitions (limit=%i)! Ignoring new control '%s'!\n",
	       max_blocks, newcontrols->name );
    }

  nfree( name );
}


void controls_free_control( controlsdef *controls )
{
  nfree( controls->name );

  nfree( controls );
}


int controls_verify( controlsdef *controls, blockdef *blocks[], int nrblocks )
{
  int i;

  for (i=0;i<nrblocks;i++)
    {
      output( 10, "%s <-> %s\n", blocks[i]->name, controls->name );
      if ( strcmp( blocks[i]->name, controls->name ) == 0 )
	return 0;
    }
	     
  fprintf( stderr, "Error:: Can't find any block with control name '%s'!\n", controls->name );
  return 1;
}
