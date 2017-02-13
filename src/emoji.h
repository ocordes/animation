/* (C) Copyright 2017-02 by Oliver Cordes
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

/* emoji.h

   written by: Oliver Cordes 2017-02-06
   changed by: Oliver Cordes 2017-02-13

   $Id$

*/


#ifndef __emoji_h

#define __emoji_h 1


/* some old definitions
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_COLOR_RED_BRIGHT     "\x1b[31;1m"
#define ANSI_COLOR_GREEN_BRIGHT   "\x1b[32;1m"
#define ANSI_COLOR_YELLOW_BRIGHT  "\x1b[33;1m"
#define ANSI_COLOR_BLUE_BRIGHT    "\x1b[34;1m"
#define ANSI_COLOR_MAGENTA_BRIGHT "\x1b[35;1m"
#define ANSI_COLOR_CYAN_BRIGHT    "\x1b[36;1m"
#define ANSI_COLOR_COLOR_RESET    "\x1b[39;49m"
*/

/* emoiji definitions */
#define _em_red              0
#define _em_green            1
#define _em_yellow           2
#define _em_blue             3
#define _em_magenta          4
#define _em_cyan             5
#define _em_col_full_reset   6
#define _em_bright_red       7
#define _em_bright_green     8
#define _em_bright_yellow    9
#define _em_bright_blue      10
#define _em_bright_magenta   11
#define _em_bright_cyan      12
#define _em_col_reset        13
#define _em_okay             14
#define _em_printer          15
#define _em_copyright        16


extern char * *_em;

void emoji_init( void );


#endif
