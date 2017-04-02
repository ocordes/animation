Data types in animation
====================================

animation does not have predifined data types for variables like C or other
languages. animation assigns data types from constants or from function results,
which also means that variable can change their data types during execution, e.g. ::

   ...
   C = 1               # C has the type integer
   ...
   C = "Hello world!"  # C is now a string
   ...


Basic types
----------------

Integers
++++++++++++++

.. index:: Integer

Integers are used as "signed integers" in the C int format which is 64bit wide under
Linux and Mac OSes.

Floats
+++++++++++++

.. index:: Float

Floats are stored as double constants. Exponential descriptions are supported, mostly
everything which can be used with "atof" in standard C library.

Strings
+++++++++++++

.. index:: String

Strings are simply identical in what C provides. String concatination is not supported!

Bools
+++++++++++++

.. index:: Bool

TRUE and FALSE ;-)


Complex types
----------------

Points
+++++++++++++++

.. index:: Point

Points are vectors of the size 2; the elements are floats. The syntax is as follows: ::

   ...
   C = ( 0.1, 1.2 )
   D = ( 1, 2)       # OK, but identical to
   D = ( 1.0, 2.0 )
   ...

Arrays
++++++++++++++

.. index:: Array

Arrays are list of constant all of the same type. The major drive for the
implementation was to have an array of points to draw a polygon! But of course,
there are several other use caes. ::

    ...
    A = [ 0, 1, 2 ]                  # array of Integers
    B = [ 0, 1.0 ]                   # wrong, different types
    C = [ (0.0, 0.0) , ( 1.0, 1.0) ] # Array of Points
    ...

Arrays can be accessed element wise or as a sub-array: ::

   ...
   A = [ 0, 10, 20]
   B = $A[1]        # B = 10
   C = $A[0:1]      # C = [ 0, 10 ]
   ...

Individual elements can also be changed : ::

  ...
  A = [ 0, 10, 20 ]
  A[1] = 100        # A = [ 0, 100, 20 ]
  ...

Automatic type conversion
-------------------------------

.. index:: float()
.. index:: int()

animation does an internal type conversion whenever it is necessary. The best
exmaple is some math operation which mixes integers and floats. The result is
then stored as float: ::

   ...
   A = 1 + 2 * 3.0    # A = 7.0
   ...

Sometimes it is necessary to control the type conversion, e.g. ::

  ...
  A = 7 / 5   # A = 1
  ...

gives not the result you want to have. The following conversion fixes this problem: ::

  ...
  A = float( 7) / 5    # correct: A = 1.4
  ...

On the other hand if you want to round a float to integer, you can use the function int(): ::

  ...
  A = int( 1.2 )      # A = 1
  B = int( 4.7 )      # B = 5
  ...
