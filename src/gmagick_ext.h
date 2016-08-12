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

/* gmagick_ext.h

   written by: Oliver Cordes 2016-08-12
   changed by: Oliver Cordes 2016-08-12

   $Id: magick.h 687 2014-09-14 17:53:49Z ocordes $

*/


#ifndef __gmagick_ext_h

#define __gmagick_ext_h 1




#include <wand/magick_wand.h>

WandExport unsigned int MagickConstituteImage(MagickWand *wand,
  const unsigned long columns,
  const unsigned long rows,const char *map,const StorageType storage,
  unsigned char *pixels);


#endif
