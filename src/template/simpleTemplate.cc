/** \file simpleTemplate.cc
  \brief Simple template examples.
*/

#include <iostream>
#include <string.h>

/** 
    \brief Template function to copy a string literal

    \author Karl N. Redman

    \param a array 1
    \param b array 2
    \param size total size of the array
    \param numElements used for printing the array contents (cheezy)

    \return count of elements copied or -1 on error. Failure to pass an
    array to this function is undetermined.

    \par Purpose:
    Template that copies a string literal or any array upto the point
    where the contents of the array element is equal to 0 ('\0').

    \note
    The arrays are expected to be the same size. 

    \warning
    This template has very little practical application. This is
    merely here to demonstrate a simple Template function
*/
template <class T> int Tmycpy( T a, T b, size_t size, int numElements)
{
  //set the number of elements copied to the initial 1 (an unconditional
  //copy is performed later on)
  int count = 1;

  //print out the array using the numElements
  std::cout << "INITIAL ARRAY CONTENTS: " << "a=" ;
  for(int i=0; i < numElements; i++)
    std::cout << a[i];

  std::cout << "|b=";

  for(int i=0; i < numElements; i++)
    std::cout << b[i];
  
  std::cout << "|"
	    << "size=" << size
	    << "|"
	    << "numElements=" << numElements
	    << std::endl;

  //range check the arrays
  //if( ! sizeof(b) >= sizeof(a))
    //return -1;

  //create a couple of pointers (assuming we can -if we passed a
  //reference we might be in trouble --> type checking and
  //specialization might be a better option for this template...)
  T p1 =a;
  T p2 =b; 

  //do an unconditional copy until a NULL Terminator is found
  //(inclusive) and count the elements copied
  while(*p2++ = *p1++)
    count++;

  //compare the arrays
  if(memcmp(static_cast<void *>(a), static_cast<void *>(b), size) != 0)
    return -1;

  //return number of elements copied
  return count;
}
  
int main(int argc, char *argv[])
{
  //string literal copy to a character array

  char ca1[] = {"123456789"};
  char ca2[sizeof(ca1)];	// create an array that is the size of
				// ca1.

  memset(ca2, 'x', sizeof(ca2)); // set the contents of ca2 to a bunch
				 // of char 'x' so we can "watch" what
				 // happens.

  //call our template function
  int i = Tmycpy<char *>(ca1, ca2, sizeof(ca2), sizeof(ca2)/sizeof(char));

  //check the data
  if(i != -1)
    std::cout << "SUCCESS: " 
	      << "ca1=" << ca1
	      << "|" 
	      << "ca2=" << ca2 
	      << "|" 
	      << "elements copied=" << i
	      << std::endl;

  std::cout << "#############################" << std::endl;

  // int array copy (upto a found \0)
  int ia[] = {9,8,7,6,5,4,3,2,1,0};
  int ib[] = {0,0,0,0,0,0,0,0,0,4}; // initialize to all \0 and a 4 at
				    // the end -for range checking
				    // demonstration purposes in the
				    // template

  //call our template function to make the copy
  i = Tmycpy<int *>(ia, ib, sizeof(ib), sizeof(ib)/sizeof(*ib));
						    
						   
  //check status and display contents
  if(i != -1)
    std::cout << "SUCCESS: int array works too" 
	      << "|" 
	      << "elements copied=" << i
	      << std::endl;

  std::cout << "SECOND ARRAY CONTENTS: " << "ia=" ;
  for(int j=0; j < i; j++)
    std::cout << ia[j];

  std::cout << "|ib=";

  for(int j=0; j < i; j++)
    std::cout << ib[j];
  
  std::cout << std::endl;

  std::cout << "#############################" << std::endl;

  //return success
  return 0;
}
