/*!\file threadDeath2.cc
 \brief slightly complex pthread example
  \par Purpose:
  Additional Example program to threadDeath1.cc that creates a thread
  that uses a function that calls a user defined function. Also
  demonstrates casing examples for the pthread_join(void **) return
  type. Often the return type from pthread_join() is confusing so we
  demonstrate only a string * usage here.

  This program is slightly more complicated than threadDeath1.cc
  because we are passing around additional parameters.

  The intention here is that we want to demonstrate passing pointers
  around when creating threads and then accessing those pointers as we
  go allong.
*/
#include <iostream>
#include <string>
#include <pthread.h>

using namespace std;

///user defined function
void *myfunc(void *arg);

/** 
    \brief wrapper function parameters/arguments
    \par Purpose:
    function parameters to be passed to the wrapper function of the
    user defined function.
**/
struct func_params
{
  void *(*user_func)(void *);
  void *argument;
  void *other_info;
};

/**
   \brief A wrapper function for the user defined function

   \par Purpose:
   This function wrapps the user defined function so that additional
   opperations may be performed on a thread once it is
   created. Unified signal handling or thread ID tracking can be
   performed from this function as well as pthread_cleanup_push()
   and pthread_cleanup_pop() operations.

   \return NOTHING calls pthread_exit() to be joined later.

   \param arg is a void pointer that is expected to point to a
   func_params object.
*/
void *func(void *arg)
{
  //grab the parameters for redability (demonstration purposes)
  struct func_params *params = (struct func_params *)arg;

  //print out other info from the paramater structure
  cout << (char *)params->other_info << endl;

  //call the user function with the intended argument (void *)
  void *return_value = params->user_func(params->argument);

  //exit thread
  pthread_exit(return_value);

  /** \note This function should never return -it calls pthread_exit()
      instead
  */
  return NULL;
}

/**
  \par Purpose:
  Demonstrate the use of a wrapper function to call a user defined
  function from our wrapper function. A pointer to the user defined
  function is passed to the wrapper function through a func_params
  object. Casting is used to pass the func_params object through
  pthread_create to the wrapper function.
  <br>
  <br>
  The func_params object also contains a pointer to an argument that
  will be passed on to the user defined function.
*/
int main(int argc, char *argv[])
{
  //track the thread ID
  pthread_t tid;

  //capture the return value of our user defined function (ultimately
  //from a call from pthread_exit in our wrapper function).
  void **ret;

  //create a string to pass to our user defined function
  string arg("string from main");
  
  //create a func_params object to pass to our wrapper function
  struct func_params params;

  //set the members of the func_params object
  params.user_func = myfunc;	// pointer to user defined function
  params.argument = &arg;	// object passed to user function

  //more data -for demonstration 
  params.other_info = (void *)"literal string in main params"; 

  //create the thread (passing our wrapper function as the function
  //argument)
  pthread_create(&tid, (pthread_attr_t *)NULL, func, (void *)&params);

  //join the thread -simple enough
  pthread_join(tid, ret);

  //check the return value 
  if(*ret != NULL)
    {
      //if the contents of the pointer to a pointer (ret) is not NULL...

      //print the contents -casting it as what we expect it to be
      cout << *((string *)(string *)*ret) << endl;

      //delete the pointer because we don't want memory leaks
      delete ((string *)(string *)*ret);
    }

  //arg falls out of scope and is handled by the string destructor
  //after return.

  //exit normally
  return(0);
}

/**
   \par Purpose:
   A user defined function. This function, for demonstration purposes,
   dynamically creates a string object that will be returned through a
   call to pthread_join() later on.

   \note This function SHOULD NOT call pthread_exit() as it is
   intended, in this case, to return to the wrapper function
   func(). Calling pthread_exit() from here will terminate the thread
   and will result in "undefined behavior".

   \param arg is a string object pointer passed in as a void *

   \return a string object that is dynamically allocated
*/
void *myfunc(void *arg)
{
  //print out the argument passed in
  cout << *(string *)arg << endl;
  
  //allocate for a new string object
  string *s = new string("string from myfunc");

  //display our string: for demonstration
  cout << "myfunc returning:" << *s <<endl;

  /** \warning DO NOT call pthread_exit */
  //pthread_exit((void *)s);	// BAD, BAD, BAD

  //return the object as a void pointer
  return( (void *)s);
}
