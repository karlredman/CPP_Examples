/** \file overload.cc
 * \brief simple overloading example
*/

#include <stdio.h>

/*
  this won't compile -can't overload functions by return type. The
  compiler sees this as ambiguious.
*/

int thing1()
{
  printf( "thing1\n");
  return 1;
}

int thing1(int i)
{
  printf( "thing1a\n" );
  return 0;
}

double thing2()
{
  printf( "thing2\n");
  return 'x';
}
double thing2(int i)
{
  printf( "thing2a\n" );
  return 0;
}



int main(int argc, char **argv)
{
  int x;
  double y;

  x = thing1();
  x = thing1(1);

  y = thing2();
  y = thing2(1);
}
