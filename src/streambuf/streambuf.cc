/*!\file streambuf.cc
  \brief Demonstration of streambuf inheritance

  \par Purpose:
  Program that demonstrates inheriting from streambuf to act
  just like cout. The program comes from chapter 13 of "The C++
  Standard Library" (Josuttis). 
  
  Also demonstrates the values of various types of variables when
  instantiated.
*/

#include <iostream>

///Demonstration of streambuf inheritance
class dmsg : public std::streambuf
{
public:
  int i;
  int *p;
public:
protected:
  /**
     \param c character from input (<<)

     \par 
     Function override that evaluates each character the stream
     operator (<<) recieves. This is a necessary function to print to
     the output
  */
  virtual int_type overflow(int_type c)
  {
    if(c != EOF)
      {
	//! uses C function putchar() for output
	if(putchar(c) == EOF)
	  return EOF;

	std::cout << "|";
      }
    return c;
  }
};


int main(int argc, char **argv)
{
  dmsg X;
  std::ostream out(&X);

  out << "xxx " 
      << 123
      << " yyy"
      << std::endl;

  std::cout << "blah" << std::endl;


  /* demonstrate compound input from a stream
     (for example type "2xx4y [enter]" at the prompt)
  */
  int i;
  char ch;
  std::cout << std::endl;
  std::cout << "i="<< i << "|ch="<<ch<<std::endl;
  std::cout << "prompt:";
  std::cin >> i >> ch;
  std::cout << "i="<< i << "|ch="<<ch<<std::endl;

  /* demonstrate the contents of variables when they are created
   */
  dmsg *msg_p = new dmsg();

  if(msg_p->p == NULL)
    std::cout << "msg_p->p is NULL" << std::endl;

  if(msg_p->i == 0)
    std::cout << "msg_p->i is ZERO" << std::endl;

  delete msg_p;


  int *i_p;

  if(i_p)
    std::cout << "i_p is VALID" << std::endl;

  if(i_p == NULL)
    std::cout << "i_p is NULL" << std::endl;
  else
    if(i_p == 0)
      std::cout << "i_p is ZERO" << std::endl;

  i_p = new int;

  if(*i_p == 0)
    std::cout << "*i_p is ZERO" << std::endl;
  else
    std::cout << "*i_p=" << *i_p <<std::endl;

  delete i_p;
}
