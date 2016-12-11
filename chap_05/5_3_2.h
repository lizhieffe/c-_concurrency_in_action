#include <iostream>

namespace ccia {

int GetNum() {
  static int value = 0;
  return ++value;  
}

void Foo(int a, int b) {
  std::cout << a << "," << b << std::endl;
}

void Run532() {
  for (int i = 0; i < 5;  ++i) {
    Foo(GetNum(), GetNum());
  }
}

}  // namespace ccia
