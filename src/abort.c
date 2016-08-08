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

/* abort.c

   written by: Oliver Cordes 2010-08-01
   changed by: Oliver Cordes 2010-08-01

   $Id: abort.c 130 2010-08-01 21:38:45Z ocordes $

 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>

#include "abort.h"


char *aabort_msg( int type )
{
	static char dummy[1000];

	switch( type )
	{
		case abort_msg_ok:
			snprintf( dummy, 1000, "OK." );
			break;
		case abort_msg_malloc:
			snprintf( dummy, 1000, "Can't allocate memory" );
			break;
		case abort_msg_null:
			snprintf( dummy, 1000, "NULL pointer detected" );
			break;
		default:
			snprintf( dummy, 1000, "Unknown msgtype=%i", type );
			break;
	}

	return dummy;
}


void aabort( int type, char *format, ... )
{
	char dummy[1000];

	va_list ap;

	va_start( ap, format );
	vsnprintf( dummy, 1000, format, ap );
	va_end( ap );

	fprintf( stderr, "Program abort: %s (%s)!\n", aabort_msg( type ), dummy );

	exit( 1 );
}
