
#include <iomanip>
#include <iostream>
#include "biguint.hpp"

typedef xu::biguint<2> biguint64;

/*
  Print hex representation of biguint to stream
  */
std::ostream& operator<<(std::ostream& stream, const biguint64& b)
{
  stream << b[1] << ',' << b[0];

  return stream;
}

int main()
{
  biguint64 b1(789u);

  biguint64 b2(123u);

  std::cout << b1 - b2 << std::endl;
}