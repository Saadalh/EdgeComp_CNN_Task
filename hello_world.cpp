#include <iostream>

int main() {

  int x = 2; // integer variable
  int * xp = &x; // integer pointer (points to x)
  int * viptr = new int(64); // void integer pointer (points to 64 bytes)

  std::cout << "Hello B!" << std::endl << std::endl;
  while (x<5) {
    std::cout << x << std::endl;
    x++;
  }

  std::cout << std::endl << xp << std::endl;
  std::cout << viptr << std::endl;

  delete viptr;
  return 0;
}

//-O1,2,3 OPtimization levels
//-march=skylake , -march=native generates code based on the (native) architecture
//-g enables debugging symbols