# dfloat
### Decimal floating point type for C++

`xu::dfloat` stores a decimal number with 18 digits of precision and exponent between [-128, 127)

Basic arithmetic (`+ - * /`) support. All operations are truncating (round towards zero) to 18 digits.

Requires GCC's `__uint128_t`. Uses fixed-width integer types internally.

Also requires C++11.

License information can be found in the LICENSE file.

### Performance

A rough test suggests that `xu::dfloat` is slower than `double` by a factor on the order of 20:1 on a x86_64 machine.
