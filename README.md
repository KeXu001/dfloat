# dfloat
### Decimal floating point type for C++

`xu::dfloat` stores a decimal number with 18 digits of precision and exponent between [-128, 127)

Basic arithmetic operation (+ - \* /) support. All operations are truncating (round towards zero) beyond 18 digits.

Uses fixed-width integer types from `<cstdint>` internally.

License information can be found in the LICENSE file.

### Performance

Not intended for highly-optimized applications.

Rough comparison (on the order of 1 million operations):

| Test | Type | Opt (g++) | Elapsed | Ratio (Higher is worse) |
| ---- | ---- | --------- | ------- | --------------- |
| sum += (a+b+c+d+e) | double | -O0 | 0.00520982 | 1:1 |
| sum += (a-b-c-d-e) | double | -O0 | 0.00524061 | 1:1 |
| sum += (a\*b\*c\*d\*e) | double | -O0 | 0.00537364 | 1:1 |
| sum += (a/b/c/d/e) | double | -O0 | 0.00868209 | 1:1 |
| sum += (a+b+c+d+e) | **dfloat** | -O0 | 0.0983526 | **19:1** |
| sum += (a-b-c-d-e) | **dfloat** | -O0 | 0.112579 | **22:1** |
| sum += (a\*b\*c\*d\*e) | **dfloat** | -O0 | 16.3602 | **3045:1** |
| sum += (a/b/c/d/e) | **dfloat** | -O0 | 17.0358 | **1962:1** |
| sum += (a+b+c+d+e) | double | -O3 | 0.000992855 | 1:1 |
| sum += (a-b-c-d-e) | double | -O3 | 0.000978037 | 1:1 |
| sum += (a\*b\*c\*d\*e) | double | -O3 | 0.00102609 | 1:1 |
| sum += (a/b/c/d/e) | double | -O3 | 0.0011253 | 1:1 |
| sum += (a+b+c+d+e) | **dfloat** | -O3 | 0.0412338 | **42:1** |
| sum += (a-b-c-d-e) | **dfloat** | -O3 | 0.0400112 | **41:1** |
| sum += (a\*b\*c\*d\*e) | **dfloat** | -O3 | 0.868846 | **847:1** |
| sum += (a/b/c/d/e) | **dfloat** | -O3 | 0.923355 | **821:1** |

