#include "sigslot.h"
#include <iostream>
#include <vector>


class Receiver
{
 public:
  void run1_a() const
  {
    std::cout<<"run1_a"<<std::endl;
  }
  void run2_a(int x) const
  {
    std::cout<<"run2_a:"<< x <<std::endl;
  }
};

class Receiver_B
{
 public:
  void run1_b()
  {
    std::cout<<"run1_b"<<std::endl;
  }
  void run2_b(int x)
  {
    std::cout<<"run2_b:"<< x <<std::endl;
  }
};

void run1_c()
{
  std::cout<<"run1_c"<<std::endl;
}

void run2_c(int x)
{
  std::cout<<"run2_c:"<< x <<std::endl;
}

int main(int argc, char *argv[])
{
  std::shared_ptr<Receiver> t_a(new Receiver);
  std::shared_ptr<Receiver_B> t_b(new Receiver_B);
  auto f_1 = []() -> void {std::cout<<"run1_d"<<std::endl;};
  auto f_2 = [](int x) -> void {std::cout<<"run2_d:"<<x<<std::endl;};

  sigslot::Signal<> sig;
  sig.connect(&Receiver::run1_a, t_a);
  sig.connect(&Receiver_B::run1_b, t_b);
  sig.connect(&run1_c);
  sig.connect(f_1);
  sig.invoke();

  sigslot::Signal<int> sig2;
  sig2.connect(&Receiver::run2_a, t_a);
  sig2.connect(&Receiver_B::run2_b, t_b);
  sig2.connect(&run2_c);
  sig2.connect(f_2);
  sig2.invoke(100);

  return 0;
}