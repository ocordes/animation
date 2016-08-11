#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <wand/magick_wand.h>

int main( int argc, char *argv[] )
{
  MagickWand          *im;

  unsigned char       *pixels;

  long                 width, height;

  MagickPassFail       result = MagickPass;

  /* start here */
  InitializeMagick( NULL );

  width  = 1024;
  height = 768;

  pixels = (unsigned char*) calloc( (width * height * 3), 1);

  if ( pixels == NULL )
    printf( "Memory allocation failed!" );
  else
  {
    printf( "MagickPass=%i MagickFail=%i\n", MagickPass, MagickFail );
    im = NewMagickWand();

    if ( im == NULL )
      printf( "NULL pointer\n" );

    //result = MagickReadImage( im, "olli.jpg" );
    MagickSetImagePage( im, width, height, 0, 0 );
    result = MagickSetImagePixels( im, 0, 0, width, height, "RGB", CharPixel, pixels );
    free( pixels );

    printf( "MagickSetImagePixels result: %i\n", result );


    result = MagickWriteImage( im, "test.png" );

    printf( "MagickWriteImage result: %i\n", result );
  }

  DestroyMagick();
  return 0;
}
