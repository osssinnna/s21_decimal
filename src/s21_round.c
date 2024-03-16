#include "./s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  s21_decimal null = {0}, result2 = {0}, one = {{0b1, 0}};
  int ostatok = 0, bag = 0;
  *result = null;
  big_decimal number = to_big_decimal(value);
  big_decimal copyNumber = number, ten = {{0b1010, 0}};
  if (!convert_pow(number)) {
    *result = value;
  } else {
    if (!equal_null(number)) {
      repeat_exponent(convert_pow(number) - 1, &copyNumber);
      from_big_decimal_to_decimal(copyNumber, &value);
      s21_truncate(value, &result2);
      copyNumber = to_big_decimal(result2);
      value = result2;
      result2 = null;
      repeat_exponent(28, &copyNumber);
      ostatok = segmentation(copyNumber, ten, &copyNumber);
      value.bits[3] = value.bits[3] | 1 << 16;
      s21_truncate(value, &result2);
      if (ostatok == 1 || ostatok == 0) {
        if (!sign(value.bits[3])) {
          bag = s21_add(result2, one, &result2);
        } else {
          bag = s21_sub(result2, one, &result2);
        }
      }
      *result = result2;
    }
  }
  return bag;
}
