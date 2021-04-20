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

#pragma once

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

namespace xu
{
  /**
    @brief  Decimal floating point type
            This class implements a decimal floating point number with up to 18 significant figures of precision.
            The value is stored in three parts: sign, mantissa (scaled), and power (base 10)
    */
  class dfloat
  {
  public:
    /**
      @brief  The scale of member variable `mant`
              Equal to 100,000,000 billion
      @note   Determined by picking highest number which still leaves enough headroom for arithmetic operations:
              addition requires:        2^64 - 1 >= (MANT_CAP - 1) + (MANT_CAP - 1)
              multiplication requires:  2^64 - 1 >= (MANT_CAP - 1) * (MANT_CAP - 1) / SCALE
      */
    static const uint64_t SCALE = 100000000000000000;

    /**
      @brief  The power of 10 which equals SCALE
      */
    static const int8_t SCALE_POW = 17;

    /**
      @brief  Constant to hold base 10
      */
    static const uint64_t BASE = 10;

    /**
      @brief  The maximum value of `mant`, plus one
              Equal to 1 billion billion
      */
    static const uint64_t MANT_CAP = BASE * SCALE;

    /**
      @brief  Highest possible value of `pow`
      */
    static const int8_t MAX_POW = 127;

    /**
      @brief  Lowest possible value of `pow`
      */
    static const int8_t MIN_POW = -128;

    /**
      @brief  Represents sign of the mantissa
      */
    enum class Sign : int8_t
    {
      NEGATIVE = -1,
      ZERO = 0,
      POSITIVE = 1
    };

    /**
      @brief  Error type when parsing string as dfloat
      */
    class parse_error : public std::runtime_error
    {
    protected:
      char err_c;
    public:
      explicit parse_error(const std::string& what_arg)
        : std::runtime_error(what_arg)
      {}

      explicit parse_error(char c)
        : std::runtime_error(std::string("dfloat parse error: at char \'") + c + '\'')
      {}
    };

  public:
    /**
      @brief  Default constructor
              Construct zero-initialized dfloat
      */
    dfloat();

    dfloat(const dfloat& other);

    /**
      @brief  Verbose constructor
              Construct dfloat from parts
      */
    dfloat(Sign sign_, uint64_t mant_, int8_t pow_);

    //  ===========
    //  Conversions
    //  ===========

    /* must fit into a long long */
    template <
      typename T,
      typename std::enable_if_t<std::is_integral<T>::value, bool> = true>
    dfloat(T value);

    /* default logic for integral types */
    dfloat(int64_t value);

    /* uint64_t won't fit into int64_t, so have a special overload */
    dfloat(uint64_t value);

    /* since on some systems, uint64_t may be defined as `unsigned long` instead of `unsigned long long`, have another overload */
    dfloat(unsigned long long value);

    dfloat(double value);

    dfloat(float value);

    explicit operator double() const;

    //  ====================
    //  Comparison Operators
    //  ====================

    bool operator==(const dfloat& other) const;

    bool operator!=(const dfloat& other) const;

    bool operator>(const dfloat& other) const;

    bool operator<(const dfloat& other) const;

    bool operator>=(const dfloat& other) const;

    bool operator<=(const dfloat& other) const;

    bool isZero() const;

    //  ====================
    //  Assignment Operators
    //  ====================

    dfloat& operator+=(const dfloat& other);

    dfloat& operator-=(const dfloat& other);

    dfloat& operator*=(const dfloat& other);

    dfloat& operator/=(const dfloat& other);

    //  ====================
    //  Arithmetic Operators
    //  ====================

    /*
      There is no warning for overflow for arithmetic operators. If an overflow 
      occurs, behavior is undefined.
    */

    dfloat operator-() const;

    /**
      @brief  Add a dfloat
      @note   Truncates the operand with the smaller magnitude
      */
    dfloat operator+(const dfloat& other) const;

    /**
      @brief  Subtract a dfloat
      @note   Truncates the operand with the smaller magnitude
      */
    dfloat operator-(const dfloat& other) const;

    /**
      @brief  Multiply by a dfloat
      @note   Uses __uint128_t to perform 128-bit multiplication
      */
    dfloat operator*(const dfloat& other) const;

    /**
      @brief  Divide by a dfloat
      @note   Uses __uint128_t to perform 128-bit division
      @throw  std::runtime_error
              If divisor is zero
      */
    dfloat operator/(const dfloat& other) const;

  protected:
    /**
      @brief  Returns which operand is greater
      @return 1 if greater than other, -1 if less than other, 0 if equal
      */
    short _comparedTo(const dfloat& other) const;

    /**
      @brief  Returns which operand has larger magnitude
      @return 1 if bigger than other, -1 if smaller than other, 0 if equal
      */
    short _compareMagnitudeTo(const dfloat& other) const;

  protected:
    //  ================
    //  Member Variables
    //  ================

    /**
      @brief  Sign of the value, or ZERO if zero
      @note   If sign is ZERO, `mant` and `pow` are NOT defined
      */
    Sign sign;

    /**
      @brief  Mantissa of the expression, in units of 1/SCALE
              Integer value falls in the range
                [ 100,000,000,000,000,000 , 999,999,999,999,999,999 ]
              and represents a number between 1 and 10
                [ 1.0, 10.0 )
      */
    uint64_t mant;

    /**
      @brief  Power (base 10) of the expression
              Integer value falls in the range
                [ MIN_POW, MAX_POW ]
      */
    int8_t pow;
    
  public:

    //  ==============
    //  To/From String
    //  ==============

    /**
      @brief  Parse string as dfloat
              String must be in decimal notation (not scientific notation) and contain NO commas
              String must consist of only digits [and a negative sign] [and a decimal point]
      @note   Leading zeros are ignored.
      @throw  xu::dfloat::parse_error
              If format is not understood
      */
    dfloat(const std::string& str);

    /**
      @brief  Convert to string
      */
    explicit operator std::string() const;

    /**
      @brief  Converts dfloat to string on stream
      @note   Output is always in decimal notation (never scientific notation)
      */
    std::ostream& print(std::ostream& stream) const;
  } __attribute__((packed));

  /**
    @brief  operator+ free function with dfloat as right operand
    */
  template <typename T>
  dfloat operator+(T x, const dfloat& d);

  /**
    @brief  operator- free function with dfloat as right operand
    */
  template <typename T>
  dfloat operator-(T x, const dfloat& d);

  /**
    @brief  operator* free function with dfloat as right operand
    */
  template <typename T>
  dfloat operator*(T x, const dfloat& d);

  /**
    @brief  operator/ free function with dfloat as right operand
    */
  template <typename T>
  dfloat operator/(T x, const dfloat& d);

  /**
    @brief  operator== free function with dfloat as right operand
    */
  template <typename T>
  bool operator==(T x, const dfloat& d);

  /**
    @brief  operator!= free function with dfloat as right operand
    */
  template <typename T>
  bool operator!=(T x, const dfloat& d);

  /**
    @brief  operator> free function with dfloat as right operand
    */
  template <typename T>
  bool operator>(T x, const dfloat& d);

  /**
    @brief  operator< free function with dfloat as right operand
    */
  template <typename T>
  bool operator<(T x, const dfloat& d);

  /**
    @brief  operator>= free function with dfloat as right operand
    */
  template <typename T>
  bool operator>=(T x, const dfloat& d);

  /**
    @brief  operator<= free function with dfloat as right operand
    */
  template <typename T>
  bool operator<=(T x, const dfloat& d);
}

/**
  @brief  Implement operator<< for dfloat
  */
std::ostream& operator<<(std::ostream& stream, const xu::dfloat& d);
