/*!\file threadDeath1.cc
  \brief basic thread functionality
  \par Purpose:
  Example program that creates a thread that uses a function that
  calls a user defined function. Also demonstrates casing examples for
  the pthread_join(void **) return type. Often the return type from
  pthread_join() is confusing so we demonstrate a char* or a string*
  depending on whether CHAR_EXAMPLE is defined within this file (see
  below).

  The intention here is that we want to demonstrate passing pointers
  around when creating threads and then accessing those pointers as we
  go allong.
*/

#include <pthread.h>
#include <iostream>
#include <string>

using namespace std;

//##########################################################
///make this a define to use the char * example
#define CHAR_EXAMPLE 
//##########################################################

///user defined function to call
void *myfunc(void *arg)
{
  /** \return some object from the heap
      \param some object
  */

  //print whatever is in arg as a string 
  /** \note this function assumes that arg is a string pointer passed
  in from main -for demonstration purposes
  */
  cout << *(string *)arg << endl;
  
#ifdef CHAR_EXAMPLE
  //demonstrate a char * return
  char *c = new char[256];
  memset(c, 0, 256);
  strcpy(c,"string from myfunc");
  cout << "myfunc returning:" << c <<endl;

  /** \warning do NOT call pthread_exit from here! This function will
  //behave the same if it is called directly or not and a returning
  normally does not affect the return_value parameter of a
  pthread_join().
  */
  //pthread_exit((void *)c);

  return( (void *)c);

#else
  //demonstrate a string * return.
  string *s = new string("string from myfunc");
  cout << "myfunc returning:" << *s <<endl;

  /* DO NOT CALL pthread_exit from here */
  //pthread_exit((void *)s); 

  return( (void *)s);
#endif
}

///a function that calls the user defined function
void *func(void *arg)
{
  /**
     \par
     See myfunc for more details. This function does, however use
     pthread_exit() to demonstrate the passing of the return_value
     pointer back through pthread_join() in main
  */
  void *return_value = myfunc(arg);
  pthread_exit(return_value);
}

int main(int argc, char *argv[])
{
  //a thread id
  pthread_t tid;

  //return var from a pthread_exit()/pthread_join()
  void **ret;

  //a string to pass to myfunc()
  string arg("string from main");

  //##########################################################
  //##########################################################
  cout << "example 1:" << endl;
  //##########################################################

  //call a function tha calls the function
  pthread_create(&tid, (pthread_attr_t *)NULL, func, (void *)&arg);
  
  //join the thread
  pthread_join(tid, ret);

#ifdef CHAR_EXAMPLE
  //demonstrate a char * example
  char *pc = ((char *)(char *)*ret);
  cout << pc << endl;
  delete pc;
#else
  //demonstrate a string * example
  string *ps = ((string *)(string *)*ret);
  cout << *ps << endl;
  delete (string *)ps;
#endif

  //##########################################################
  //##########################################################
  /* Same type of example as above: this one just calls the function
  directly and uses more direct casting when accesing the return
  value from the function.
  */
  //##########################################################
  cout << "\n" << "example 2:" << endl;
  //##########################################################

  //creat thread 
  pthread_create(&tid, (pthread_attr_t *)NULL, myfunc, (void *)&arg);

  //join thread
  pthread_join(tid, ret);

#ifdef CHAR_EXAMPLE
  //access char through casts
  cout << ((char *)(char *)*ret) << endl;
  delete ((char *)(char *)*ret);
#else
  //access string through casts
  cout << *((string *)(string *)*ret) << endl;
  delete ((string *)(string *)*ret);
#endif

  return 0;
}

