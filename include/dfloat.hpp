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

#include "dfloat.h"

namespace xu
{
  inline
  dfloat::dfloat(Sign sign_, uint64_t mant_, int8_t pow_)
    : sign(sign_),
      mant(mant_),
      pow(pow_)
  {

  }

  template <
    typename T,
    typename std::enable_if_t<std::is_integral<T>::value, bool> = true>
  inline
  dfloat::dfloat(T value)
    : dfloat((int64_t)value)
  {
    
  }

  inline
  dfloat::dfloat(int64_t value)
  {
    if (value == 0)
    {
      sign = Sign::ZERO;
      return;
    }
    else if (value > 0)
    {
      sign = Sign::POS;
      mant = value;
    }
    else
    {
      sign = Sign::NEG;
      mant = -value;
    }

    pow = SCALE_POW;
    while (mant < SCALE)
    {
      mant *= BASE;
      --pow;
    }
    while (mant >= MANT_CAP)
    {
      mant /= BASE;
      ++pow;
    }
  }

  inline
  dfloat::dfloat(uint64_t value)
  {
    if (value == 0)
    {
      sign = Sign::ZERO;
      return;
    }
    else
    {
      sign = Sign::POS;
      mant = value;
    }    

    pow = SCALE_POW;
    while (mant < SCALE)
    {
      mant *= BASE;
      --pow;
    }
    while (mant >= MANT_CAP)
    {
      mant /= BASE;
      ++pow;
    }
  }

  inline
  dfloat::dfloat(unsigned long long value)
    : dfloat((uint64_t)value)
  {

  }

  template <
    typename T,
    typename std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  inline
  dfloat::dfloat(T value)
  {
    if (value == 0)
    {
      sign = Sign::ZERO;
      return;
    }
    else if (!std::isfinite(value))
    {
      sign = Sign::_NAN_;
      return;
    }
    else if (value > 0)
    {
      sign = Sign::POS;
    }
    else
    {
      sign = Sign::NEG;
      value = -value;
    }

    /* scale value until it's between 1 and 10 */
    pow = 0;
    while (value < 1)
    {
      value *= BASE;
      --pow;
    }
    while (value >= BASE)
    {
      value /= BASE;
      ++pow;
    }

    mant = (uint64_t)(value * SCALE);

    /*
      I think there may be rounding errors when converting value to mant
      Let's do a final check in case
      */
    while (mant < SCALE)
    {
      mant *= BASE;
      --pow;
    }
    while (mant >= MANT_CAP)
    {
      mant /= BASE;
      ++pow;
    }
  }

  template <
    typename T,
    typename std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  inline
  dfloat::operator T() const
  {
    if (sign == Sign::ZERO)
    {
      return 0;
    }
    else if (sign == Sign::_NAN_)
    {
      return NAN;
    }

    T res = mant;
    res /= SCALE;

    /*
      Multiply by 10^pow
      This should be safe because IEEE754 uses 11 bits for the base-2 exponent
      and we use 8 bits for the base-10 exponent
        log10(2^(2^11)) >= log10(10^(2^8))
      */
    int8_t pow_to_zero = pow;
    while (pow_to_zero > 0)
    {
      res *= BASE;
      --pow_to_zero;
    }
    while (pow_to_zero < 0)
    {
      res /= BASE;
      ++pow_to_zero;
    }

    if (sign == Sign::POS)
    {
      return res;
    }
    else
    {
      return -res;
    }
  }

  inline
  bool dfloat::operator==(const dfloat& other) const
  {
    return 0 == _comparedTo(other);
  }

  inline
  bool dfloat::operator!=(const dfloat& other) const
  {
    return 0 != _comparedTo(other);
  }

  inline
  bool dfloat::operator>(const dfloat& other) const
  {
    return 1 == _comparedTo(other);
  }

  inline
  bool dfloat::operator<(const dfloat& other) const
  {
    return -1 == _comparedTo(other);
  }

  inline
  bool dfloat::operator>=(const dfloat& other) const
  {
    return 0 <= _comparedTo(other);
  }

  inline
  bool dfloat::operator<=(const dfloat& other) const
  {
    return 0 >= _comparedTo(other);
  }

  inline
  dfloat& dfloat::operator+=(const dfloat& other)
  {
    return operator=(operator+(other));
  }

  inline
  dfloat& dfloat::operator-=(const dfloat& other)
  {
    return operator=(operator-(other));
  }

  inline
  dfloat& dfloat::operator*=(const dfloat& other)
  {
    return operator=(operator*(other));
  }
  
  inline
  dfloat& dfloat::operator/=(const dfloat& other)
  {
    return operator=(operator/(other));
  }

  inline
  dfloat dfloat::operator-() const
  {
    switch (sign)
    {
      case Sign::NEG:
        return dfloat(Sign::POS, mant, pow);
      case Sign::ZERO:
        return dfloat(Sign::ZERO, 0, 0);
      case Sign::POS:
        return dfloat(Sign::NEG, mant, pow);
      case Sign::_NAN_:
      default:
        return dfloat(Sign::_NAN_, 0, 0);
    }
  }

  inline
  dfloat dfloat::operator+(const dfloat& other) const
  {
    /* edge case: either is nan */
    if (sign == Sign::_NAN_ or other.sign == Sign::_NAN_)
    {
      return dfloat(Sign::_NAN_, 0, 0);
    }
    
    /* edge case: lhs is zero */
    if (sign == Sign::ZERO)
    {
      return other;
    }

    /* edge case: rhs is zero */
    if (other.sign == Sign::ZERO)
    {
      return *this;
    }

    /* same sign: add magnitudes and copy over sign */
    if (sign == other.sign)
    {
      dfloat res;

      res.sign = sign;

      uint64_t a_mant = mant;
      int8_t a_pow = pow;
      uint64_t b_mant = other.mant;
      int8_t b_pow = other.pow;

      /* scale the smaller magnitude number to match the larger magnitude number */
      while (a_pow < b_pow)
      {
        ++a_pow;
        a_mant /= BASE;
      }
      while (a_pow > b_pow)
      {
        ++b_pow;
        b_mant /= BASE;
      }

      res.pow = a_pow;

      res.mant = a_mant + b_mant;

      if (res.mant >= MANT_CAP)
      {
        res.mant /= BASE;
        
        /* overflow results in NaN */
        if (res.pow >= MAX_POW)
        {
          res.sign = Sign::_NAN_;
        }
        else
        {
          ++res.pow;
        }
      }

      return res;
    }

    /* different sign: subtract smaller magnitude from larger magnitude and use larger magnitude's sign */
    else
    {
      dfloat res(Sign::ZERO, 0, 0);

      short compare = _compareMagnitudeTo(other);

      /* equal but opposite */
      if (compare == 0)
      {
        return res;
      }

      /* a will hold the larger magnitude number */
      uint64_t a_mant, b_mant;
      int8_t a_pow, b_pow;

      /* this is larger magnitude than other */
      if (compare > 0)
      {
        a_mant = mant;
        a_pow = pow;
        b_mant = other.mant;
        b_pow = other.pow;

        res.sign = sign;
      }
      /* this is smaller magnitude than other */
      else
      {
        a_mant = other.mant;
        a_pow = other.pow;
        b_mant = mant;
        b_pow = pow;

        res.sign = other.sign;
      }

      /* scale the smaller magnitude number to match the larger magnitude number */
      while (a_pow > b_pow)
      {
        ++b_pow;
        b_mant /= BASE;
      }

      res.pow = a_pow;

      /* at this point, a_mant should be bigger than b_mant, so it's safe to do an unsigned subtraction */
      res.mant = a_mant - b_mant;

      /* the difference may be small, i.e. below SCALE */
      while (res.mant < SCALE)
      {
        res.mant *= BASE;
        
        /* overflow results in NaN */
        if (res.pow <= MIN_POW)
        {
          res.sign = Sign::_NAN_;
          break;
        }
        else
        {
          --res.pow;
        }
      }

      return res;
    }
  }

  inline
  dfloat dfloat::operator-(const dfloat& other) const
  {
    return operator+(-other);
  }

  inline
  dfloat dfloat::operator*(const dfloat& other) const
  {
    /* edge case: either is NaN */
    if (sign == Sign::_NAN_ or other.sign == Sign::_NAN_)
    {
      return dfloat(Sign::_NAN_, 0, 0);
    }
    
    /* edge case: either is zero */
    if (sign == Sign::ZERO or other.sign == Sign::ZERO)
    {
      return dfloat(Sign::ZERO, 0, 0);
    }
    
    dfloat res;
    res.sign = (sign == other.sign) ? Sign::POS : Sign::NEG;
    
    int16_t new_pow = (int16_t)pow + (int16_t)other.pow;  // wider type in order to bounds check after final touches

    __uint128_t a = mant;
    __uint128_t b = other.mant;

    /*
      We can demote from __uint128_t to uint64_t because 
        99 million billion * 99 million billion / 100 million billion
        is less than 2^64 - 1
    */
    res.mant = (uint64_t)(a * b / SCALE);

    if (res.mant >= MANT_CAP)
    {
      res.mant /= BASE;
      ++new_pow;
    }
    
    /* overflow results in NaN */
    if (new_pow > MAX_POW or new_pow < MIN_POW)
    {
      res.sign = Sign::_NAN_;
    }
    else
    {
      res.pow = new_pow;
    }

    return res;
  }

  inline
  dfloat dfloat::operator/(const dfloat& other) const
  {
    /* edge case: either is NaN */
    if (sign == Sign::_NAN_ or other.sign == Sign::_NAN_)
    {
      return dfloat(Sign::_NAN_, 0, 0);
    }
    
    /* edge case: denominator zero */
    if (other.sign == Sign::ZERO)
    {
      return dfloat(Sign::_NAN_, 0, 0);
    }
    
    /* edge case: numerator is zero */
    if (sign == Sign::ZERO)
    {
      return dfloat(Sign::ZERO, 0, 0);
    }

    dfloat res;
    res.sign = (sign == other.sign) ? Sign::POS : Sign::NEG;
    
    int16_t new_pow = (int16_t)pow - (int16_t)other.pow;

    __uint128_t a = mant;
    __uint128_t b = other.mant;

    /* make it so that numerator is >= denominator, that way quotient will be between 1 and 10 */
    if (a < b)
    {
      a = a * BASE;
      --new_pow;
    }
    
    /* overflow results in NaN */
    if (new_pow > MAX_POW or new_pow < MIN_POW)
    {
      res.sign = Sign::_NAN_;
    }
    else
    {
      res.pow = new_pow;
    }

    /* multiply the numerator by scale before dividing */
    res.mant = (uint64_t)(a * SCALE / b);

    return res;
  }

  inline
  short dfloat::_comparedTo(const dfloat& other) const
  {
    /* if either is nan, there is no comparison */
    if (sign == Sign::_NAN_ or other.sign == Sign::_NAN_)
    {
      return 2;
    }
    
    switch (sign)
    {
      case Sign::NEG:
      {
        switch (other.sign)
        {
          case Sign::NEG:
            break;  // sign match, need to compare magnitude
          case Sign::ZERO:
          case Sign::POS:
            return -1;
          default:
            break;
        }
        break;
      }
      case Sign::ZERO:
      {
        switch (other.sign)
        {
          case Sign::NEG:
            return 1;
          case Sign::ZERO:
            return 0;
          case Sign::POS:
            return -1;
          default:
            break;
        }
        break;
      }
      case Sign::POS:
      {
        switch (other.sign)
        {
          case Sign::NEG:
          case Sign::ZERO:
            return 1;
          case Sign::POS:
            break;  // sign match, need to compare magnitude
          default:
            break;
        }
        break;
      }
      default:
        break;
    }

    /* flip comparison if operands are both NEG */
    if (sign == Sign::POS)
    {
      return _compareMagnitudeTo(other);
    }
    else
    {
      return -_compareMagnitudeTo(other);
    }
  }

  inline
  short dfloat::_compareMagnitudeTo(const dfloat& other) const
  {
    if (pow > other.pow)
    {
      return 1;
    }
    else if (pow < other.pow)
    {
      return -1;
    }
    else
    {
      if (mant > other.mant)
      {
        return 1;
      }
      else if (mant < other.mant)
      {
        return -1;
      }
      else
      {
        return 0;
      }
    }
  }

  inline
  dfloat::dfloat(const std::string& str)
  {
    /* edge case: str is empty */
    if (str.length() < 1)
    {
      sign = Sign::_NAN_;
      return;
    }

    sign = Sign::ZERO;
    mant = 0;
    pow = SCALE_POW;

    /* whether the string contains a decimal point */
    bool decimal_point = false;

    /* only true for the very first character */
    bool first_char = true;

    /* number of nonzero digits read so far */
    size_t nonzero_digits = 0;

    for (const char& c : str)
    {
      if (c == '-')
      {
        if (first_char)
        {
          sign = Sign::NEG;
        }
        else
        {
          sign = Sign::_NAN_;
          return;
        }
      }
      else if (c >= '0' and c <= '9')
      {
        if (c == '0')
        {
          /* ignore leading zeros */
          if (nonzero_digits == 0 and !decimal_point)
          { 
            /* need to set first_char if we are going to continue */
            first_char = false;
            continue;
          }
        }

        /* if mant already holds max digits, do not add any more, just increase power if above decimal point */
        if (mant >= SCALE)
        {
          if (!decimal_point)
          {
            if (pow++ >= MAX_POW)
            {
              sign = Sign::_NAN_;
              return;
            }
          }
        }
        /* otherwise, append digit to mant */
        else
        {
          mant = mant * 10 + (c - '0');

          /* if we are below decimal, we need to decrease power to compensate for appending to mant */
          if (decimal_point)
          {
            if (pow-- <= MIN_POW)
            {
              sign = Sign::_NAN_;
              return;
            }
          }
        }

        if (c != '0')
        {
          /* if first nonzero digit, set to POS sign (unless already NEG); then increment nonzer_digits */
          if (nonzero_digits++ == 0)
          {
            if (sign != Sign::NEG)
            {
              sign = Sign::POS;
            }
          }
        }
      }
      else if (c == '.')
      {
        if (!decimal_point)
        {
          decimal_point = true;
        }
        else
        {
          sign = Sign::_NAN_;
          return;
        }
      }
      else
      {
        sign = Sign::_NAN_;
        return;
      }


      first_char = false;
    }

    /* edge case: zero value */
    if (mant == 0)
    {
      sign = Sign::ZERO;
      return;
    }

    /* at the end, scale up the mantissa if necessary */
    while (mant < SCALE)
    {
      mant *= BASE;
      if (pow-- <= MIN_POW)
      {
        sign = Sign::_NAN_;
        return;
      }
    }
  }

  inline
  dfloat::operator std::string() const
  {
    std::stringstream ss;

    print(ss);

    return ss.str();
  }

  inline
  std::ostream& dfloat::print(std::ostream& stream) const
  {
    /* edge case - nan */
    if (sign == Sign::_NAN_)
    {
      stream << "NaN";
      return stream;
    }
    
    /* edge case - zero */
    if (sign == Sign::ZERO)
    {
      stream << '0';
      return stream;
    }

    /* save current format flags so we can reset our manipulators when we are done */
    std::ios_base::fmtflags orig_stream_flags(stream.flags());
    stream << std::resetiosflags(orig_stream_flags);

    if (sign == Sign::NEG)
    {
      stream << '-';
    }


    {
      /* if pow is zero, how many digits are above/below decimal point */
      int mant_digits_above = 1;
      int mant_digits_below = SCALE_POW;

      /* now shift the digits left/right in base-10 */
      int8_t pow_it = pow;
      uint64_t divisor = SCALE;
      while (pow_it > 0)
      {
        divisor /= BASE;
        ++mant_digits_above;
        --mant_digits_below;
        --pow_it;
      }
      while (pow_it < 0)
      {
        divisor *= BASE;
        --mant_digits_above;
        ++mant_digits_below;
        ++pow_it;
      }

      /* print digits above decimal */
      if (mant_digits_above > 0)
      {
        if (mant_digits_above <= SCALE_POW + 1)
        {
          stream << (mant / divisor);
        }
        else
        {
          stream << std::setw(mant_digits_above) << std::setfill('0') << std::left << mant;
        }
      }
      else
      {
        stream << '0';
      }

      /* print below decimal only if there are nonzero digits */
      if (mant_digits_below > 0)
      {
        uint64_t mant_below;
        int mant_nz_digits_below = mant_digits_below;
        if (mant_digits_below >= SCALE_POW + 1)
        {
          mant_below = mant;
        }
        else
        {
          mant_below = mant % divisor;
        }

        /* do not print trailing zeros */
        for (int8_t i = 0; i < mant_digits_below; i++)
        {
          if (mant_below % BASE != 0)
          {
            break;
          }
          else
          {
            --mant_nz_digits_below;
            mant_below /= BASE;
          }
        }

        if (mant_nz_digits_below > 0)
        {
          stream << '.';
          stream << std::setw(mant_nz_digits_below) << std::setfill('0') << std::right << mant_below;
        }
      }
    }

    /* reset manipulators */
    stream.flags(orig_stream_flags);

    return stream;
  }
  
  inline
  bool dfloat::isfinite(const dfloat& d)
  {
    return d.sign != Sign::_NAN_;
  }

  template <typename T>
  inline
  dfloat operator+(T x, const dfloat& d)
  {
    return dfloat(x) + d;
  }

  template <typename T>
  inline
  dfloat operator-(T x, const dfloat& d)
  {
    return dfloat(x) - d;
  }

  template <typename T>
  inline
  dfloat operator*(T x, const dfloat& d)
  {
    return dfloat(x) * d;
  }

  template <typename T>
  inline
  dfloat operator/(T x, const dfloat& d)
  {
    return dfloat(x) / d;
  }

  template <typename T>
  inline
  bool operator==(T x, const dfloat& d)
  {
    return dfloat(x) == d;
  }

  /**
    @brief  operator!= free function with dfloat as right operand
    */
  template <typename T>
  inline
  bool operator!=(T x, const dfloat& d)
  {
    return dfloat(x) != d;
  }

  /**
    @brief  operator> free function with dfloat as right operand
    */
  template <typename T>
  inline
  bool operator>(T x, const dfloat& d)
  {
    return dfloat(x) > d;
  }

  /**
    @brief  operator< free function with dfloat as right operand
    */
  template <typename T>
  inline
  bool operator<(T x, const dfloat& d)
  {
    return dfloat(x) < d;
  }

  /**
    @brief  operator>= free function with dfloat as right operand
    */
  template <typename T>
  inline
  bool operator>=(T x, const dfloat& d)
  {
    return dfloat(x) >= d;
  }

  /**
    @brief  operator<= free function with dfloat as right operand
    */
  template <typename T>
  inline
  bool operator<=(T x, const dfloat& d)
  {
    return dfloat(x) <= d;
  }
}

inline
std::ostream& operator<<(std::ostream& stream, const xu::dfloat& d)
{
  return d.print(stream);
}