/*!\file iteration.cc
  \brief Program that demonstrates various iteration constructs
*/

#include <iostream>
#include <cstring>
using namespace std;

#define MAX 10

int main (int argc, char ** argv)
{
  int i, j;

  for(i=0, j=0; i < MAX; i++)
    {
      cout << "IN LOOP:i=" << i << endl;
      cout << "IN LOOP:j=" << j << endl;
      ++j;
    }

  cout << "AFTER LOOP:i=" << i << endl;
  cout << "AFTER LOOP:j=" << j << endl;

  //////////////////////////////////////////////

  //array copying using pointers.
  char s1[MAX] = "012345678";
  char s2[MAX];

  //set the contents of s2 to all '\0' characters.
  memset(s2, 0, MAX); 

  char *p1 = s1, *p2 = s2;

  //copy the contents of one array into another using pointers.
  while(*p2++ = *p1++);
  cout << "s2=" << s2 << endl;

  //reset the contents of s2 to "null" characters
  memset(s2, 0, MAX); 

  //put something into the first element of s2 so we will be able to
  //print it later on (otherwise a '\0' character is sitting in there
  //from the memset we just did).
  s2[0] = '9';

  //reset the pointers to point to be beginning of each array
  //respectively.
  p1 = s1, p2 = s2;

  //copy from the second element of s1 thru to the last element of s1
  //(including terminating 0) into the second element of s2 thru the
  //last element of s2.
  while(*++p2 = *++p1);
  cout << "s2=" << s2 << endl;


  //ERROR
  /* This is a programming error. We copy the contents of p1 from the
  first to the last element (MAX size) including the null terminator
  (\0) but the size of the array (s2) that p2 points to is only MAX
  length. The issue here is that we are starting our copy from the
  second element of s2 (s2[1]) and copying MAX more characters into
  the contiguious space respectively.

  Due to boundry lines on most modern computers this will most likely
  not produce a runtime error unless resources are low. At any rate we
  are "poking" a char into a memory address that is not managed my our
  program. One way of fixing this problem might be to increase the
  size of the array s2 or provide some sort of range checking.
  */
  while(*++p2 = *p1++);
  cout << "s2=" << s2 << endl;

  cout << "sizeof(s2)="<<sizeof(s2)<<endl;
  cout << "sizeof(p2)="<<sizeof(p2)<<endl;

  return 0;
}
