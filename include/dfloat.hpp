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
  dfloat::dfloat(Sign sign_, mant_t mant_, pow_t pow_)
    : sign(sign_),
      mant(mant_),
      pow(pow_)
  {

  }

  template <
    typename T,
    typename std::enable_if_t<
      std::is_integral<T>::value && std::is_unsigned<T>::value,
      bool> = true>
  inline
  dfloat::dfloat(T value)
  {
    if (value == 0)
    {
      sign = Sign::ZERO;
      return;
    }

    sign = Sign::POS;
    mant = value;    

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

  template <
    typename T,
    typename std::enable_if_t<
      std::is_integral<T>::value && std::is_signed<T>::value,
      bool> = true>
  inline
  dfloat::dfloat(T value)
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
      if (pow <= MIN_POW)
      {
        /* if underflow, make into denormal value or zero */
        mant = (mant_t)(value * SCALE);
        if (mant == 0)
        {
          sign = Sign::ZERO;
          return;
        }
        else
        {
          return;
        }
      }

      value *= BASE;
      --pow;
    }

    while (value >= BASE)
    {
      /* if overflow, make into nan */
      if (pow >= MAX_POW)
      {
        sign = Sign::_NAN_;
        return;
      }

      value /= BASE;
      ++pow;
    }

    mant = (mant_t)(value * SCALE);
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
      return std::numeric_limits<T>::quit_NaN();
    }

    T res = mant;
    res /= SCALE;

    /*
      Multiply by 10^pow
      This should be safe because IEEE754 uses 11 bits for the base-2 exponent
      and we use 8 bits for the base-10 exponent
        log10(2^(2^11)) >= log10(10^(2^8))
      */
    pow_t pow_to_zero = pow;
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
    ComparisonResult r = _comparedTo(other);

    return r == ComparisonResult::EQUAL;
  }

  inline
  bool dfloat::operator!=(const dfloat& other) const
  {
    ComparisonResult r = _comparedTo(other);

    return r != ComparisonResult::EQUAL and r != ComparisonResult::_NAN_;
  }

  inline
  bool dfloat::operator>(const dfloat& other) const
  {
    ComparisonResult r = _comparedTo(other);

    return r == ComparisonResult::MORE;
  }

  inline
  bool dfloat::operator<(const dfloat& other) const
  {
    ComparisonResult r = _comparedTo(other);

    return r == ComparisonResult::LESS;
  }

  inline
  bool dfloat::operator>=(const dfloat& other) const
  {
    ComparisonResult r = _comparedTo(other);

    return r == ComparisonResult::EQUAL or r == ComparisonResult::MORE;
  }

  inline
  bool dfloat::operator<=(const dfloat& other) const
  {
    ComparisonResult r = _comparedTo(other);

    return r == ComparisonResult::EQUAL or r == ComparisonResult::LESS;
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
  dfloat dfloat::operator+() const
  {
    return *this;
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

      mant_t a_mant = mant;
      pow_t a_pow = pow;
      mant_t b_mant = other.mant;
      pow_t b_pow = other.pow;

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

      ComparisonResult compare = _compareMagnitudeTo(other);

      /* equal but opposite */
      if (compare == ComparisonResult::EQUAL)
      {
        return res;
      }

      /* a will hold the larger magnitude number */
      mant_t a_mant, b_mant;
      pow_t a_pow, b_pow;

      /* this is larger magnitude than other */
      if (compare == ComparisonResult::MORE)
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
        /* underflow results in denormal value */
        if (res.pow <= MIN_POW)
        {
          break;
        }
        else
        {
          res.mant *= BASE;
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
    
    pow2_t new_pow = (pow2_t)pow + (pow2_t)other.pow;  // wider type in order to bounds check after final touches

    mant2_t a = mant;
    mant2_t b = other.mant;
    mant2_t new_mant = a * b / SCALE;

    if (new_mant >= MANT_CAP)
    {
      new_mant /= BASE;
      ++new_pow;
    }

    while (new_mant < SCALE)
    {
      new_mant *= BASE;
      --new_pow;
    }
    
    /* overflow results in nan */
    if (new_pow > MAX_POW)
    {
      res.sign = Sign::_NAN_;
      return res;
    }

    /* underflow results in denormal or zero */
    while (new_pow < MIN_POW)
    {
      new_mant /= 10;
      ++new_pow;

      if (new_mant == 0)
      {
        res.sign = Sign::ZERO;
        return res;
      }
    }

    res.pow = new_pow;
    res.mant = (mant_t)new_mant;

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
    
    pow2_t new_pow = (pow2_t)pow - (pow2_t)other.pow;

    mant2_t a = mant;
    mant2_t b = other.mant;
    mant2_t new_mant = a * SCALE / b;

    while (new_mant >= MANT_CAP)
    {
      new_mant /= BASE;
      ++new_pow;
    }

    if (new_mant < SCALE)
    {
      new_mant *= BASE;
      --new_pow;
    }

    /* overflow results in nan */
    if (new_pow > MAX_POW)
    {
      res.sign = Sign::_NAN_;
      return res;
    }

    /* underflow results in denormal or zero */
    while (new_pow < MIN_POW)
    {
      new_mant /= 10;
      ++new_pow;

      if (new_mant == 0)
      {
        res.sign = Sign::ZERO;
        return res;
      }
    }

    res.pow = new_pow;
    res.mant = (mant_t)new_mant;

    return res;
  }

  inline
  dfloat::ComparisonResult dfloat::_comparedTo(const dfloat& other) const
  {
    /* if either is nan, there is no comparison */
    if (sign == Sign::_NAN_ or other.sign == Sign::_NAN_)
    {
      return ComparisonResult::_NAN_;
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
            return ComparisonResult::LESS;
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
            return ComparisonResult::MORE;
          case Sign::ZERO:
            return ComparisonResult::EQUAL;
          case Sign::POS:
            return ComparisonResult::LESS;
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
            return ComparisonResult::MORE;
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
      ComparisonResult r = _compareMagnitudeTo(other);

      if (r == ComparisonResult::MORE)
      {
        r = ComparisonResult::LESS;
      }
      else if (r == ComparisonResult::LESS)
      {
        r = ComparisonResult::MORE;
      }

      return r;
    }
  }

  inline
  dfloat::ComparisonResult dfloat::_compareMagnitudeTo(const dfloat& other) const
  {
    if (pow > other.pow)
    {
      return ComparisonResult::MORE;
    }
    else if (pow < other.pow)
    {
      return ComparisonResult::LESS;
    }
    else
    {
      if (mant > other.mant)
      {
        return ComparisonResult::MORE;
      }
      else if (mant < other.mant)
      {
        return ComparisonResult::LESS;
      }
      else
      {
        return ComparisonResult::EQUAL;
      }
    }
  }

  /*
    State machine

      descr {action}                next char --> next state
    state                           +       -       0       1-9     eE      .       end     other
    ------------------------------------------------------------------------------------------------
    begin                           sign    sign    leadz   whole   fail    fail    fail    fail
      initial state
    sign                            fail    fail    leadz   whole   fail    fail    fail    fail
      just parsed a sign
    leadz                           fail    fail    leadz   whole   ze1     frac1   zero    fail
      zeros in front of decimal
    ze1                             zes     zes     ze2     ze2     fail    fail    fail    fail
      parsed an e/E after zero,
      expecting sign or digits
    zes                             fail    fail    ze2     ze2     fail    fail    fail    fail
      parsed a sign after e/E
      after zero, expecting digits
    ze2                             fail    fail    ze2     ze2     fail    fail    zero    fail
      parsed digits after e/E
      after zero, expecting digits
    whole                           fail    fail    whole   whole   e1      frac1   done    fail
      already saw first digit, in
      integral part
    frac1                           fail    fail    frac2   frac2   fail    fail    fail    fail
      parsed a decimal point,
      expecting digits or exp
    frac2                           fail    fail    frac2   frac2   e1      fail    done    fail
      parsed digits after decimal
      point, expecting digits or
      exp
    e1                              es      es      e2      e2      fail    fail    fail    fail
      parsed an e/E, expecting
      sign or digits
    es                              fail    fail    e2      e2      fail    fail    fail    fail
      parsed a sign after e/E,
      expecting digits
    e2                              fail    fail    e2      e2      fail    fail    done    fail
      parsed digits after e/E,
      expecting digits

    zero
      {return 0}
    fail
      {return nan}
    done
      {return number}

  */
  inline
  dfloat dfloat::parse(const std::string& str)
  {
    Sign sign = Sign::POS;
    mant_t mant = 0;
    pow_t pow = SCALE_POW;

    sign_t exp_sign = 1;
    pow_t exp_pow = 0;

    auto it = str.begin();

    if (it == str.end())
    {
      goto dfloat_parse_fail;
    }

    switch (*it)
    {
      case '+':
      {
        goto dfloat_parse_sign;
      }
      case '-':
      {
        goto dfloat_parse_sign;
      }
      case '0':
      {
        goto dfloat_parse_leadz;
      }
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_whole;
      }
      case '.':
      {
        goto dfloat_parse_fail;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_sign:
    if (*it == '+')
    {
      sign = Sign::POS;
    }
    else
    {
      sign = Sign::NEG;
    }

    if (++it == str.end())
    {
      goto dfloat_parse_fail;
    }

    switch (*it)
    {
      case '0':
      {
        goto dfloat_parse_leadz;
      }
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_whole;
      }
      case '.':
      {
        goto dfloat_parse_fail;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_leadz:
    /* no action: ignore leading zeroes */

    if (++it == str.end())
    {
      goto dfloat_parse_zero;
    }

    switch (*it)
    {
      case '0':
      {
        goto dfloat_parse_leadz;
      }
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_whole;
      }
      case 'e':
      case 'E':
      {
        goto dfloat_parse_ze1;
      }
      case '.':
      {
        goto dfloat_parse_frac1;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_ze1:
    /* no action: prepare to parse exponent */

    if (++it == str.end())
    {
      goto dfloat_parse_fail;
    }

    switch (*it)
    {
      case '+':
      case '-':
      {
        goto dfloat_parse_zes;
      }
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_ze2;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_zes:
    /* no action: doesn't matter what sign exponent is after zero */

    if (++it == str.end())
    {
      goto dfloat_parse_fail;
    }

    switch (*it)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_ze2;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_ze2:
    /* no action: doesn't matter what exponent is after zero */
    
    if (++it == str.end())
    {
      goto dfloat_parse_zero;
    }

    switch (*it)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_ze2;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_whole:
    /*
      If mant is would exceed its maximum, we must truncate. This results in
      data loss if the digit is not '0'
    */
    if (mant >= SCALE)
    {
      /*
        If we cannot increment power any further, then the whole number part is
        out of range. Even if the exponent were to bring the result back into
        range, we will return NaN
      */
      if (pow >= MAX_POW)
      {
        goto dfloat_parse_fail;
      }
      else
      {
        ++pow;
      }
    }
    /* If mant is still small, we can just append to mant */
    else
    {
      /* we only transition into whole state after a digit 0-9 */
      mant = mant * BASE + ((*it) - '0');
    }
    
    if (++it == str.end())
    {
      goto dfloat_parse_done;
    }

    switch (*it)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_whole;
      }
      case 'e':
      case 'E':
      {
        goto dfloat_parse_e1;
      }
      case '.':
      {
        goto dfloat_parse_frac1;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_frac1:
    if (++it == str.end())
    {
      goto dfloat_parse_fail;
    }

    switch (*it)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_frac2;
      }
      case 'e':
      case 'E':
      {
        goto dfloat_parse_fail;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_frac2:
    /*
      If mant is would exceed its maximum, we must truncate. This results in
      data loss if the digit is not '0'
    */
    if (mant >= SCALE)
    {
      /* effectively ignoring any decimal places that are too small */
    }
    /* If mant is still small, we can append to mant and decrement pow */
    else
    {
      /*
        If we cannot decrement power any further, then the fractional part is
        out of range. Even if the exponent were to bring the result back into
        range, we will return NaN
      */
      if (pow <= MIN_POW)
      {
        goto dfloat_parse_fail;
      }
      else
      {
        --pow;
        /* at this point we know it's a digit and not a decimal point */
        mant = mant * BASE + ((*it) - '0');
      }
    }

    if (++it == str.end())
    {
      goto dfloat_parse_done;
    }

    switch (*it)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_frac2;
      }
      case 'e':
      case 'E':
      {
        goto dfloat_parse_e1;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_e1:
    /* Make sure mant is between SCALE and SCALE*BASE before proceeding */
    while (mant < SCALE)
    {
      if (pow <= MIN_POW)
      {
        goto dfloat_parse_fail;
      }

      mant *= BASE;
      --pow;
    }

    if (++it == str.end())
    {
      goto dfloat_parse_fail;
    }

    switch (*it)
    {
      case '+':
      case '-':
      {
        goto dfloat_parse_es;
      }
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_e2;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_es:
    if (*it == '+')
    {
      exp_sign = 1;
    }
    else
    {
      exp_sign = -1;
    }

    if (++it == str.end())
    {
      goto dfloat_parse_fail;
    }

    switch (*it)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_e2;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_e2:
    /* Make sure exp_pow will not fall out of range if we append */
    if (exp_sign > 0)
    {
      if (exp_pow > MAX_POW / BASE)
      {
        goto dfloat_parse_fail;
      }

      exp_pow *= BASE;

      pow_t add_pow = (*it) - '0';

      /* MAX_POW - exp_pow is valid here because of integer promotion */
      if (add_pow > MAX_POW - exp_pow)
      {
        goto dfloat_parse_fail;
      }
      
      exp_pow += add_pow;
    }
    else
    {
      if (exp_pow < MIN_POW / BASE)
      {
        goto dfloat_parse_fail;
      }

      exp_pow *= BASE;

      pow_t subtract_pow = (*it) - '0';

      /* MIN_POW - exp_pow is valid here because of integer promotion */
      if (subtract_pow < MIN_POW - exp_pow)
      {
        goto dfloat_parse_fail;
      }
      
      exp_pow -= subtract_pow;
    }

    if (++it == str.end())
    {
      goto dfloat_parse_done;
    }

    switch (*it)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        goto dfloat_parse_e2;
      }
      default:
      {
        goto dfloat_parse_fail;
      }
    }

dfloat_parse_zero:
    return dfloat(Sign::ZERO, 0, 0);

dfloat_parse_fail:
    return dfloat(Sign::_NAN_, 0, 0);

dfloat_parse_done:
    /* Make sure mant is between SCALE and SCALE*BASE before proceeding */
    while (mant < SCALE)
    {
      if (pow <= MIN_POW)
      {
        goto dfloat_parse_fail;
      }

      mant *= BASE;
      --pow;
    }

    /*
      Add in the exponent parsed, if any
      Comparison is valid here because it will promote the integers
    */
    if (pow + exp_pow > MAX_POW or pow + exp_pow < MIN_POW)
    {
      goto dfloat_parse_fail;
    }

    pow += exp_pow;

    return dfloat(sign, mant, pow);
  }

  inline
  std::string dfloat::to_string(const dfloat& d, pow2_t exp_thresh)
  {
    std::stringstream ss;

    d.print_to(ss, exp_thresh);

    return ss.str();
  }

  inline
  std::ostream& dfloat::print_to(std::ostream& stream, pow2_t exp_thresh) const
  {
    /* edge case - nan */
    if (sign == Sign::_NAN_)
    {
      stream << "nan";
      return stream;
    }
    
    /* edge case - zero */
    if (sign == Sign::ZERO)
    {
      if (exp_thresh > 0)
      {
        stream << '0';
        return stream;
      }
      else
      {
        stream << "0.0e0";
        return stream;
      }
    }

    if (sign == Sign::NEG)
    {
      stream << '-';
    }

    /* Use scientific notation if past exponent threshold */
    if (pow >= exp_thresh or pow <= -exp_thresh)
    {
      mant_t mant_it = mant;
      size_t place = 0;

      while (mant_it != 0)
      {
        mant_t quo = mant_it / SCALE;
        mant_t rem = mant_it % SCALE;

        char digit = '0' + quo;

        stream << digit;

        if (place == 0)
        {
          stream << '.';
        }

        ++place;

        mant_it = rem * BASE;
      }

      /*
        If we only printed a single digit, include a single trailing zero after
        the decimal point
      */
      if (place == 1)
      {
        stream << '0';
      }

      stream << 'e';

      /* edge case: exponent is zero */
      if (pow == 0)
      {
        stream << '0';
      }
      else
      {
        /* enough digits to capture the power */
        const size_t POW_BUF_SIZE = 3;
        char pow_buf[POW_BUF_SIZE] = {0};
        size_t pow_idx = POW_BUF_SIZE - 1;  // initially set to the last index of `pow_buf`

        pow_t pow_it = pow;

        if (pow_it < 0)
        {
          stream << '-';

          pow_it = -pow_it;
        }

        while (pow_it != 0)
        {
          pow_t quo = pow_it / BASE;
          pow_t rem = pow_it % BASE;

          char digit = '0' + rem;

          pow_buf[pow_idx--] = digit;

          pow_it = quo;
        }

        for (pow_idx = 0; pow_idx < POW_BUF_SIZE; pow_idx++)
        {
          if (pow_buf[pow_idx] != 0)
          {
            stream << pow_buf[pow_idx];
          }
        }
      }
    }
    /* Otherwise use decimal notation */
    else
    {
      /* Print mantissa */
      mant_t mant_it = mant;
      pow_t pow_it = pow;

      if (pow_it < 0)
      {
        stream << "0.";
      }

      while (pow_it < -1)
      {
        stream << '0';
        ++pow_it;
      }

      while (mant_it != 0)
      {
        mant_t quo = mant_it / SCALE;
        mant_t rem = mant_it % SCALE;

        char digit = '0' + quo;

        stream << digit;

        if (pow_it == 0 and rem != 0)
        {
          stream << '.';
        }

        --pow_it;
        mant_it = rem * BASE;
      }

      while (pow_it >= 0)
      {
        stream << '0';
        --pow_it;
      }
    }

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
  return d.print_to(stream);
}