#include "sigslot.h"
#include <iostream>
#include <vector>


class Test
{
 public:
  void run()
  {
    std::cout<<"Got a signal."<<std::endl;
  }
  void run2()
  {
    std::cout<<"Got a signal-2."<<std::endl;
  }
  void run3(int x)
  {
    std::cout<<"Got a signal-3.  "<< x <<std::endl;
  }
};

int main(int argc, char *argv[])
{
  std::shared_ptr<Test> t(new Test);

  sigslot::signal<Test> sig;
  sig.connect(t, &Test::run);
  sig.connect(t, &Test::run2);
  sig.invoke();

  sigslot::signal<Test, int> sig2;
  sig2.connect(t, &Test::run3);
  sig2.invoke(100);

  return 0;
}