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

#include "biguint.h"

namespace xu
{
  /**
    @brief      Inline helper function for biguint addition and multiplication
    @param[in]  a
                First operand to add
    @param[in]  b
                Second operand to add
    @param[in]  c
                Third operand to add
    @param[out] out
                Where to store the summed value (modulo n)
    @return     Value to carry over (0, 1, or 2)
  */
  inline uint32_t addCarry(
    const uint32_t& a,
    const uint32_t& b,
    const uint32_t& c,
    uint32_t& out)
  {
    /* we store the output separately until assignment in case `out` is also one of the input operands */
    uint32_t res;

    uint32_t carry = 0;

    /*
      If truncated sum is less than either of the terms, then it must have wrapped around

      If c is no greater than 1, then only one of the below increment operations may occur at most
      */

    if ((res = a + b) < b)
    {
      carry++;
    }

    if ((res = res + c) < c)
    {
      carry++;
    }

    out = res;

    return carry;
  }

  /**
    @brief      Inline helper function for biguint multiplication
                Simply peforms 64-bit multiplication and splits result into two 32-bit words
    @param[in]  a
                First operand of multiply
    @param[in]  b
                Second operand of multiply
    @param[out] out_upper
                Upper word of the product
    @param[out] out_lower
                Lower word of the product
    */
  inline void multiplyParts(
    const uint32_t& a,
    const uint32_t& b,
    uint32_t& out_upper,
    uint32_t& out_lower)
  {
    uint64_t out = (uint64_t)a * b;
    out_lower = out & 0xFFFFFFFF;
    out_upper = out >> 32;
  }

  template <unsigned w>
  biguint<w>::biguint()
    : words{0}
  {
    
  }

  template <unsigned w>
  biguint<w>::biguint(const biguint<w>& other)
  {
    for (unsigned i = 0; i < w; i++)
    {
      words[i] = other.words[i];
    }
  }

  template <unsigned w>
  biguint<w>::biguint(const uint32_t value)
    : words{0}
  {
    static_assert(w >= 1, "Value too wide for biguint");

    words[0] = value;
  }

  template <unsigned w>
  biguint<w>::biguint(const uint64_t value)
    : words{0}
  {
    static_assert(w >= 2, "Value too wide for biguint");

    words[0] = value & 0xFFFFFFFF;
    words[1] = value >> WORD_SIZE;
  }

  template <unsigned w>
  biguint<w>::operator uint32_t() const
  {
    return words[0];
  }

  template <unsigned w>
  biguint<w>::operator uint64_t() const
  {
    return ((uint64_t)words[1] << WORD_SIZE) |
      ((uint64_t)words[0]);
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator~() const
  {
    biguint res;

    for (unsigned i = 0; i < w; i++)
    {
      res.words[i] = ~words[i];
    }

    return res;
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator&(const biguint<w>& other) const
  {
    biguint res;

    for (unsigned i = 0; i < w; i++)
    {
      res.words[i] = words[i] & other.words[i];
    }

    return res;
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator|(const biguint<w>& other) const
  {
    biguint res;

    for (unsigned i = 0; i < w; i++)
    {
      res.words[i] = words[i] | other.words[i];
    }

    return res;
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator^(const biguint<w>& other) const
  {
    biguint res;

    for (unsigned i = 0; i < w; i++)
    {
      res.words[i] = words[i] ^ other.words[i];
    }

    return res;
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator<<(unsigned shift) const
  {
    biguint res(*this);

    while (shift >= WORD_SIZE)
    {
      res._leftShiftOneWord();

      shift -= 32;
    }

    while (shift > 0)
    {
      res._leftShiftOneBit();

      shift--;
    }

    return res;
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator>>(unsigned shift) const
  {
    biguint res(*this);

    while (shift >= WORD_SIZE)
    {
      res._rightShiftOneWord();

      shift -= 32;
    }

    while (shift > 0)
    {
      res._rightShiftOneBit();

      shift--;
    }

    return res;
  }

  template <unsigned w>
  bool biguint<w>::operator==(const biguint<w>& other) const
  {
    return 0 == _comparedTo(other);
  }

  template <unsigned w>
  bool biguint<w>::operator!=(const biguint<w>& other) const
  {
    return 0 != _comparedTo(other);
  }

  template <unsigned w>
  bool biguint<w>::operator>(const biguint<w>& other) const
  {
    return 1 == _comparedTo(other);
  }

  template <unsigned w>
  bool biguint<w>::operator<(const biguint<w>& other) const
  {
    return -1 == _comparedTo(other);
  }

  template <unsigned w>
  bool biguint<w>::operator>=(const biguint<w>& other) const
  {
    return 0 <= _comparedTo(other);
  }

  template <unsigned w>
  bool biguint<w>::operator<=(const biguint<w>& other) const
  {
    return 0 >= _comparedTo(other);
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator+(const biguint<w>& other) const
  {
    biguint res;

    uint32_t carry = 0;
    for (unsigned i = 0; i < w; i++)
    {
      carry = addCarry(words[i], other.words[i], carry, res.words[i]);
    }

    return res;
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator-(const biguint<w>& other) const
  {
    return operator+(~other + 1u);
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator*(const biguint<w>& other) const
  {
    biguint res;

    /* product of the two words, split into two words */
    uint32_t p_upper;
    uint32_t p_lower;

    for (unsigned i = 0; i < w; i++)
    {
      uint32_t a = words[i];

      if (a == 0)
      {
        continue;
      }

      for (unsigned j = 0; j < w; j++)
      {
        uint32_t b = other.words[j];

        if (b == 0)
        {
          continue;
        }

        /* this is the word index where the product will go */
        unsigned k = i + j;

        if (k >= w)
        {
          continue;
        }

        multiplyParts(a, b, p_upper, p_lower);

        /*
          The product of the parts needs to be added to the total (res) at the
          correct word index (aka left shift)

          Because the upper word of the product of parts may be nonzero, we need
          to add that to the result as well. We can do this by adding the lower
          word first, and then adding the upper word separately. Or, we can
          add the upper word to the next carry when doing the addition. We will
          use the latter method
          */
        uint32_t carry = p_upper;
        carry += addCarry(res[k], p_lower, 0, res[k]);
        while (++k < w and carry > 0)
        {
          carry = addCarry(res[k], 0, carry, res[k]);
        }
      }
    }

    return res;
  }

  template <unsigned w>
  biguint<w> biguint<w>::operator/(const biguint<w>& other) const
  {
    if (other._isZero())
    {
      throw std::runtime_error("Divide by zero error");
    }

    /* Perform long division, one bit at a time */

    biguint<w> res;

    biguint<w> dividend(_getBit(w * WORD_SIZE - 1));

    for (unsigned at = w * WORD_SIZE - 1; ; at--)
    {
      if (other <= dividend)
      {
        res._setBit(at, 1);

        dividend = dividend - other;
      }
      else
      {
        res._setBit(at, 0);
      }

      if (at > 0)
      {
        /* append next digit */
        dividend = (dividend << 1) | _getBit(at - 1);
      }
      /* since at is unsigned, we need to break manually rather than use a >= 0 condition */
      else
      {
        break;
      }
    }

    return res;
  }

  template <unsigned w>
  uint32_t& biguint<w>::operator[](unsigned at)
  {
    if (at >= w)
    {
      throw std::out_of_range("Word index is out of range");
    }

    return words[at];
  }

  template <unsigned w>
  void biguint<w>::_leftShiftOneBit()
  {
    for (unsigned i = 0; i < w - 1; i++)
    {
      unsigned ii = w - 1 - i;

      words[ii] = (words[ii] << 1) | (words[ii - 1] >> 31);
    }

    words[0] <<= 1;
  }

  template <unsigned w>
  void biguint<w>::_leftShiftOneWord()
  {
    for (unsigned i = 0; i < w - 1; i++)
    {
      unsigned ii = w - 1 - i;

      words[ii] = words[ii - 1];
    }

    words[0] = 0;;
  }

  template <unsigned w>
  void biguint<w>::_rightShiftOneBit()
  {
    for (unsigned i = 0; i < w - 1; i++)
    {
      words[i] = words[i + 1];
    }

    words[w - 1] = 0;
  }

  template <unsigned w>
  short biguint<w>::_comparedTo(const biguint<w>& other) const
  {
    for (unsigned i = 0; i < w; i++)
    {
      unsigned ii = w - 1 - i;

      if (words[ii] > other.words[ii])
      {
        return 1;
      }
      else if (words[ii] < other.words[ii])
      {
        return -1;
      }
    }

    return 0;
  }

  template <unsigned w>
  uint32_t biguint<w>::_getBit(unsigned at) const
  {
    /* currently a protected member function */
    // if (at >= w * WORD_SIZE)
    // {
    //   throw std::out_of_range("Bit index is out of range");
    // }

    unsigned word_idx = at / WORD_SIZE;
    unsigned bit_idx = at % WORD_SIZE;

    return ((words[word_idx] >> bit_idx) & 0x1);
  }

  template <unsigned w>
  void biguint<w>::_setBit(unsigned at, uint32_t to)
  {
    /* currently a protected member function */
    // if (at >= w * WORD_SIZE)
    // {
    //   throw std::runtime_error("Bit index is out of range");
    // }

    unsigned word_idx = at / WORD_SIZE;
    unsigned bit_idx = at % WORD_SIZE;

    uint32_t bit_msk = (0x1 << bit_idx);

    if (to == 0)
    {
      words[word_idx] &= ~bit_msk;
    }
    else
    {
      words[word_idx] = (words[word_idx] & ~bit_msk) | (bit_msk);
    }
  }

  template <unsigned w>
  bool biguint<w>::_isZero() const
  {
    for (unsigned i = 0; i < w; i++)
    {
      if (words[i] != 0)
      {
        return false;
      }
    }

    return true;
  }
}