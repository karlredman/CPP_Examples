/** \file threadDeath3.cc

\brief Thread Management Class (complex)

\par Purpose:
Example class that manages threads and the detection of their
exits (death) through the use of condition variables.
Please see the example code and ThreadMgr class documentation for
more details.
<br>
<br>
For more fundamental examples see documentation for files
threadDeath1.cc and threadDeath2.cc.
<br>
<br>
The class ThreadMgr uses a map and a stack from the C++ STL to
track thread IDs and their runtime status. While ThreadMgr does add
some overhead to basic pthread management (in the form of some
memory usage and slightly slower thread creation and destruction)
the class offers a relatively easy to use interface to pthreads in
general and may be enhanced for better type recognition (i.e. making
it a template) and the like.
*/

#include <iostream>
#include <stack>
#include <map>
#include <algorithm>
#include <string>
#include <pthread.h>

/** 
    \brief A basic thread management class

    \author Karl N. Redman (parasyte@sleepingstill.com)

    \par Purpose:
    This is an example class intended to be used as a guidline for the
    work required to handle pthreads where the main process (the
    parent of the initial threads) waits on a condition variable for
    the threads to terminate. The functions that are specified for the
    pthread_create are user defined. <br>
    <br>
    This class should be pretty fast and relatively easy to use for
    most general purpose thread programming. In general, it's a good
    place to start.
    <br>
    <br>
    Basic pthread "voidness" has been retained for demonstration
    purposes. It is concevable that this class could be instantiated
    from and / or altered to make the user interface easier to work
    with (in other words -we could get rid of much of the user end
    casting through inheritance).

    \note
    This class is not intended for use by detached threads unless some
    of the functions are overridden. Also, the use of long casting was
    used in favor of readability in the interests of speed.
*/
class ThreadMgr {
private:
  ///structure for static wrapper function arguments
  struct func_arguments
  {
    ///pointer to user defined function
    void *(*func)(void *);

    ///function to call when we are canceled
    void (*cancel_func)(void *);

    ///user arguments for user function
    void *arg;

    ///the this object -one per thread...EEEEK!
    ThreadMgr *thisObject;	//! probably redundant. 
  };
  
public:
  ///constructor
  ThreadMgr() 
  { 
    /** \note this function instantiates static pthread_x_t variables
	for mutexes and a condition variable
    */

    //the data mutex
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
    m_mutex = &mutex; 

    //the condition variable mutex
    static pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
    m_cond_mutex = &cond_mutex;

    //the condition variable
    static pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
    m_cond_var = &cond_var;
  }

  ///cancel a thread
  int cancel_thread(pthread_t *tid)
  {
    /**
       \return return value of a pthread_cancel() for parameter tid
       \param tid pointer to the thread id to kill
    */
    /** \warning
	If the thread has allocated dynamic memory and has been canceled
	this class does not compensate. In other words, never cancel a
	thread (with this class) that allocates dynamic memory -you will
	lose your pointer. This is because pthread_cancel does not allow
	you to recieve anything back from a canceled thread. However,
	you could delete your pointer before you retrurn from the thread
	if that is your wish.
    */

    /** \warning
	use of the function pthread_cancel() for threads managed by this
	class may result in loss of dynamic memory pointers, or worse,
	a race condition for m_mutex resulting in a deadlock of the
	threads. Don not use pthread_cancel() for threads managed by
	this class. Use ThreadMgr::cancel_thread() instead.
    */

    /** \todo add more robust thread canceling ability */

    //cancel a thread
    int ret = 0;
    pthread_mutex_lock(m_mutex);	//lock the data mutex 
    m_ids.erase(*tid);			//remove from id map

    //cancel the thread
    ret = pthread_cancel(*tid);
    
    pthread_mutex_unlock(m_mutex);	// unlock data mutex

    return ret; 
  }

  ///wait on a condition variable for a thread to terminate
  int condWait(void **thread_return_val)
  {
    /** 
	\par Purpose: 
	Act like a seamless condition variable. Allows a process to
	block until a thread has terminated and returns the thread
	functions return value through the parameter.

	\param a pointer to the pointer of the threads return value.
	\return pthread_join status from removeTerminated()
    */

    /** \warning
	This function may not be safe for threads that are canceled
    */

    int ret = 0;
    
    //condition variable mutex lock
    pthread_mutex_lock(m_cond_mutex);

    //check predicate
    while(no_threads_terminated())
      {
	//wait on condition variable
	pthread_cond_wait(m_cond_var, m_cond_mutex);
      }


    //remove the thread from the terminated list (handle join)
    ret = removeTerminated(thread_return_val);

    //unlock
    pthread_mutex_unlock(m_cond_mutex);

    //return join status
    return ret;
  }

  ///attempt to create a new thread and register it
  //int createThread( void *(*thread_func)(void *), void *arg)
  pthread_t createThread( void *(*thread_func)(void *), void *arg)
  {
    /** 
	\par Purpose:
	Create a new thread and register it for management by this
	class.

	\return 0 on error, pthread_t thread ID on success

	\param pointer to function to run as thread, pointer to
	argument. [i.e. createThread(myfunc, arg);]

	\note
	This function works by building the an argument list from the
	one provided by the user and some internal stuff in order to
	call an internal function that sets things up for return
	values, etc. and then calls the users function. This adds
	about the size of 8 (roughly) pointers to the memory usage of
	each thread being created -but that's the price for wanting to
	keep track of this stuff generically i guess.
    */
    
    pthread_t tid = 0;		// Id of thread
    int ret_val;		// return value
 
    //arguments for the function
    struct func_arguments *arguments = new func_arguments;

    arguments->func = thread_func;	// users function
    arguments->cancel_func = NULL; 	// NOT IMPLIMENTED
    arguments->arg = arg;		// users argument

    //this is the this pointer (so far, every thread get's one -eek!)
    arguments->thisObject = this;

    //create a new thread
    /*
      default attributes (for now), 
      internal function, func(), calls users function,
      arguments contain other info + user's argument.
    */
    ret_val = pthread_create(&tid, (pthread_attr_t *) NULL, func, (void *)arguments);

    if(ret_val == 0)
      {
	//register the thread and arguments in the ids map
	addID(tid, arguments);
    
	//return thread id
	return tid;
      }
    else
      std::cout << "pthread_create FAIL" << std::endl;


    //return 0 on error
    return 0;
  }
  
  ///return the number of active threads
  int threadsActive()
  { 
    pthread_mutex_lock(m_mutex);
    int ret =  m_ids.size(); 
    pthread_mutex_unlock(m_mutex);
    
    return ret;
  }
    
  ///answers the question "are there no theads terminated?"
  bool no_threads_terminated()
  {
    /** \return boolean of terminated threads in terminate queue
     */
    pthread_mutex_lock(m_mutex);
    bool ret = m_terminated.empty();
    pthread_mutex_unlock(m_mutex);
    return ret;
  }
    


protected:
  ///internal thread function
  static void *func(void *arg)
  {
    /** 
	\par Purpose:
	This function is called from the call to pthread_create()
	within member function createThread. This function handles
	basic thread management issues and is a wrapper around the
	user function -which is called from here as well.

	\return NONE -this function shouldn't return!!! it calls
	pthread_exit().

	\param arg shold be a pointer to a func_arguments structure.
    */

    //convenience this object
    ThreadMgr *thisObject = ((struct func_arguments *)arg)->thisObject;

    //return argument from user function
    void *tmpArg = NULL;

    //basic cancel stuff -should be more robust
    struct func_arguments *cancel_arg = new func_arguments;
    cancel_arg->cancel_func = shutdown_thread;
    cancel_arg->func = NULL;
    cancel_arg->arg = NULL;
    cancel_arg->thisObject = thisObject;

    //set the cleanup function
    pthread_cleanup_push(shutdown_thread, (void *)cancel_arg);

    //call the user's function
    tmpArg = ((struct func_arguments *)arg)->func( ((struct func_arguments *)arg)->arg );
    //std::cout << "func() passing \"" << *(std::string *)tmpArg << "\" to pthread_exit()" << std::endl;

    //delete (struct func_arguments *)arg;

    //pop the cleanup handler off the cleanup stack
    //delete the arg variable list from the createThread function.
    pthread_cleanup_pop(1);
    
    //add this thread to the terminated list
    thisObject->addTerminated(thisObject);

    //exit this thread
    pthread_exit(tmpArg);

    //we will never get here
    return NULL;
  }

  ///add a thread id to the m_terminated stack
  static void *addTerminated(ThreadMgr *arg)
  { 
    //pthread_mutex_lock(((ThreadMgr *)arg)->m_mutex);
    pthread_mutex_lock(arg->m_mutex);

    //add self to list of stuff to be terminated (joined)
    arg->m_terminated.push(pthread_self());

    //broadcast to threads waiting on the condition variable to notify
    //them to wake up
    pthread_cond_broadcast(arg->m_cond_var);

    pthread_mutex_unlock(arg->m_mutex);

    //return NULL -blah
    return NULL;
  }

  ///remove a terminated thread LIFO
  int removeTerminated(void **return_val)
  {
    /**
       \return result of pthread_join

       \param pointer to user function return value pointer

       \note the void **retrun_val is a result of the pthread_join.
    **/
    int ret = 0;
    pthread_t tempID;

    //lock critical section
    pthread_mutex_lock(m_mutex);

    //make sure we have something
    if(!m_terminated.empty())
      {
	//get id from stack
	tempID = m_terminated.top();

	/** \warning the thread is unregistered only if the
	    pthread_join was successfull. This may be a problem down
	    the line (but VERY rare)
	*/

	//join with the terminated thread
	if( (ret = pthread_join(tempID, return_val)) == 0)
	  {
	    //delete the arguments (created in createThread)
	    //delete m_ids.find(tempID)->second;
	    
	    //get rid of the ID from active list
	    m_ids.erase(tempID);
	    
	    //remove id from stack
	    m_terminated.pop();
	  }
	else
	  std::cout << "pthread_join() = " << ret << std::endl;
      }

    //unlock critical section
    pthread_mutex_unlock(m_mutex);

    //return value of pthread_join
    return ret;
  }

  ///add a thread id to the id vector
  void addID(pthread_t id, func_arguments *arg)
  {
    //lock the data mutex
    pthread_mutex_lock(m_mutex);
    
    //add the thread to the std::map
    m_ids.insert(std::make_pair(id, arg));

    //unlock the data mutex
    pthread_mutex_unlock(m_mutex);
  }
  
  ///shutdown a thread from pthread_cleanup_pop().
  static void shutdown_thread(void *arg)
  {
    /** \param arg must be void * per pthread_cleanup_x()
     */
    delete ((struct func_arguments *)arg);
    return;
  }


private:
  ///mutex for data access (m_ids, m_terminated)
  pthread_mutex_t *m_mutex;

  ///mutex for condition variable
  pthread_mutex_t *m_cond_mutex;
  
  ///condition variable
  pthread_cond_t *m_cond_var;

  ///map of all thread ids and function attributes
  std::map<pthread_t, ThreadMgr::func_arguments *> m_ids;

  ///stack of terminated thread ids
  std::stack<pthread_t> m_terminated;
};


//################## PROTOTYPES
///generic wait for string-centric threads
void waitStringThreads(ThreadMgr *mgr, void **return_value);

///Example Thread function
void *myfunc0(void *arg);

///Example Thread function
void *myfunc1(void *arg);

///Example Thread function
void *myfunc2(void *arg);

///Example Thread function returning an object
void *myStringFunc(void *arg);

///Example template -waits for thread and prints object 
template <class T>
void TwaitStringThreads(ThreadMgr *mgr, T return_value);

//################## MAIN
///the main function
int main(int argc, char *argv[])
{
  void **return_val;
  void **ret;

  //string literal for later use
  char *pc = "987654321";

  //string object of later use (example 2)
  std::string *str = new std::string("a string from main");

  //some thread IDs to track
  pthread_t pret, pret2;

  //common variable
  int i = 0;

  //the thread manager class
  ThreadMgr m;

  
  //##########################################################
  std::cout << "Example 1:" << std::endl;
  //##########################################################

  /** \par Example 1:
      A slightly overcomplicated example that attempts to cancel a
      thread. The thread attributes have not been set to be cancelable
      so milage may vary from system to system. The cancelation
      routine is not very robust here -this is just for demonstratio
      purposes to show the kinds of things we might want to do with a
      class implimentation like that of ThreadMgr.
  */

  //test the functionality 
  pret = m.createThread((void *(*)(void *))myfunc0, NULL);
  pret = m.createThread((void *(*)(void *))myfunc2, (void *)pc);
  pret = m.createThread((void *(*)(void *))myfunc1, NULL);
  pret2 = m.createThread((void *(*)(void *))myfunc2, (void *)pc);
  pret = m.createThread((void *(*)(void *))myfunc1, NULL);
  pret2 = m.createThread((void *(*)(void *))myfunc2, (void *)pc);
  pret = m.createThread((void *(*)(void *))myfunc1, NULL);
  pret = m.createThread((void *(*)(void *))myfunc1, NULL);

  while(m.threadsActive())
    {
      /*do, while there are active threads or there are threads being
	terminated.
      */

      //on the third itteration...
      if(i++ == -1)
	{
	  //cancel a thread (may lose data this way though!)
	  std::cout << "CANCELING THREAD:" << pret2 << std::endl;
	  m.cancel_thread(&pret2);
	  
	  //create another thread for fun and profit
	  pret = m.createThread((void *(*)(void *))myfunc2, (void *)pc);
	}
      else
	{
	  //wait on the thread
	  m.condWait(return_val);
	  if(*return_val != NULL)
	    {
	      std::cout << "#######################main:" << ((char *)(char *)*return_val) << std::endl;

	      /* This cast is not entirely proper. See the next
		 example for proper casting when deleting this pointer.
	      */
	      delete (char *)*return_val;
	    }
	}
    }


  //##########################################################
  std::cout << "\n" << "Example 2:" << std::endl;
  //##########################################################

  /** \par Example 2:
      A less complicated implimentation of Example 1. Here we don't
      cancel any threads.
  */

  //test the functionality 
  pret = m.createThread((void *(*)(void *))myfunc0, NULL);
  pret = m.createThread((void *(*)(void *))myfunc2, (void *)pc);
  pret = m.createThread((void *(*)(void *))myfunc1, NULL);
  pret2 = m.createThread((void *(*)(void *))myfunc2, (void *)pc);
  pret = m.createThread((void *(*)(void *))myfunc1, NULL);
  pret2 = m.createThread((void *(*)(void *))myfunc2, (void *)pc);
  pret = m.createThread((void *(*)(void *))myfunc1, NULL);
  pret = m.createThread((void *(*)(void *))myfunc1, NULL);

  while(m.threadsActive())
    {
      /*do, while there are active threads or there are threads being
	terminated.
      */

      //wait on the thread
      m.condWait(return_val);

      /* check return_val: meanwhile other threads may be dieing
       */
      if(*return_val != NULL)
	{
	  /* We should get here 3 times from our usage of myfunc2
	     above */
	  std::cout << "#######################main:" << ((char *)(char *)*return_val) << std::endl;

	  //delete the pointer since it was valid (not NULL)
	  delete ((char *)(char *)*return_val);
	}
    }


  //##########################################################
  std::cout << "\n" << "Example 3:" << std::endl;
  //##########################################################

  /** \par Example 3:
      Demonstrates creating a thread that both passes arguments to the
      user defined function as well as gets something back from the
      call to condWait (ultimately from pthread_join)
  */
  /**
     \note we use a different pointer in the third example to provide a
     cleaner example of returning an object
  */

  /** \warning an attempt to declare void **ret after example 3
   //yielded a consistant segmentation fault using "g++ (GCC) 3.3.3
   20040125" this is considered to be a compler or linker error -I
   didn't bother trying to figure it out (just be aware that these
   things happen sometimes).
  */
  //void **ret;

  //try to pass an object and get one back
  m.createThread(myStringFunc, (void *)str);

  //while(m.threadsActive() || !m.no_threads_terminated())
  while(m.threadsActive())
    {
      //wait on the thread
      m.condWait(ret);
      if(*ret != NULL)
	{
	  //cast the return_val -contents of a std::string pointer
	  std::cout << *((std::string *)(std::string *)*ret) << std::endl;
	  
	  //delete the memory at the pointer
	  delete ((std::string *)(std::string *)*ret);
	}

    }//end while()
  
  std::cout << "threads Active:" << m.threadsActive() << std::endl;
  std::cout << "no_threads_terminated:" << m.no_threads_terminated() << std::endl;

  //cleanup
  delete str;

  //##########################################################
  std::cout << "\n" << "Example 4:" << std::endl;
  //##########################################################

  /** \par Example 4:
      Demonstrates the use of two instances of the ThreadMgr
      class. While some of the functionality of the class uses static
      functions, the thread management of each instance stays with the
      instance. In other words, each instance of ThreadMgr manages it's
      own private list of threads (the threads are managed seperately).
  */
  
  //let's try another instance in tandum
  ThreadMgr m1;

  //create a couple of strings
  std::string *str1 = new std::string("string1 from main");
  std::string *str2 = new std::string("string2 from main");

  //create a thread under m1 instance of ThreadMgr
  m1.createThread(myStringFunc, (void *)str1);

  //create a thread under m instance of ThreadMgr
  m.createThread(myStringFunc, (void *)str2);


  //manage threads from m instance of ThreadMgr
  waitStringThreads(&m, ret);

  //manage threads from m1 instance of ThreadMgr
  TwaitStringThreads(&m1, (std::string **)ret);

  
  //cleanup
  delete str1;
  delete str2;
  
  //exit normally
  return(0);
}

///wait for a managed thread to return
void waitStringThreads(ThreadMgr *mgr, void **return_value)
{
  /**
     \par Purpose:
     Wait for a thread managed by the ThreadMgr pointer to return.

     \param mgr A ThreadMgr class pointer

     \param return_value (must be from the function that calls
     createThread) This is a detail of how the staticness of the
     return value from the pthread_join is implimented -I think.
  */
  /**
     \note This function could easily be made into a template. We're
     just doing this for now for demonstration purposes. Something
     along the lines of the following might do the trick:<br>
     <br>
     template \<class T\>
     void waitStringThreads(ThreadMgr *mgr, T **return_value);
     <br>
     <br>
     (Of coarse, then you would have to cast your pointer from the
     calling function or something like that, but that's probably not
     that big of a deal.)
  */
  /**
     \warning: Due to stack undwinding, declaring the void** for the
     return value in this scope will simply not work. The return value
     pointer MUST be passed in or you will get a segmentation fault.
  */
  //void **return_value;		// doesn't work !!!!

  while(mgr->threadsActive())
    {
      //wait on the thread
      mgr->condWait((void **)return_value);

      if(*return_value != NULL)
	{
	  //cast the return_val -contents of a std::string pointer
	  std::cout << "waitStringThreads:" << *((std::string *)(std::string *)*return_value) << std::endl;
	  
	  //delete the memory at the pointer
	  delete ((std::string *)(std::string *)*return_value);
	}
    }//end while()
}

/**
   \par Purpose:
   Example template that waits for a thread and prints out the string
   that is the return value from the condWait. This is here for
   example only and EXPECTS that the Type is something printable.

   \param mgr A ThreadMgr pointer
   \param return_value of type cast by calling function (see main for
   more details)

   \return nothing

   \note a call to this function: <br>
   TwaitStringThreads(p_mgr, (string **)ret);<br>
   Yields ret to be a (string **). When we want to send it to cout
   then we must cast it bacwards to get the contents of the pointer
   of the pointer (which is a string pointer in this case). Get it?
*/
template <class T>
void TwaitStringThreads(ThreadMgr *mgr, T return_value)
{
  while(mgr->threadsActive())
    {
      //wait on the thread
      mgr->condWait((void **)return_value);

      if(*return_value != NULL)
	{
	  //cast the return_val -contents of a T pointer

	  /* if we pass in a string ** then this translates to: 
	  **(string **)return_value for the call to cout. remember, if
	  return_value is a pointer to an object then we want the
	  contents of the pointed to item of the pointer (it's a pointer
	  to a pointer to a pointer to something)
	  */
	  std::cout << "TwaitStringThreads:" << **((T)return_value) << std::endl;
	  
	  //delete the memory at the pointer
	  delete *return_value;
	}
    }//end while()
}

/**
   \par Purpose:
   Print the parameter from main, allocate memory and populate the new
   memory with data, and return a pointer to the new memory area (of
   type string pointer cast to a void *).
   
   \param arg a void pointer

   \return a string pointer cast to a void pointer
*/
void *myStringFunc(void *arg)
{
  //copy the string using casts
  //std::string *a = ((std::string *)arg);
  //std::cout << "myStringFunc printing:" << *a << std::endl;

  //print the contents of arg as an std::string pointer
  std::cout << "myStringFunc printing:" << *(std::string *)arg << std::endl;
  
  //create a new string to be returned to the calling thread
  std::string *s = new std::string("a string from myStringFunc");
  std::cout << "myStringFunc returning:" << *s << std::endl;

  //cast the object as a void pointer and return
  return ((void *)s);

  //return ((void *)NULL);
}

/**
   \brief Simply counts and returns 
   \return NULL is returned
*/
void *myfunc0(void *arg)
{
  std::cout << "got here: myfunc0:BEGIN" << std::endl;

  for(int i=10000; i > 0; i--)
    for(int i=10000; i > 0; i--);

  std::cout << "got here: myfunc0:END" << std::endl;

  return NULL; 
}

/**
   Simply counts and returns in less time than myfunc0
   \return NULL is returned
*/
   
void *myfunc1(void *arg)
{
  std::cout << "got here: myfunc1:BEGIN" << std::endl;

  for(int i=1000; i > 0; i--)
    for(int i=10000; i > 0; i--);

  std::cout << "got here: myfunc1:END" << std::endl;

  return NULL; 
}

/**
   \par Purpose:
   Allocate some memory and add data to the memory area. Then wait for
   some amount of time (in a for loop) and return a pointer to the
   allocated memory.

   \param a void *
   \return a char * cast to a void *
*/
void *myfunc2(void *arg)
{
  std::cout << "got here: myfunc2:BEGIN" << std::endl;

  //print the value of arg from main
  std::cout << "|arg = " << (char *)arg << std::endl;

  //create a new char and add data to the memory area
  char *tmp = new char[10];
  char x[10] = {"123456789"};

  //cheezy, but whatever...
  memcpy(tmp, x, 10);

  std::cout << "myfunc2:" << tmp << std::endl;

  for(int i=10000; i > 0; i--)
    for(int i=10000; i > 0; i--);

  std::cout << "got here: myfunc2:END" << std::endl;

  /**
     \return a pointer to the new memory memory area that was
     populated earlier
  */
  return tmp;
}
