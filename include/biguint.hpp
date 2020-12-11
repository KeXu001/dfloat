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

#include "biguint.h"

namespace xu
{
  /**
    @brief      Inline helper function for biguint addition
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
    uint32_t carry = 0;

    /*
      If truncated sum is less than either of the terms, then it must have wrapped around

      If c is no greater than 1, then only one of the below increment operations may occur at most
      */

    if ((out = a + b) < b)
    {
      carry++;
    }

    if ((out = out + c) < c)
    {
      carry++;
    }

    return carry;
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

    for (unsigned i = 0; i < w; i++)
    {
      uint64_t a = words[i];

      if (a == 0)
      {
        continue;
      }

      /* product of words[i] * other */
      biguint part;

      for (unsigned j = 0; j < w; j++)
      {
        uint64_t b = other.words[j];

        if (b == 0)
        {
          continue;
        }

        if (i + j >= w)
        {
          continue;
        }

        /* add product of words[i] * other.words[j] */
        part = part + (biguint(a * b) << (WORD_SIZE * (i + j)));
      }

      res = res + part;
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

    for (unsigned at = w * WORD_SIZE - 1;; at--)
    {
      if (other<=dividend)\
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
  uint32_t biguint<w>::operator[](unsigned at) const
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