#include <future>
#include <iostream>
#include <string>

struct X {
  void foo(int num, const std::string& str) {
    std::cout << __func__ << ": X address is " << this << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << __func__ << ": num is " << num << ", str is " << str 
              << std::endl;
  }

  std::string bar(const std::string& str) {
    std::string result(__func__);
    result += " ";
    result += str;
    std::cout << __func__ << ": X address is " << this << std::endl;
    return result;
  }
};

struct Y {
  double operator()(double num) {
    std::cout << __func__ << ": Y address is " << this << std::endl;
    return num + 1;
  }
};
