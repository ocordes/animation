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

/* abort.h

   written by: Oliver Cordes 2010-08-01
   changed by: Oliver Cordes 2010-08-01

   $Id: abort.h 130 2010-08-01 21:38:45Z ocordes $

 */

#ifndef __abort_h

#define __abort_h

/* abort message type */

#define abort_msg_ok     0
#define abort_msg_malloc 1
#define abort_msg_null   2


void aabort( int type, char *format, ... );

#endif
