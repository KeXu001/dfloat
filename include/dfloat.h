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

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <limits>
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
    using sign_t = int8_t;
    using mant_t = uint64_t;
    using pow_t = int8_t;
    using mant2_t = __uint128_t;  // type that can fit a product of `mant_t`
    using pow2_t = int16_t;  // type that can fit a product of `pow_t`

    /**
      @brief  The scale of member variable `mant`
              Equal to 100,000,000 billion
      @note   Determined by picking highest number which still leaves enough headroom for arithmetic operations:
              addition requires:        2^64 - 1 >= (MANT_CAP - 1) + (MANT_CAP - 1)
              multiplication requires:  2^64 - 1 >= (MANT_CAP - 1) * (MANT_CAP - 1) / SCALE
      */
    static constexpr mant_t SCALE = 100000000000000000;

    /**
      @brief  The power of 10 which equals SCALE
      */
    static constexpr pow_t SCALE_POW = 17;

    /**
      @brief  Constant to hold base 10
      @note   Implementation note: we use uint8_t instead of mant_t, otherwise
              some necessary operations will not have integer promotion
      */
    static constexpr uint8_t BASE = 10;

    /**
      @brief  The maximum value of `mant`, plus one
              Equal to 1 billion billion
      */
    static constexpr mant_t MANT_CAP = BASE * SCALE;

    /**
      @brief  Highest possible value of `pow`
      */
    static constexpr pow_t MAX_POW = 100;

    /**
      @brief  Lowest possible value of `pow`
      @note   Must be greater than the numeric minimum limit of `pow_t`, so that
              unary minus sign does not result in loss
      */
    static constexpr pow_t MIN_POW = -100;

    /**
      @brief  Represents sign of the mantissa, if there is one, or NaN
      @note   Unlike doubles, dfloats cannot be infinity or -infinity or -nan
      */
    enum class Sign : sign_t
    {
      NEG = -1,
      ZERO = 0,
      POS = 1,
      
      _NAN_ = 2
    };

    enum class ComparisonResult : sign_t
    {
      LESS = -1,
      EQUAL = 0,
      MORE = 1,

      _NAN_ = 2
    };

  public:
    /**
      @brief  Default constructor
              Construct uninitialized dfloat
      */
    dfloat() = default;

    dfloat(const dfloat& other) = default;
    
    dfloat& operator=(const dfloat& other) = default;
    
    dfloat(dfloat&& other) = default;
    
    dfloat& operator=(dfloat&& other) = default;

  protected:
    /**
      @brief  Verbose constructor
              Construct dfloat from parts
      */
    dfloat(Sign sign_, mant_t mant_, pow_t pow_);
  
  public:

    //  ===========
    //  Conversions
    //  ===========

    /* constructor for unsigned integers */
    template <
      typename T,
      typename std::enable_if_t<
        std::is_integral<T>::value && std::is_unsigned<T>::value,
        bool> = true>
    dfloat(T value);

    /* constructor for signed integers */
    template <
      typename T,
      typename std::enable_if_t<
        std::is_integral<T>::value && std::is_signed<T>::value,
        bool> = true>
    dfloat(T value);

    template <
      typename T,
      typename std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    dfloat(T value);

    /*
      @brief  Convert to unsigned integer
              Returns the remainder R of the division F / D, where D is the
              divisor i.e. the number of integers representable by typename T,
              minus any fractional part, such that 0 <= R < D
      @note   nan will be converted to 0
      @note   this will not work for types larger than `mant_t`
    */
    template <
      typename T,
      typename std::enable_if_t<
        std::is_integral<T>::value && std::is_unsigned<T>::value,
        bool> = true>
    explicit operator T() const;

    /*
      @brief  Convert to unsigned integer
              Similiar to the version for unsigned types, except that R is
              within the valid range for signed typename T
      @note   nan will be converted to 0
    */
    template <
      typename T,
      typename std::enable_if_t<
        std::is_integral<T>::value && std::is_signed<T>::value,
        bool> = true>
    explicit operator T() const;

    template <
      typename T,
      typename std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    explicit operator T() const;

    //  ====================
    //  Comparison Operators
    //  ====================

    bool operator==(const dfloat& other) const;

    bool operator!=(const dfloat& other) const;

    bool operator>(const dfloat& other) const;

    bool operator<(const dfloat& other) const;

    bool operator>=(const dfloat& other) const;

    bool operator<=(const dfloat& other) const;

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
    dfloat operator+() const;

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
      */
    dfloat operator/(const dfloat& other) const;

    /**
      @brief  Modulo another dfloat
      @note   Given two dfloats A and B, this method returns R such that:
                R = A - N * B
                N is an integer
      */
    dfloat operator%(const dfloat& other) const;

  protected:
    /**
      @brief  Returns which operand is greater
      @return 1 if greater than other, -1 if less than other, 0 if equal, 2 if no comparison
      */
    ComparisonResult _comparedTo(const dfloat& other) const;

    /**
      @brief  Returns which operand has larger magnitude
      @note   Assumes both numbers are valid and finite
      @return 1 if bigger than other, -1 if smaller than other, 0 if equal, 2 if no comparison
      */
    ComparisonResult _compareMagnitudeTo(const dfloat& other) const;

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
    mant_t mant;

    /**
      @brief  Power (base 10) of the expression
              Integer value falls in the range
                [ MIN_POW, MAX_POW ]
      */
    pow_t pow;
    
  public:

    //  ==============
    //  To/From String
    //  ==============

    /**
      @brief  Parse string as dfloat
              String must be in decimal or scientific notation.
      @note   Leading and trailing zeros are ignored.
      @note   Sign (+/-) is supported before the integral part as well as the
              exponent.
      @note   Exponent must be preceded by and followed by a digit
      @note   Decimal must be preceded by and followed by a digit, otherwise "."
              would be a valid number
      @note   If bad format, result is NaN.
      @note   If outside range, result is NaN.
      @todo   Make it possible to tell between bad format and outside range.
      @note   If whole number part exceeds range, or if exponent exceeds
              exponent range, result will be NaN, even if the exponent would
              bring it back within range e.g. "10...0e-200" would fail
      */
    static dfloat parse(const std::string& str);

    /**
      @brief  Convert to string
      */
    static std::string to_string(const dfloat& d, pow2_t exp_thresh = 10);

    /**
      @brief  Converts dfloat to string on stream
      @param  exp_thresh  use scientific notation if exponent is of equal or
                          larger magnitude than this value
                            if zero or below: always use scientific
                            if between 1 and MAX_POW: sometimes use scientific
                            if above MAX_POW: never use scientific
      */
    std::ostream& print_to(std::ostream& stream, pow2_t exp_thresh = 10) const;
    
    //  ==============
    //  Static Methods
    //  ==============
    
    static bool isfinite(const dfloat& d);
    
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
