#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  *result = value;
  result->bits[3] = value.bits[3] ^ 1 << 31;
  return 0;
}