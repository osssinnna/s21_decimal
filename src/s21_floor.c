#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  int res = 0;
  s21_decimal value_zero = {{0}};
  int val = s21_is_less(value, value_zero);
  if (val == 0) {
    s21_truncate(value, result);
  } else {
    s21_decimal value_int = {{0}};
    s21_decimal value_res = {{0}};
    s21_truncate(value, &value_int);
    load_bit(&value.bits[3], 31, 0);
    load_bit(&value_int.bits[3], 31, 0);
    s21_sub(value, value_int, &value_res);
    if (s21_is_less_or_equal(value_res, value_zero) == 0) {
      s21_decimal value_1 = {{1, 0}};
      s21_add(value_int, value_1, result);
    } else {
      s21_truncate(value, result);
    }
    load_bit(&result->bits[3], 31, 1);
  }
  return res;
}
