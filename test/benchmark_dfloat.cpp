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

#include <chrono>
#include <fstream>
#include <iostream>
#include "dfloat.h"
#include "Timer.hpp"

typedef xu::dfloat dfloat;

template <typename number_t>
class Data
{
private:
  number_t* arr_;
  size_t count_;
public:
  Data(const std::string& filepath)
    : arr_(nullptr), count_(0)
  {
    /* open at end to get filesize first */
    std::ifstream file(filepath, std::ios::in | std::ios::ate | std::ios::binary);

    if (file)
    {
      size_t filesize = file.tellg();
      file.seekg(0);

      count_ = filesize / sizeof(number_t);

      arr_ = new number_t[count_];

      for (size_t i = 0; i < count_; i++)
      {
        file.read((char*)&arr_[i], sizeof(number_t));
      }
    }
  }

  ~Data()
  {
    if (arr_ != nullptr)
    {
      delete[] arr_;
    }
  }

  number_t operator[](size_t idx) const
  {
    return arr_[idx];
  }

  size_t count() const
  {
    return count_;
  }
};

#define TEST_ADD(a,b,c,d,e) sum += (a + b + c + d + e);
#define TEST_SUBTRACT(a,b,c,d,e) sum += (a - b - c - d - e);
#define TEST_MULTIPLY(a,b,c,d,e) sum += (a * b * c * d * e);
#define TEST_DIVIDE(a,b,c,d,e) sum += (a / b / c / d / e);

#define START_LOOP(i) for (size_t i = 0; i < count; i++) {

#define END_LOOP() }

#define NESTED_LOOP(TYPE, TEST)                                                 \
  START_LOOP(a)                                                                 \
      START_LOOP(b)                                                             \
        START_LOOP(c)                                                           \
          START_LOOP(d)                                                         \
            START_LOOP(e)                                                       \
              TEST(                                                             \
                TYPE(data[a]),                                                  \
                TYPE(data[b]),                                                  \
                TYPE(data[c]),                                                  \
                TYPE(data[d]),                                                  \
                TYPE(data[e]))                                                  \
            END_LOOP()                                                          \
          END_LOOP()                                                            \
        END_LOOP()                                                              \
      END_LOOP()                                                                \
    END_LOOP()

#define DO_TEST(TYPE, TEST, LABEL)                                              \
  {                                                                             \
    size_t count = data.count();                                                \
    TYPE sum = 0;                                                               \
                                                                                \
    Timer t;                                                                    \
    t.start();                                                                  \
                                                                                \
    NESTED_LOOP(TYPE, TEST);                                                    \
                                                                                \
    std::cout << #TYPE << '\t';                                                 \
    std::cout << LABEL << '\t';                                                 \
    std::cout << std::setw(8) << std::left << t.stop() << '\t';                 \
    std::cout << sum << std::endl;                                              \
  }

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: benchmark_dfloat <filepath>" << std::endl;
    abort();
  }

  Data<long long> data(argv[1]);

  if (data.count() < 1)
  {
    std::cerr << "At least one number required" << std::endl;
    abort();
  }
  else
  {
    std::cout << "Read " << data.count() << " numbers from file" << std::endl;
  }

  DO_TEST(double, TEST_ADD, "+");
  
  DO_TEST(dfloat, TEST_ADD, "+");

  DO_TEST(double, TEST_SUBTRACT, "-");
  
  DO_TEST(dfloat, TEST_SUBTRACT, "-");

  DO_TEST(double, TEST_MULTIPLY, "*");
  
  DO_TEST(dfloat, TEST_MULTIPLY, "*");

  DO_TEST(double, TEST_DIVIDE, "/");
  
  DO_TEST(dfloat, TEST_DIVIDE, "/");
}