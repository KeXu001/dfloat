/*
 *  MIT License
 *
 *  Copyright (c) 2020 Kevin Xu
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <iomanip>
#include <iostream>
#include "dfloat.h"

#define MAX32 4294967295

typedef xu::dfloat dfloat;

int main()
{
  // dfloat df1(1152921504606846976ul);

  // dfloat df2(1152921504606846976ul);

  // dfloat df1(-10.1);

  // dfloat df2(20ul);

  // dfloat df1(-100000000000000ul);

  // dfloat df2(0ul);

  dfloat df1(110.0);

  dfloat df2("110");

  dfloat df3("012310123409010203040.506070809");

  std::cout << "df1=" << df1 << std::endl;

  std::cout << "df2=" << df2 << std::endl;

  std::cout << "df3=" << df3 << std::endl;

  std::cout << (df1 * df2) << std::endl;

  std::cout << (df1 / df2) << std::endl;

  std::cout << (df1 + df2) << std::endl;

  std::cout << (df1 - df2) << std::endl;

  std::cout << dfloat(MAX32) * 2 / (dfloat(MAX32)) << std::endl;

  df2 /= 25;

  std::cout << df2 << std::endl;

  df2 += 1;

  std::cout << df2 << std::endl;

  std::cout << (double)df3 << std::endl;

  std::cout << 012310123409010203040.506070809 << std::endl;

  double oneten = 110;
  std::cout << std::boolalpha << (oneten > 110) << std::endl;
  std::cout << std::boolalpha << (df1 > 110) << std::endl;
  std::cout << std::boolalpha << (df2 > 110) << std::endl;
}