/*!\file virtuals.cc
 \brief basic virtual function / class usage
*/	

#include <iostream>

using namespace std;

///A basic abstract Class (that is inherited from)
class AbstractClass
{
public:
  //demanding polymorphic capability
  virtual void print() = 0;
};

///class that inherits from AbstractClass
class BaseClass : public AbstractClass
{
public:
  //allowing polymorphic behavior from derived classes
  //the virtual keyword is superfluious here
  virtual void print()
  { std::cout << "base class" << std::endl; }
};

///class that inherits from BaseClass -> AbstractClass
class ChildClass : public BaseClass
{
public:
  //taking advantage of polymorphism by overriding parrent function
  void print()
  { std::cout << "child class" << std::endl; }
};

///another example of ChildClass (inherits from BaseClass)
class AnotherChildClass : public BaseClass
{
public:
  //do NOT take advantage of polymorphism
  //void print()
  //{ std::cout << "child class" << std::endl; }
};

///print something
void callPrint(AbstractClass& p)
{
  //call the appropriat function (based on the actual type of
  //BaseClass -be it derived or not)
  p.print();
}

///demonstrate virtual functions and abstract classes
int main(int argc, char **argv)
{
  BaseClass b;
  ChildClass c;
  AnotherChildClass a;

  //concrete behavior
  callPrint(b);

  //polymorphic behavior
  callPrint(c);

  //concrete (hiarchial) behavior
  callPrint(a);
}
