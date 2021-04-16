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

#include <cassert>
#include <iomanip>
#include <iostream>
#include "dfloat.hpp"
#include "other_class.h"

#define MAX32 4294967295

typedef xu::dfloat dfloat;

int main()
{
  std::cout << "sizeof(dfloat)=" << sizeof(dfloat) << std::endl;

  // dfloat df1(1152921504606846976ul);

  // dfloat df2(1152921504606846976ul);

  // dfloat df1(-10.1);

  // dfloat df2(20ul);

  // dfloat df1(-100000000000000ul);

  // dfloat df2(0ul);

  dfloat df1(110.0);

  dfloat df2("110");

  dfloat df3("012310123409010203040.506070809");

  dfloat df4(110.0f);

  int16_t x5 = -32768;
  dfloat df5(x5);
  uint16_t x6 = 65535;
  dfloat df6(x6);
  int32_t x7 = -2147483648;
  dfloat df7(x7);
  uint32_t x8 = 4294967295;
  dfloat df8(x8);
  int64_t x9 = -9223372036854775808ul;
  dfloat df9(x9);
  uint64_t x10 = 18446744073709551615ull;
  dfloat df10(x10);
  long long x11 = -9223372036854775808ul;
  dfloat df11(x11);
  unsigned long long x12 = 18446744073709551615ull;
  dfloat df12(x12);

  std::cout << "df1=" << df1 << std::endl;

  std::cout << "df2=" << df2 << std::endl;

  std::cout << "df3=" << df3 << std::endl;

  std::cout << "df4=" << df4 << std::endl;

  std::cout << "df5=" << df5 << std::endl;
  std::cout << "df6=" << df6 << std::endl;
  std::cout << "df7=" << df7 << std::endl;
  std::cout << "df8=" << df8 << std::endl;
  std::cout << "df9=" << df9 << std::endl;
  std::cout << "df10=" << df10 << std::endl;
  std::cout << "df11=" << df11 << std::endl;
  std::cout << "df12=" << df12 << std::endl;

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

  std::cout << dfloat("100000000.000000001") << std::endl;

  double test = 0.1l * 0.1l - 0.01l;
  std::cout << (test == 0) << std::endl;

  dfloat test2 = dfloat("0.1") * dfloat("0.1") - dfloat("0.01");
  std::cout << (test2 == 0) << std::endl;

  double test3 = 0.4l * 0.4l - 0.16l;
  std::cout << test3 << std::endl;

  dfloat test4 = dfloat("0.4") * dfloat("0.4") - dfloat("0.16");
  std::cout << test4 << std::endl;

  other_class oc;

  std::cout << oc.get() << std::endl;

  std::cout << (dfloat("20") * 0.1) << "==?" << (0.1 * dfloat("20")) << std::endl;
  std::cout << (dfloat("20") + 16) << "==?" << (16 + dfloat("20")) << std::endl;

  std::cout << (dfloat("78.5") <= 78.5) << std::endl;
  std::cout << (78.5 >= dfloat("78.5")) << std::endl;

  assert(dfloat("10") + dfloat("20") == dfloat("30"));
  assert(dfloat("10") - dfloat("20") == dfloat("-10"));
  assert(dfloat("10") + dfloat("10") == dfloat("20"));
  assert(dfloat("10") - dfloat("10") == dfloat("0"));

  assert(-dfloat("12345") == dfloat("-12345"));
  assert(-dfloat("0") == dfloat("0"));

  assert(dfloat("123") * dfloat("123") == dfloat("15129"));
  assert(-dfloat("123") * dfloat("123") == dfloat("-15129"));
  assert(dfloat("1000") / dfloat("250") == dfloat("4"));
  assert(dfloat("1000") / -dfloat("1000") == dfloat("-1"));
}