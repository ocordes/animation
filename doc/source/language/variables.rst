Constants / Variables
========================

Constants
---------------

.. index:: Constants

Animation has some predefined constants, e.g. PI. The constants can be
addressed like normal variables, e.g.  *$PI*.

.. index:: PI

.. list-table::
   :widths: 15 50
   :header-rows: 1

   * - Name
     - Description
   * - PI
     - the PI constant



Variables
-----------------

.. index:: Variables

For the scripting commands inside animation blocks it is sometimes useful to have variables defined, which can be used different purposes, e.g. geometry calculations. *animation* doesn't need a variable type. Variable names have only capital characters. Numbers are allowed but not as a starting character.
The definition of a variable starts with the first assign command: ::

   ...
   FOO = 1
   ...

Afterwards *$FOO* can be used as a referenz to this variable, e.g. ::

  ...
  print $FOO
  ...

Some examples for good and wrong variable names: ::

  ...
  Foo = 1           # lower case characters
  0FOO = 1          # variable name starts with a number
  FOO0 = 1          # ok
  ...

internal Variables
++++++++++++++++++++++

*Animation*  creates a lot of internal variables globally and for blocks only. Most of them refers to the current images  (e.g. geometry):

.. index:: Variables (internal)
.. index:: FRAMENR
.. index:: MTIME.FRAME, MTIME.SEC, MTIME.MIN, MTIME.HOUR
.. index:: BACKGROUND, GEOMETRY.X, GEOMETRY.Y, FILE, IMAGENR, MAGE.X, MAGE.Y, MAXFRAMES

.. list-table::
   :widths: 15 10 50
   :header-rows: 1

   * - Name
     - Place
     - Description
   * - *FRAMENR*
     - global
     - global number of the frame
   * - *MTIME.FRAME*
     - global
     - absolute time (frame part)
   * - *MTIME.SEC*
     - global
     - absolute time (seconds part)
   * - *MTIME.MIN*
     - global
     - absolute time (minutes part)
   * - *MTIME.HOUR*
     - global
     - absolute time (hour part)
   * - *BACKGROUND*
     - global
     - background color
   * - *GEOMETRY.X*
     - global
     - output frame size (x dimension)
   * - *GEOMETRY.Y*
     - global
     - output frame size (y dimension)
   * - *IMAGE.X*
     - global
     - current image size (x dimension)
   * - *IMAGE.Y*
     - global
     - current image size (y dimension)
   * - *FILE*
     - block
     - name of the current input file
   * - *IMAGENR*
     - block
     - number of the current image in a block
   * - *MAXFRAMES*
     - block
     - number of the total amount of images in a block


global Variables
+++++++++++++++++

.. index:: Variables (global)

Global variables are similar to constants. They are also readonly.
The declaration is always outside any block definition: ::

  global FOO = 1


Inside the block the variable can be referenced like other variables.
