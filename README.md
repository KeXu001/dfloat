# dfloat
### Decimal floating point type for C++

`xu::dfloat` stores a decimal number with 18 digits of precision and exponent between [-128, 127)

Basic arithmetic (`+ - * /`) support. All operations are truncating (round towards zero) beyond 18 digits.

Uses fixed-width integer types from `<cstdint>` internally.

License information can be found in the LICENSE file.

### Performance

Not intended for highly-optimized applications. This class is slower than double by a factor of about 20:1.
