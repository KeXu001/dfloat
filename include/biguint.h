
#pragma once

#include <cstdint>

namespace xu
{
  /**
    @brief  Unsigned integer type with width parameter
            This class implements an unsigned integer whose width is some multiple of 32 bits
    @tparam w
            Width of the biguint (in units of 32-bit words)
    */
  template <size_t w>
  class biguint
  {
  public:
    /**
      @brief  Number of bits in one word
      */
    static const size_t WORD_SIZE = 32;

  public:
    /**
      @brief  Default constructor
              Construct zero-initialized biguint
      */
    biguint();

    biguint(const biguint<w>& other);

    //  ===========
    //  Conversions
    //  ===========

    biguint(const uint32_t value);

    biguint(const uint64_t value);

    operator uint32_t() const;

    operator uint64_t() const;

    //  =================
    //  Bitwise Operators
    //  =================

    biguint<w> operator~() const;

    biguint<w> operator&(const biguint<w>& other) const;

    biguint<w> operator|(const biguint<w>& other) const;

    biguint<w> operator^(const biguint<w>& other) const;

    biguint<w> operator<<(size_t shift) const;

    biguint<w> operator>>(size_t shift) const;

    //  ====================
    //  Comparison Operators
    //  ====================

    bool operator==(const biguint<w>& other) const;

    bool operator!=(const biguint<w>& other) const;

    bool operator>(const biguint<w>& other) const;

    bool operator<(const biguint<w>& other) const;

    bool operator<=(const biguint<w>& other) const;

    bool operator>=(const biguint<w>& other) const;

    //  ====================
    //  Arithmetic Operators
    //  ====================

    biguint<w> operator+(const biguint<w>& other) const;

    /**
      @brief  Subtract another biguint
      @note   Assumes two's complement representation of words
      */
    biguint<w> operator-(const biguint<w>& other) const;

    /**
      @brief  Multiply by another biguint
              Performs multiplication in parts. Operation is modulo n (will overflow silently)
      */
    biguint<w> operator*(const biguint<w>& other) const;

    /**
      @brief  Divide by another biguint
              Performs long division. Operation is truncating (round down)
      @throw  std::runtime_error
              If divisor is zero
      */
    biguint<w> operator/(const biguint<w>& other) const;

    //  =============
    //  Member Access
    //  =============

    /**
      @brief  Returns the 32-bit word at index

      @param  at
              Index of word in range [0, w)
      @throw  std::out_of_range
              If index is out of range
      */
    uint32_t operator[](size_t at) const;

  protected:
    //  ================
    //  Helper Functions
    //  ================

    void _leftShiftOneBit();

    void _leftShiftOneWord();

    void _rightShiftOneBit();

    void _rightShiftOneWord();

    /**
      @brief  Returns which operand is greater
      @return 1 if greater than other, -1 if less than other, 0 if equal
      */
    short _comparedTo(const biguint<w>& other) const;

    /**
      @brief  Returns value of bit at specified index
      @param  at
              Index of bit
      @return 0 or 1
      @throw  std::out_of_range
              If index is out of range
      */
    uint32_t _getBit(size_t at) const;

    /**
      @brief  Sets the bit at specified index
      @param  at
              Index of bit
      @param  to
              0 if bit should be cleared, nonzero if bit should be set
      */
    void _setBit(size_t at, uint32_t to);

    /**
      @brief  Returns whether value is zero
      */
    bool _isZero() const;

  protected:
    //  ================
    //  Member Variables
    //  ================

    /**
      @brief  Array of 32-bit words making up the value
              Words are stored low-to-high power (lowest magnitude is always in the 0th index)
      */
    uint32_t words[w];
  };
}