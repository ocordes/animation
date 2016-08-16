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

/* output_ffmpeg.h

   written by: Oliver Cordes 2013-01-11
   changed by: Oliver Cordes 2016-08-14

   $Id: ffmpeg.h 352 2013-01-11 20:19:33Z ocordes $

*/

#ifndef __output_ffmpeg_h

#define __output_ffmpeg_h 1

int  ffmpeg_start( int );

int  ffmpeg_out_func( void );

void ffmpeg_done( void );

#endif
