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

// g++ -o bin/test_dfloat -I../include -Wfatal-errors -Wall test_dfloat.cpp

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include "dfloat.hpp"

typedef xu::dfloat dfloat;

#define assert_false(expr) assert((expr)==false)

void constructors()
{
  {
    dfloat f;
  }

  {
    dfloat f(0);
  }

  {
    dfloat f1(0);
    dfloat f2(f1);
  }

  {
    dfloat f1(0);
    __attribute__((__unused__))
    dfloat f2 = f1;
  }

  {
    dfloat f1(0);
    dfloat f2(std::move(f1));
  }

  {
    dfloat f1(0);
    __attribute__((__unused__))
    dfloat f2 = std::move(f1);
  }

  {
    int16_t x = -32768;
    dfloat f(x);
  }

  {
    uint16_t x = 65535;
    dfloat f(x);
  }

  {
    int32_t x = -2147483648;
    dfloat f(x);
  }

  {
    uint32_t x = 4294967295;
    dfloat f(x);
  }

  {
    int64_t x = -9223372036854775808ul;
    dfloat f(x);
  }

  {
    uint64_t x = 18446744073709551615ull;
    dfloat f(x);
  }

  {
    float x = 0.1;
    dfloat f(x);
  }

  {
    double x = 0.1;
    dfloat f(x);
  }

  {
    long double x = 0.1;
    dfloat f(x);
  }
}

void to_from_strings()
{
  // value
  {
    dfloat f = dfloat::parse("0");
    assert(dfloat::to_string(f) == "0");
  }

  {
    dfloat f = dfloat::parse("1");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("100");
    assert(dfloat::to_string(f) == "100");
  }

  {
    dfloat f = dfloat::parse("101");
    assert(dfloat::to_string(f) == "101");
  }

  {
    dfloat f = dfloat::parse("1.1");
    assert(dfloat::to_string(f) == "1.1");
  }

  {
    dfloat f = dfloat::parse("1.01");
    assert(dfloat::to_string(f) == "1.01");
  }

  {
    dfloat f = dfloat::parse("10.1");
    assert(dfloat::to_string(f) == "10.1");
  }

  {
    dfloat f = dfloat::parse("123456789");
    assert(dfloat::to_string(f) == "123456789");
  }

  {
    dfloat f = dfloat::parse("1.23456789");
    assert(dfloat::to_string(f) == "1.23456789");
  }

  {
    dfloat f = dfloat::parse("0.1");
    assert(dfloat::to_string(f) == "0.1");
  }

  {
    dfloat f = dfloat::parse("0.01");
    assert(dfloat::to_string(f) == "0.01");
  }

  {
    dfloat f = dfloat::parse("0.11");
    assert(dfloat::to_string(f) == "0.11");
  }

  {
    dfloat f = dfloat::parse("0.011");
    assert(dfloat::to_string(f) == "0.011");
  }

  // sign
  {
    dfloat f = dfloat::parse("-1");
    assert(dfloat::to_string(f) == "-1");
  }

  {
    dfloat f = dfloat::parse("+1");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("+0");
    assert(dfloat::to_string(f) == "0");
  }

  {
    dfloat f = dfloat::parse("-0");
    assert(dfloat::to_string(f) == "0");
  }

  // leading/trailing zeros
  {
    dfloat f = dfloat::parse("001");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("000");
    assert(dfloat::to_string(f) == "0");
  }

  {
    dfloat f = dfloat::parse("+000");
    assert(dfloat::to_string(f) == "0");
  }

  {
    dfloat f = dfloat::parse("+001");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("-001");
    assert(dfloat::to_string(f) == "-1");
  }

  {
    dfloat f = dfloat::parse("1.00");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("+1.00");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("-1.00");
    assert(dfloat::to_string(f) == "-1");
  }

  // scientific notation
  {
    dfloat f = dfloat::parse("1e0");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("1e1");
    assert(dfloat::to_string(f) == "10");
  }

  {
    dfloat f = dfloat::parse("1e2");
    assert(dfloat::to_string(f) == "100");
  }

  {
    dfloat f = dfloat::parse("1e-1");
    assert(dfloat::to_string(f) == "0.1");
  }

  {
    dfloat f = dfloat::parse("1e-2");
    assert(dfloat::to_string(f) == "0.01");
  }

  {
    dfloat f = dfloat::parse("1.1e1");
    assert(dfloat::to_string(f) == "11");
  }

  {
    dfloat f = dfloat::parse("1.1e-1");
    assert(dfloat::to_string(f) == "0.11");
  }

  {
    dfloat f = dfloat::parse("11e-2");
    assert(dfloat::to_string(f) == "0.11");
  }

  {
    dfloat f = dfloat::parse("11e2");
    assert(dfloat::to_string(f) == "1100");
  }

  {
    dfloat f = dfloat::parse("123e-3");
    assert(dfloat::to_string(f) == "0.123");
  }

  {
    dfloat f = dfloat::parse("0.123e3");
    assert(dfloat::to_string(f) == "123");
  }

  {
    dfloat f = dfloat::parse("1E0");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("1e000000");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("1e000001");
    assert(dfloat::to_string(f) == "10");
  }

  {
    dfloat f = dfloat::parse("1e000002");
    assert(dfloat::to_string(f) == "100");
  }

  {
    dfloat f = dfloat::parse("1e-000000");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("1e-000001");
    assert(dfloat::to_string(f) == "0.1");
  }

  {
    dfloat f = dfloat::parse("1e-000002");
    assert(dfloat::to_string(f) == "0.01");
  }

  {
    dfloat f = dfloat::parse("1e+0");
    assert(dfloat::to_string(f) == "1");
  }

  {
    dfloat f = dfloat::parse("1e+1");
    assert(dfloat::to_string(f) == "10");
  }

  {
    dfloat f = dfloat::parse("1e+01");
    assert(dfloat::to_string(f) == "10");
  }

  {
    dfloat f = dfloat::parse("1e-0");
    assert(dfloat::to_string(f) == "1");
  }

  // bad format
  {
    dfloat f = dfloat::parse("");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("abcd");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("12f");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("12d");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("12_3");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("0+0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse(" ");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("1 ");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse(" 1");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("1.");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse(".1");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse(".0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse(".0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse(".");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("1e");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("e1");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("1e.1");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("0e.1");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("0e-0.1");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("-1.0.0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("-1..0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("1..0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("1e10.0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("+-0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("-+0");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("+");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse("-");
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f = dfloat::parse(".e1");
    assert_false(dfloat::isfinite(f));
  }

  // truncation
  {
    dfloat f = dfloat::parse("1000000000000000009");
    assert(dfloat::to_string(f) == "1.0e18");
  }

  {
    dfloat f = dfloat::parse("-1000000000000000009");
    assert(dfloat::to_string(f) == "-1.0e18");
  }

  {
    dfloat f = dfloat::parse("0.1000000000000000009");
    assert(dfloat::to_string(f) == "0.1");
  }

  {
    dfloat f = dfloat::parse("-0.1000000000000000009");
    assert(dfloat::to_string(f) == "-0.1");
  }

  {
    dfloat f = dfloat::parse("1000000000000000789");
    assert(dfloat::to_string(f) == "1.00000000000000078e18");
  }

  {
    dfloat f = dfloat::parse("-1000000000000000789");
    assert(dfloat::to_string(f) == "-1.00000000000000078e18");
  }

  {
    dfloat f = dfloat::parse("0.1000000000000000789");
    assert(dfloat::to_string(f) == "0.100000000000000078");
  }

  {
    dfloat f = dfloat::parse("-0.1000000000000000789");
    assert(dfloat::to_string(f) == "-0.100000000000000078");
  }

  // nan
  {
    assert(dfloat::to_string(dfloat(NAN)) == "nan");
  }
}

void comparisons()
{
  const size_t N = 10;

  dfloat values[N] = {
    dfloat(0),
    dfloat(0.0),
    dfloat(0.1),
    dfloat(-0.1),
    dfloat(1),
    dfloat(-1),
    dfloat(100),
    dfloat(-100),
    dfloat(123456.789),
    dfloat(-123456.789)
  };

  int sign[N] = {
    0,
    0,
    1,
    -1,
    1,
    -1,
    1,
    -1,
    1,
    -1
  };

  int rel_mag[N] = {
    0,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4
  };

  for (size_t i = 0; i < N; i++)
  {
    for (size_t j = 0; j < N ; j++)
    {
      // ==
      if (sign[i] == sign[j] and rel_mag[i] == rel_mag[j])
      {
        assert(values[i] == values[j]);
      }
      else
      {
        assert_false(values[i] == values[j]);
      }

      // !=
      if (sign[i] == sign[j] and rel_mag[i] == rel_mag[j])
      {
        assert_false(values[i] != values[j]);
      }
      else
      {
        assert(values[i] != values[j]);
      }

      // <=
      if (
        (sign[i] < sign[j]) ||
        (sign[i] == sign[j] and sign[i] == 1 and rel_mag[i] <= rel_mag[j]) ||
        (sign[i] == sign[j] and sign[i] == -1 and rel_mag[i] >= rel_mag[j]) ||
        (sign[i] == sign[j] and sign[i] == 0)
        )
      {
        assert(values[i] <= values[j]);
      }
      else
      {
        assert_false(values[i] <= values[j]);
      }

      // >=
      if (
        (sign[i] > sign[j]) ||
        (sign[i] == sign[j] and sign[i] == 1 and rel_mag[i] >= rel_mag[j]) ||
        (sign[i] == sign[j] and sign[i] == -1 and rel_mag[i] <= rel_mag[j]) ||
        (sign[i] == sign[j] and sign[i] == 0)
        )
      {
        assert(values[i] >= values[j]);
      }
      else
      {
        assert_false(values[i] >= values[j]);
      }

      // <
      if (
        (sign[i] < sign[j]) ||
        (sign[i] == sign[j] and sign[i] == 1 and rel_mag[i] < rel_mag[j]) ||
        (sign[i] == sign[j] and sign[i] == -1 and rel_mag[i] > rel_mag[j])
        )
      {
        assert(values[i] < values[j]);
      }
      else
      {
        assert_false(values[i] < values[j]);
      }

      // >
      if (
        (sign[i] > sign[j]) ||
        (sign[i] == sign[j] and sign[i] == 1 and rel_mag[i] > rel_mag[j]) ||
        (sign[i] == sign[j] and sign[i] == -1 and rel_mag[i] < rel_mag[j])
        )
      {
        assert(values[i] > values[j]);
      }
      else
      {
        assert_false(values[i] > values[j]);
      }
    }
  }

}

void arithmetic()
{
  assert(-dfloat(0) == dfloat(0));
  assert(+dfloat(0) == dfloat(0));
  assert(-dfloat(1) == dfloat(-1));
  assert(+dfloat(1) == dfloat(1));
  assert(-dfloat(-1) == dfloat(1));
  assert(+dfloat(-1) == dfloat(-1));

  assert(dfloat(0) + dfloat(0) == dfloat(0));
  assert(dfloat(0) + dfloat(1) == dfloat(1));
  assert(dfloat(1) + dfloat(0) == dfloat(1));
  assert(dfloat(-1) + dfloat(0) == dfloat(-1));
  assert(dfloat(0) + dfloat(-1) == dfloat(-1));
  assert(dfloat(1) + dfloat(1) == dfloat(2));
  assert(dfloat(-1) + dfloat(-1) == dfloat(-2));
  assert(dfloat(1) + dfloat(-1) == dfloat(0));
  assert(dfloat(-1) + dfloat(1) == dfloat(0));

  assert(dfloat(0) - dfloat(0) == dfloat(0));
  assert(dfloat(0) - dfloat(1) == dfloat(-1));
  assert(dfloat(1) - dfloat(0) == dfloat(1));
  assert(dfloat(-1) - dfloat(0) == dfloat(-1));
  assert(dfloat(0) - dfloat(-1) == dfloat(1));
  assert(dfloat(1) - dfloat(1) == dfloat(0));
  assert(dfloat(-1) - dfloat(-1) == dfloat(0));
  assert(dfloat(1) - dfloat(-1) == dfloat(2));
  assert(dfloat(-1) - dfloat(1) == dfloat(-2));

  assert(dfloat(0) * dfloat(0) == dfloat(0));
  assert(dfloat(0) * dfloat(1) == dfloat(0));
  assert(dfloat(1) * dfloat(0) == dfloat(0));
  assert(dfloat(-1) * dfloat(0) == dfloat(0));
  assert(dfloat(0) * dfloat(-1) == dfloat(0));
  assert(dfloat(1) * dfloat(1) == dfloat(1));
  assert(dfloat(-1) * dfloat(-1) == dfloat(1));
  assert(dfloat(1) * dfloat(-1) == dfloat(-1));
  assert(dfloat(-1) * dfloat(1) == dfloat(-1));

  assert(dfloat(0.1) * dfloat(0.1) == dfloat(0.01));
  assert(dfloat(10) * dfloat(0.1) == dfloat(1));
  assert(dfloat(10) * dfloat(10) == dfloat(100));

  assert(dfloat(0) / dfloat(1) == dfloat(0));
  assert(dfloat(0) / dfloat(-1) == dfloat(0));
  assert(dfloat(1) / dfloat(1) == dfloat(1));
  assert(dfloat(-1) / dfloat(-1) == dfloat(1));
  assert(dfloat(1) / dfloat(-1) == dfloat(-1));
  assert(dfloat(-1) / dfloat(1) == dfloat(-1));

  assert(dfloat(10) / dfloat(10) == dfloat(1));
  assert(dfloat(10) / dfloat(0.1) == dfloat(100));
  assert(dfloat(0.1) / dfloat(0.1) == dfloat(1));

  {
    dfloat f(0);
    f += dfloat(1);
    assert(f == dfloat(1));
  }

  {
    dfloat f(0);
    f -= dfloat(1);
    assert(f == dfloat(-1));
  }

  {
    dfloat f(-1);
    f += dfloat(1);
    assert(f == dfloat(0));
  }

  {
    dfloat f(-1);
    f -= dfloat(1);
    assert(f == dfloat(-2));
  }

  {
    dfloat f(0);
    f -= dfloat(-1);
    assert(f == dfloat(1));
  }

  {
    dfloat f(0);
    f *= dfloat(1);
    assert(f == dfloat(0));
  }

  {
    dfloat f(10);
    f *= dfloat(0.1);
    assert(f == dfloat(1));
  }

  {
    dfloat f(10);
    f /= dfloat(0.1);
    assert(f == dfloat(100));
  }
}

void not_a_number()
{
  {
    dfloat f(NAN);
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f(-NAN);
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f(INFINITY);
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f(-INFINITY);
    assert_false(dfloat::isfinite(f));
  }

  assert_false(dfloat::isfinite(dfloat(0) / dfloat(0)));
  assert_false(dfloat::isfinite(dfloat(1) / dfloat(0)));
  assert_false(dfloat::isfinite(dfloat(-1) / dfloat(0)));

  assert_false(dfloat::isfinite(dfloat(NAN) + dfloat(0)));
  assert_false(dfloat::isfinite(dfloat(NAN) - dfloat(0)));
  assert_false(dfloat::isfinite(dfloat(NAN) * dfloat(0)));
  assert_false(dfloat::isfinite(dfloat(NAN) / dfloat(0)));
  assert_false(dfloat::isfinite(dfloat(NAN) + dfloat(1)));
  assert_false(dfloat::isfinite(dfloat(NAN) - dfloat(1)));
  assert_false(dfloat::isfinite(dfloat(NAN) * dfloat(1)));
  assert_false(dfloat::isfinite(dfloat(NAN) / dfloat(1)));
  assert_false(dfloat::isfinite(dfloat(NAN) + dfloat(-0.1)));
  assert_false(dfloat::isfinite(dfloat(NAN) - dfloat(-0.1)));
  assert_false(dfloat::isfinite(dfloat(NAN) * dfloat(-0.1)));
  assert_false(dfloat::isfinite(dfloat(NAN) / dfloat(-0.1)));

  assert_false(dfloat(0) == dfloat(NAN));
  assert_false(dfloat(0) != dfloat(NAN));
  assert_false(dfloat(0) <= dfloat(NAN));
  assert_false(dfloat(0) >= dfloat(NAN));
  assert_false(dfloat(0) < dfloat(NAN));
  assert_false(dfloat(0) > dfloat(NAN));
  assert_false(dfloat(1) == dfloat(NAN));
  assert_false(dfloat(1) != dfloat(NAN));
  assert_false(dfloat(1) <= dfloat(NAN));
  assert_false(dfloat(1) >= dfloat(NAN));
  assert_false(dfloat(1) < dfloat(NAN));
  assert_false(dfloat(1) > dfloat(NAN));
  assert_false(dfloat(-0.1) == dfloat(NAN));
  assert_false(dfloat(-0.1) != dfloat(NAN));
  assert_false(dfloat(-0.1) <= dfloat(NAN));
  assert_false(dfloat(-0.1) >= dfloat(NAN));
  assert_false(dfloat(-0.1) < dfloat(NAN));
  assert_false(dfloat(-0.1) > dfloat(NAN));

  assert_false(dfloat(NAN) == dfloat(NAN));
  assert_false(dfloat(NAN) != dfloat(NAN));
  assert_false(dfloat(NAN) <= dfloat(NAN));
  assert_false(dfloat(NAN) >= dfloat(NAN));
  assert_false(dfloat(NAN) < dfloat(NAN));
  assert_false(dfloat(NAN) > dfloat(NAN));
}

void near_limits()
{
  {
    dfloat f(1e101);
    assert_false(dfloat::isfinite(f));
  }

  {
    dfloat f1 = dfloat::parse("9.99999999999999999e100");
    assert(dfloat::isfinite(f1));
  }

  {
    dfloat f1 = dfloat::parse("-9.99999999999999999e100");
    assert(dfloat::isfinite(f1));
  }

  {
    dfloat f1 = dfloat::parse("1e-100");
    assert(dfloat::isfinite(f1));
  }

  {
    dfloat f1 = dfloat::parse("-1e-100");
    assert(dfloat::isfinite(f1));
  }

  {
    dfloat f1 = dfloat::parse("10e100");
    assert_false(dfloat::isfinite(f1));
  }

  {
    dfloat f1 = dfloat::parse("-10e100");
    assert_false(dfloat::isfinite(f1));
  }

  {
    dfloat f1 = dfloat::parse("9.99999999999999999e100");
    assert_false(dfloat::isfinite(f1 + dfloat::parse("1e83")));
  }

  {
    dfloat f1 = dfloat::parse("-9.99999999999999999e100");
    assert_false(dfloat::isfinite(f1 - dfloat::parse("1e83")));
  }

  {
    dfloat f1 = dfloat::parse("1.1e-100");
    assert(dfloat::isfinite(f1 - dfloat::parse("1.1e-100")));
  }

  {
    dfloat f1 = dfloat::parse("-1.1e-100");
    assert(dfloat::isfinite(f1 + dfloat::parse("1.1e-100")));
  }

  {
    dfloat f1 = dfloat::parse("9.99999999999999999e99");
    dfloat f2 = dfloat::parse("2");
    assert(dfloat::isfinite(f1 * f2));
  }

  {
    dfloat f1 = dfloat::parse("2e-100");
    dfloat f2 = dfloat::parse("2");
    assert(dfloat::isfinite(f1 / f2));
  }

  {
    dfloat f1 = dfloat::parse("1e100");
    dfloat f2 = dfloat::parse("1e-100");
    assert(f1 * f2 == dfloat(1));
  }

  {
    dfloat f1 = dfloat::parse("9e100");
    dfloat f2 = dfloat::parse("9e100");
    assert(f1 / f2 == dfloat(1));
  }

  {
    dfloat f1 = dfloat::parse("1e-100");
    dfloat f2 = dfloat::parse("1e-100");
    assert(f1 / f2 == dfloat(1));
  }

  {
    dfloat f1 = dfloat::parse("2e50");
    dfloat f2 = dfloat::parse("4.4999e50");
    assert(dfloat::isfinite(f1 * f2));
  }

  {
    dfloat f1 = dfloat::parse("2e50");
    dfloat f2 = dfloat::parse("5e50");
    assert_false(dfloat::isfinite(f1 * f2));
  }

  {
    dfloat f1 = dfloat::parse("1e50");
    dfloat f2 = dfloat::parse("1e-50");
    assert(dfloat::isfinite(f1 / f2));
  }

  {
    dfloat f1 = dfloat::parse("1e-50");
    dfloat f2 = dfloat::parse("1e50");
    assert(dfloat::isfinite(f1 / f2));
  }
}

void denormal()
{
  // arithmetic resulting in denormal value
  {
    dfloat f1 = dfloat::parse("1.1e-100");
    dfloat f2 = dfloat::parse("1e-100");
    assert(dfloat::to_string(f1 - f2) == "0.1e-100");
  }

  {
    dfloat f1 = dfloat::parse("-1.1e-100");
    dfloat f2 = dfloat::parse("1e-100");
    assert(dfloat::to_string(f1 + f2) == "-0.1e-100");
  }

  {
    dfloat f1 = dfloat::parse("1.01e-100");
    dfloat f2 = dfloat::parse("1e-100");
    assert(dfloat::to_string(f1 - f2) == "0.01e-100");
  }

  {
    dfloat f1 = dfloat::parse("1e-100");
    dfloat f2 = dfloat::parse("0.1");
    assert(dfloat::to_string(f1 * f2) == "0.1e-100");
  }

  {
    dfloat f1 = dfloat::parse("1e-100");
    dfloat f2 = dfloat::parse("10");
    assert(dfloat::to_string(f1 / f2) == "0.1e-100");
  }

  // arithmetic resulting in zero denormal
  {
    dfloat f1 = dfloat::parse("1e-100");
    dfloat f2 = dfloat::parse("1e-18");
    assert(dfloat::to_string(f1 * f2) == "0");
  }

  {
    dfloat f1 = dfloat::parse("1e-100");
    dfloat f2 = dfloat::parse("1e18");
    assert(dfloat::to_string(f1 / f2) == "0");
  }

  // arithmetic with denormal value as an operand
  {
    dfloat f1 = dfloat(1);
    dfloat f2 = dfloat::parse("1e-100") / dfloat(10);
    assert(f1 - f2 == f1);
  }

  {
    dfloat f1 = dfloat(1);
    dfloat f2 = dfloat::parse("1e-100") / dfloat(10);
    assert(f1 + f2 == f1);
  }

  {
    dfloat f1 = dfloat::parse("1.2e-100");
    dfloat f2 = dfloat::parse("1e-100") / dfloat(10);  // 0.1e-100
    assert(dfloat::to_string(f1 - f2) == "1.1e-100");
  }

  {
    dfloat f1 = dfloat::parse("1.2e-100");
    dfloat f2 = dfloat::parse("1e-100") / dfloat(10);  // 0.1e-100
    assert(dfloat::to_string(f1 + f2) == "1.3e-100");
  }

  {
    dfloat f1 = dfloat::parse("5e100");
    dfloat f2 = dfloat::parse("2e-100") / dfloat(10);  // 0.2e-100
    assert(f1 * f2 == 1);
  }

  {
    dfloat f1 = dfloat::parse("5e100");
    dfloat f2 = dfloat::parse("2e-100") / dfloat(100);  // 0.02e-100
    assert(f1 * f2 == dfloat::parse("0.1"));
  }

  {
    dfloat f1 = dfloat::parse("1e-100");
    dfloat f2 = dfloat::parse("2e-100") / dfloat(10);  // 0.2e-100
    assert(f1 / f2 == dfloat::parse("5"));
  }

  {
    dfloat f1 = dfloat::parse("2e-100");
    dfloat f2 = dfloat::parse("2e-100") / dfloat(100);  // 0.02e-100
    assert(f1 / f2 == dfloat::parse("100"));
  }

  {
    dfloat f1 = dfloat::parse("1e-100");
    dfloat f2 = dfloat::parse("2e-100") / dfloat(100);  // 0.02e-100
    assert(f1 / f2 == dfloat::parse("50"));
  }

  {
    dfloat f1 = dfloat::parse("1e-100") / dfloat(10);  // 0.1e-100
    dfloat f2 = dfloat::parse("1e-100") / dfloat(10);  // 0.1e-100
    assert(f1 / f2 == 1);
  }

  {
    dfloat f1 = dfloat::parse("1e-100") / dfloat(10);  // 0.1e-100
    dfloat f2 = dfloat(10);
    assert(dfloat::to_string(f1 / f2) == "0.01e-100");
  }

  {
    dfloat f1 = dfloat::parse("1e-100") / dfloat(10);  // 0.1e-100
    dfloat f2 = dfloat::parse("0.1");
    assert(dfloat::to_string(f1 * f2) == "0.01e-100");
  }
}

int main()
{
  constructors();

  to_from_strings();

  comparisons();

  arithmetic();

  not_a_number();

  near_limits();

  denormal();
}